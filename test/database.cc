#include <gtest/gtest.h>
#include <string>
#include <cstdlib>

#include <boost/serialization/map.hpp>
#include "boost/archive/mongo_oarchive.hpp"
#include "boost/archive/mongo_iarchive.hpp"

#include "test/connect.h"
#include "test/Simple.h"

using namespace boost::archive;
using boost::serialization::make_nvp;

TEST(Database, Base)
{
	TRY_TO_CONNECT_IF_NECESSARY();

	const ::testing::TestInfo* const test_info =
		  ::testing::UnitTest::GetInstance()->current_test_info();
	ASSERT_TRUE(NULL != test_info);

	std::string const collection(db + "." + test_info->test_case_name() + "." + test_info->name());
	const char name [] = { "myMap" };

	// build reference types
	typedef std::map<std::string, Simple> map_t;
	map_t x, y;

	x["one"].a = 42;
	x["one"].b = 23;
	x["two"].a = 5;
	x["three"].b = 1337;
	ASSERT_EQ(3u, x.size());
	ASSERT_NE(x, y);

	// serialize and store in DB
	mongo::BSONObjBuilder builder;
	mongo_oarchive out(builder);
	out << make_nvp(name, x);
	mongo::BSONObj oo = builder.obj();
	connection->insert(collection, oo);

	// deserialize from DB
	mongo::BSONObj io;
	std::auto_ptr<mongo::DBClientCursor> cursor =
		connection->query(collection, mongo::Query(), 1 /* N results*/);
	while (cursor->more()) {
		io = cursor->next();
		break;
	}
	mongo_iarchive in(io);
	in >> make_nvp(name, y);

	ASSERT_EQ(3u, y.size());
	ASSERT_EQ(x, y);
}
