// Copyright 2013, Sebastian Jeltsch (sjeltsch@kip.uni-heidelberg.de)
// Distributed under the terms of the LGPLv3 or newer.

#include <gtest/gtest.h>
#include <iostream>
#include <sstream>

#include <boost/foreach.hpp>
#include "boost/archive/json_oarchive.hpp"
#include "boost/archive/json_iarchive.hpp"

#include "test/Simple.h"
#include "test/builtins.h"

using namespace boost::archive;
using boost::serialization::make_nvp;

template<typename T>
struct JSONBuiltin :
	public ::testing::Test
{
	typedef T type;
};

TYPED_TEST_CASE(JSONBuiltin, builtin_types);

TYPED_TEST(JSONBuiltin, BuiltinTypes)
{
	using namespace boost::numeric;
	typedef typename TestFixture::type type;

	const char name [] = { "myType" };
	size_t const size = 6;
	type a[size], b[size];
	a[0] = 0;
	a[1] = 1;
	a[2] = 42; // a randomly but carefully chosen number

	// long doubles is not supported by mongo. Limit values of long double
	// cast to double results in inf, which breaks serialization.
	if (!boost::is_same<type, long double>::value) {
		a[3] = bounds<type>::highest();
		a[4] = bounds<type>::smallest();
		a[5] = bounds<type>::lowest();
	} else {
		a[3] = a[4] = a[5] = 0;
	}

	std::stringstream ss;
	{
		json_oarchive out(ss);
		out << make_nvp(name, a);
	}
	ss.flush();

	ASSERT_TRUE(!ss.str().empty());

	json_iarchive in(ss);
	in >> make_nvp(name, b);

	for (size_t ii = 0; ii<size; ++ii)
	{
		EXPECT_EQ(a[ii], b[ii]);

		// older BSONElement interfaces have no safeNumberDouble() and
		// numberDouble produces <inf> values for limit doubles, therefore
		// exit earlier
		if (boost::is_same<type, double>::value && ii == 2)
			break;
	}
}

TEST(JSONArchive, Base)
{
	const char name [] = { "myType" };
	Simple a, b;
	a.a = 42;
	a.b = 23;

	std::stringstream ss;
	{
		json_oarchive out(ss);
		out << make_nvp(name, a);
	}
	ss.flush();

	json_iarchive in(ss);
	in >> make_nvp(name, b);

	ASSERT_EQ(a, b);
}

TEST(JSONArchive, Array)
{
	const char name [] = { "myArray" };
	typedef boost::array<int, 1024> type;
	type a, b;

	mongo::BSONObjBuilder builder;
	mongo_oarchive out(builder);

	size_t cnt = 0;
	BOOST_FOREACH(int& val, a)
	{
		val = cnt++;
	}

	std::stringstream ss;
	{
		json_oarchive out(ss);
		out << make_nvp(name, a);
	}
	ss.flush();

	json_iarchive in(ss);
	in >> make_nvp(name, b);

	ASSERT_EQ(a, b);
}
