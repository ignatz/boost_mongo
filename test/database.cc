#include <gtest/gtest.h>
#include <string>
#include <cstdlib>
#include <stdexcept>
#include <iostream>

#include <boost/shared_ptr.hpp>
#include <boost/serialization/map.hpp>
#include "boost/archive/mongo_oarchive.hpp"
#include "boost/archive/mongo_iarchive.hpp"

#include "test/Simple.h"

using namespace boost::archive;
using boost::serialization::make_nvp;

std::string const default_port("27017");
std::string const default_host("localhost");
std::string host(default_host + ":" + default_port);
std::string const db("BoostMongoTest");

void drop_database(mongo::DBClientConnection& con)
{
	mongo::BSONObj info;
	con.runCommand(db, BSON("dropDatabase" << 1), info);
}

boost::shared_ptr<mongo::DBClientConnection>
connect(
	std::string const& host,
	std::string const& db = std::string(),
	std::string const& user = std::string(),
	std::string const& pw = std::string())
{
	boost::shared_ptr<mongo::DBClientConnection> con(
		new mongo::DBClientConnection);

	try {
		con->connect(host);

		std::string emsg;
		if (!user.empty() && !con->auth(db, user, pw, emsg)) {
			throw std::runtime_error(emsg.c_str());
		}
	} catch (...) {
		con.reset();
		throw;
	}
	return con;
}

boost::shared_ptr<mongo::DBClientConnection>
try_to_connect_if_necessary()
{
	static boost::shared_ptr<mongo::DBClientConnection> con;

	if (!con) {
		try {
			con = connect(host, db);
			drop_database(*con);
		} catch(mongo::DBException const& e) {
			// silent failure policy: run tests, including the real
			// thing, only if DB is available.
			std::cerr << "connection failed: "
				<< e.what() << std::endl;
			std::cerr << "make sure a mongodb instance is running, "
				"to perform tests using db access" << std::endl;
		}
	}
	return con;
}

#define TRY_TO_CONNECT_IF_NECESSARY() \
	boost::shared_ptr<mongo::DBClientConnection> connection; \
	do { \
		connection = try_to_connect_if_necessary(); \
		if (!connection) return; \
	} while (false)


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
