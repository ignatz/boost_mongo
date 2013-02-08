#include <gtest/gtest.h>
#include <iostream>
#include <sstream>

#include <boost/foreach.hpp>
#include "boost/archive/json_oarchive.hpp"
#include "boost/archive/json_iarchive.hpp"

#include "test/Dummy.h"

using namespace boost::archive;
using boost::serialization::make_nvp;

TEST(JSONArchive, Base)
{
	const char name [] = { "myType" };
	Dummy a, b;
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
