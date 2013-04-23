#pragma once

#include <string>

#include <boost/shared_ptr.hpp>
#include <mongo/client/dbclient.h>

extern std::string const default_port;
extern std::string const default_host;
extern std::string host;
extern std::string const db;

void drop_database(mongo::DBClientConnection& con);

boost::shared_ptr<mongo::DBClientConnection>
connect(std::string const& host,
		std::string const& db = std::string(),
		std::string const& user = std::string(),
		std::string const& pw = std::string());

boost::shared_ptr<mongo::DBClientConnection>
try_to_connect_if_necessary();

// macro to silently skip test ('return') if no DB available
#define TRY_TO_CONNECT_IF_NECESSARY() \
	boost::shared_ptr<mongo::DBClientConnection> connection; \
	do { \
		connection = try_to_connect_if_necessary(); \
		if (!connection) return; \
	} while (false)
