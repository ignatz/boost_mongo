// Copyright 2013, Sebastian Jeltsch (sjeltsch@kip.uni-heidelberg.de)
// Distributed under the terms of the LGPLv3 or newer.

#include <gtest/gtest.h>
#include <iostream>
#include <sstream>
#include <vector>

#include <boost/foreach.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/array.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/shared_ptr.hpp>
#include <boost/serialization/binary_object.hpp>
#include <boost/type_traits/is_same.hpp>
#include <boost/numeric/conversion/bounds.hpp>

#include "test/MongoArchive.h"
#include "test/builtins.h"
#include "test/Plain.h"
#include "test/Simple.h"
#include "test/Poly.h"

using namespace boost::archive;
using boost::serialization::make_nvp;


template<typename T>
struct MongoBuiltin :
	public MongoArchive
{
	typedef T type;
};

TYPED_TEST_CASE(MongoBuiltin, builtin_types);

TYPED_TEST(MongoBuiltin, BuiltinTypes)
{
	using namespace boost::numeric;
	typedef typename TestFixture::type type;

	size_t const size = 6;
	type a[size], b[size];
	a[0] = 0;
	a[1] = 1;
	a[2] = 42; // a randomly but carefully chosen number
	a[3] = bounds<type>::highest();
	a[4] = bounds<type>::smallest();
	a[5] = bounds<type>::lowest();

	this->serialize(a);

	mongo::BSONObj obj = this->getObject();
	ASSERT_TRUE(!obj.toString().empty());

	this->deserialize(obj, b);

	for (size_t ii = 0; ii<size; ++ii)
	{
		EXPECT_EQ(a[ii], b[ii]);

		// early abort for long double, it is not supported by mongo.
		// Therefore, the subsequent tests will inevitably fail.
		if (boost::is_same<type, long double>::value && ii == 2)
			break;
	}
}


template<typename T>
struct MongoString;

template<>
struct MongoString<std::string> :
	public MongoArchive
{
	typedef std::string type;
	type str() const
	{
		return type("this is the !@#$%^&* test string");
	}
};

template<>
struct MongoString<std::wstring> :
	public MongoArchive
{
	typedef std::wstring type;
	type str() const
	{
		return type(L"z\u00df\u6c34\U0001d10b");
	}
};

TYPED_TEST_CASE(MongoString, string_types);

TYPED_TEST(MongoString, StringTypes)
{
	std::setlocale(LC_ALL, "en_US.utf8");
	using namespace boost::numeric;
	typedef typename TestFixture::type type;

	type a(this->str());
	type b;

	this->serialize(a);

	mongo::BSONObj obj = this->getObject();
	ASSERT_TRUE(!obj.isEmpty());

	this->deserialize(obj, b);

	ASSERT_EQ(a, b);
}


TEST_F(MongoArchive, SimpleType)
{
	Simple a, b;
	a.n = static_cast<Simple::Name>(42);
	a.str = "hihihi";
	serialize(a);

	mongo::BSONObj obj = getObject();
	int e = obj.getField(getName()).embeddedObject().getField("enum").Int();
	ASSERT_EQ(a.str,  obj.getField(getName()).embeddedObject().getField("str").String());
	ASSERT_EQ((int)a.n, e);

	deserialize(obj, b);
	ASSERT_EQ(a, b);
}

TEST_F(MongoArchive, Array)
{
	boost::array<int, 1024> a, b;

	size_t cnt = 0;
	BOOST_FOREACH(int& val, a)
	{
		val = cnt++;
	}

	serialize(a);
	deserialize(getObject(), b);

	ASSERT_EQ(a, b);
}

TEST_F(MongoArchive, SparseArray)
{
	boost::array<int, 1024> x, y;

	// zero all entries first
	BOOST_FOREACH(int& val, x)
	{
		val = 0;
	}

	// set some to individual values
	int start=10, range=10;
	for(int ii = start; ii<start+range; ++ii) {
		x[ii] = ii;
	}
	ASSERT_NE(x, y);

	mongo::BSONObjBuilder builder;
	mongo_oarchive oa(builder, mongo_oarchive::sparse_array);
	oa << make_nvp(getName(), x);

	mongo::BSONObj obj = builder.obj();

	mongo::BSONElement elem = obj.getField(getName()).embeddedObject().getField("elems");
	ASSERT_EQ(range+1, elem.embeddedObject().nFields());

	mongo_iarchive in(obj, mongo_iarchive::sparse_array);
	in >> make_nvp(getName(), y);
	ASSERT_EQ(x, y);
}

TEST_F(MongoArchive, Map)
{
	std::map<std::string, Simple> x, y;
	x["one"].a = 42;
	x["one"].b = 23;
	x["two"].a = 5;
	x["three"].b = 1337;
	ASSERT_EQ(3u, x.size());
	ASSERT_NE(x, y);

	serialize(x);
	deserialize(getObject(), y);

	ASSERT_EQ(3u, y.size());
	ASSERT_EQ(x, y);
}

TEST_F(MongoArchive, Abstract)
{
	Base *x(new Poly (42, 5)), *y;

	Poly const& xp = dynamic_cast<Poly const&>(*x);
	ASSERT_EQ(42, xp.member);
	ASSERT_EQ(5 , x->member);

	serialize(x);
	deserialize(getObject(), y);

	Poly const& yp = dynamic_cast<Poly const&>(*y);
	ASSERT_EQ(xp, yp);
}

TEST_F(MongoArchive, SharedPtr)
{
	boost::shared_ptr<Base> x(new Poly (42, 5)), y;

	Poly const& xp = dynamic_cast<Poly const&>(*x);
	ASSERT_EQ(42, xp.member);
	ASSERT_EQ(5, x->member);

	serialize(x);
	deserialize(getObject(), y);

	Poly const& yp = dynamic_cast<Poly const&>(*y);
	ASSERT_EQ(xp, yp);
}

TEST_F(MongoArchive, Binary)
{
	using boost::serialization::make_binary_object;
	boost::array<uint8_t, 1024> x, y;

	// set all entries
	int cnt = 0;
	BOOST_FOREACH(uint8_t& val, x)
	{
		val = (cnt++)%x.size();
	}

	ASSERT_NE(x, y);

	mongo::BSONObjBuilder builder;
	mongo_oarchive oa(builder);
	oa << make_nvp(getName(),  make_binary_object(&x[0], x.size()));

	mongo::BSONObj obj = builder.obj();

	mongo_iarchive in(obj, mongo_iarchive::sparse_array);
	in >> make_nvp(getName(), make_binary_object(&y[0], y.size()));
	ASSERT_EQ(x, y);
}
