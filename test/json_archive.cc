#include <gtest/gtest.h>
#include <iostream>
#include <sstream>

#include "boost/archive/json_oarchive.hpp"
#include "boost/archive/json_iarchive.hpp"

using namespace boost::archive;
using boost::serialization::make_nvp;

struct B
{
	int a;
	unsigned int b;

	bool operator== (B const& other) const
	{
		return a == other.a &&
			b == other.b;
	}

private:
	friend class boost::serialization::access;
	template<typename Archive>
	void serialize(Archive& ar, unsigned int const)
	{
		using namespace boost::serialization;
		ar & BOOST_SERIALIZATION_NVP(a)
		   & BOOST_SERIALIZATION_NVP(b);
	}
};

TEST(JSONArchive, Base)
{
	const char name [] = { "myType" };
	B a, b;
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
	for(auto& val : a)
		val = cnt++;

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
