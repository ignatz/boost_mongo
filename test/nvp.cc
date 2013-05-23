// Copyright 2013, Sebastian Jeltsch (sjeltsch@kip.uni-heidelberg.de)
// Distributed under the terms of the LGPLv3 or newer.

#include <gtest/gtest.h>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <list>

#include <boost/foreach.hpp>
#include "boost/archive/mongo_oarchive.hpp"
#include "boost/archive/mongo_iarchive.hpp"

#include "test/Simple.h"
#include "test/Poly.h"

using namespace boost::archive;
using boost::serialization::make_nvp;

void throw_if_empty_name(mongo::BSONObj const& obj)
{
	using namespace mongo;

	std::list<BSONElement> elems;
	obj.elems(elems);

	BOOST_FOREACH(BSONElement const& el, elems)
	{
		std::string name(el.fieldName());
		if (name.empty())
			throw std::length_error("empty fieldName");

		if (el.isABSONObj())
			throw_if_empty_name(el.Obj());
	}
}

TEST(NVP, NoEmptyFieldName)
{
	{
		Simple a;
		mongo::BSONObjBuilder builder;
		mongo_oarchive oa(builder);
		oa << BOOST_SERIALIZATION_NVP(a);
		mongo::BSONObj obj = builder.obj();

		ASSERT_NO_THROW(throw_if_empty_name(obj));
	}

	{
		Base const* a(new Poly (42, 5));
		mongo::BSONObjBuilder builder;
		mongo_oarchive oa(builder);
		oa << make_nvp("myBaseInstance", *a);
		mongo::BSONObj obj = builder.obj();

		ASSERT_NO_THROW(throw_if_empty_name(obj));
	}
}

struct NameLess
{
	int noNameInt;
	float noNameFloat;

	template<typename Archive>
	void serialize(Archive& ar, unsigned int const)
	{
		ar & make_nvp("", noNameInt)
		   & make_nvp("", noNameFloat);
	}
};

TEST(NVP, EmptyFieldName)
{
	{
		Simple a;
		mongo::BSONObjBuilder builder;
		mongo_oarchive oa(builder);
		oa << make_nvp("", a);
		mongo::BSONObj obj = builder.obj();

		ASSERT_THROW(throw_if_empty_name(obj), std::length_error);
	}

	{
		NameLess a;
		a.noNameInt = 23;
		a.noNameFloat = 3.141;
		mongo::BSONObjBuilder builder;
		mongo_oarchive oa(builder);
		oa << make_nvp("someName", a);
		mongo::BSONObj obj = builder.obj();

		ASSERT_THROW(throw_if_empty_name(obj), std::length_error);

		NameLess b;
		mongo_iarchive ia(obj);
		ia >> make_nvp("someName", b);

		ASSERT_EQ(a.noNameInt, b.noNameInt);
		ASSERT_EQ(a.noNameFloat, b.noNameFloat);
	}
}
