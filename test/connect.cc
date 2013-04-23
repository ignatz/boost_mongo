#include "test/connect.h"

#include <iostream>
#include <stdexcept>

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
connect(std::string const& host,
		std::string const& db,
		std::string const& user,
		std::string const& pw)
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
