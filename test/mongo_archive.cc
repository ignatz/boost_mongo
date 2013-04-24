#include <iostream>
#include <sstream>
#include <vector>
#include <gtest/gtest.h>

#include <boost/foreach.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/array.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/shared_ptr.hpp>
#include <boost/archive/xml_oarchive.hpp>
#include <boost/type_traits/is_same.hpp>
#include <boost/numeric/conversion/bounds.hpp>

#include "boost/archive/mongo_oarchive.hpp"
#include "boost/archive/mongo_iarchive.hpp"

#include "test/builtins.h"
#include "test/Plain.h"
#include "test/Simple.h"
#include "test/Poly.h"

using namespace boost::archive;
using boost::serialization::make_nvp;

struct ExternEqual {};
bool operator== (ExternEqual const&, ExternEqual const&) { return false; }


class MongoArchiveTest :
	public ::testing::Test
{
protected:
	MongoArchiveTest() :
		mBuilder(),
		out(mBuilder)
	{}

	mongo::BSONObj getObject()
	{
		return mBuilder.obj();
	}

	template<typename T>
	void serialize(T const& val)
	{
		out << make_nvp(name, val);
	}

	template<typename U>
	void deserialize(mongo::BSONObj const& obj, U& b) const
	{
		mongo_iarchive in(obj);
		in >> make_nvp(name, b);
	}

	static const char name[];

private:
	mongo::BSONObjBuilder mBuilder;
	mongo_oarchive out;
};

const char MongoArchiveTest::name [] = { "fancyName" };


template<typename T>
struct MongoBuiltin :
	public MongoArchiveTest
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


TEST_F(MongoArchiveTest, SimpleType)
{
	Simple a, b;
	a.n = static_cast<Simple::Name>(42);
	a.str = "hihihi";
	serialize(a);

	mongo::BSONObj obj = getObject();
	int e = obj.getField(name).embeddedObject().getField("enum").Int();
	ASSERT_EQ(a.str,  obj.getField(name).embeddedObject().getField("str").String());
	ASSERT_EQ((int)a.n, e);

	deserialize(obj, b);
	ASSERT_EQ(a, b);
}

TEST_F(MongoArchiveTest, AllMembersRegressionTest)
{
	boost::array<int, 42> a, b;
	serialize(a);

	mongo::BSONObj obj = getObject();
	mongo::BSONElement elem = obj.getField(name).embeddedObject().getField("elems");
	ASSERT_EQ(static_cast<int>(a.size()+1), elem.embeddedObject().nFields());

	deserialize(obj, b);
	ASSERT_TRUE(std::equal(a.begin(), a.end(), b.begin()));
}

#include <boost/archive/xml_oarchive.hpp>
TEST_F(MongoArchiveTest, NotCompressableRegressionTest)
{
	ASSERT_TRUE (boost::has_equal_to<Simple>::value);
	ASSERT_TRUE (boost::has_equal_to<ExternEqual>::value);
	ASSERT_FALSE(boost::has_equal_to<Plain>::value);
	ASSERT_FALSE(boost::archive::detail::is_compressable<Plain>::value);

	std::vector<Plain> a(42);
	std::vector<Plain> b;

	serialize(a);
	deserialize(getObject(), b);

	ASSERT_EQ(a.size(), b.size());
}

TEST_F(MongoArchiveTest, Array)
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

TEST_F(MongoArchiveTest, SparseArray)
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
	oa << make_nvp(name, x);

	mongo::BSONObj obj = builder.obj();

	mongo::BSONElement elem = obj.getField(name).embeddedObject().getField("elems");
	ASSERT_EQ(range+1, elem.embeddedObject().nFields());

	mongo_iarchive in(obj, mongo_iarchive::sparse_array);
	in >> make_nvp(name, y);
	ASSERT_EQ(x, y);
}

TEST_F(MongoArchiveTest, Map)
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

TEST_F(MongoArchiveTest, Abstract)
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

TEST_F(MongoArchiveTest, SharedPtr)
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
