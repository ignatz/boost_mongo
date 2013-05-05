// Copyright 2013, Sebastian Jeltsch (sjeltsch@kip.uni-heidelberg.de)
// Distributed under the terms of the LGPLv3 or newer.

#include <gtest/gtest.h>
#include <string>
#include <cstdlib>

#include "boost/archive/mongo_oarchive.hpp"
#include "boost/archive/mongo_iarchive.hpp"

#include "test/connect.h"
#include "test/detail/Version0.h"
#include "test/detail/Version1.h"
#include "test/detail/Version2.h"
#include "test/detail/Version3.h"

using namespace boost::archive;
using boost::serialization::make_nvp;

std::string const nvp_name = "myMismatchingVersions";

void write_to_db(Version0 const& x,
				 mongo::DBClientConnection& connection,
				 std::string const& collection)
{
	// serialize and store in DB
	mongo::BSONObjBuilder builder;
	mongo_oarchive out(builder);
	out << make_nvp(nvp_name.c_str(), x);
	mongo::BSONObj oo = builder.obj();
	connection.insert(collection, oo);
}

#define READ_FROM_DB(TYPE) \
	void read_from_db(TYPE& y, \
					  mongo::DBClientConnection& connection, \
					  std::string const& collection) \
	{ \
		mongo::BSONObj io; \
		std::auto_ptr<mongo::DBClientCursor> cursor = \
			connection.query(collection, mongo::Query(), 1 /* N results*/); \
		while (cursor->more()) { \
			io = cursor->next(); \
			break; \
		} \
		mongo_iarchive in(io); \
		in >> make_nvp(nvp_name.c_str(), y); \
	}

READ_FROM_DB(Version0)
READ_FROM_DB(Version1)
READ_FROM_DB(Version2)
READ_FROM_DB(Version3)

// serialize base version of class `Version` (located under
// "test/detail/Version0.h") into db.
TEST(Mismatch, ReadWrongTypesFromDB)
{
	TRY_TO_CONNECT_IF_NECESSARY();

	const ::testing::TestInfo* const test_info =
		  ::testing::UnitTest::GetInstance()->current_test_info();
	ASSERT_TRUE(NULL != test_info);
	std::string const collection(db + "." + test_info->test_case_name() + "." + test_info->name());


	Version0 x0, y0;
	Version1 y1;
	Version2 y2;
	Version3 y3;

	x0.a = 42;
	x0.b = 23;
	ASSERT_NE(x0, y0);

	write_to_db(x0, *connection, collection);
	read_from_db(y0, *connection, collection);

	ASSERT_EQ(x0, y0);

	// no read mismatching types (y1, y2) from x0 data set
	ASSERT_NO_THROW(read_from_db(y1, *connection, collection));

	ASSERT_THROW(read_from_db(y2, *connection, collection),
				 boost::archive::mongo_archive_exception);

	ASSERT_THROW(read_from_db(y3, *connection, collection),
				 boost::archive::mongo_archive_exception);
}
