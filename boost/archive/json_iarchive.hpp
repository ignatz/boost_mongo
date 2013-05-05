#pragma once

// Copyright 2013, Sebastian Jeltsch (sjeltsch@kip.uni-heidelberg.de)
// Distributed under the terms of the LGPLv3 or newer.

#include <istream>

#include "boost/archive/mongo_iarchive.hpp"

namespace boost {
namespace archive {

class json_iarchive :
	public mongo_iarchive
{
public:
	json_iarchive(
		std::istream& is,
		unsigned int const flags = 0);

	~json_iarchive();

private:
	mongo::BSONObj const& init(std::istream& is);

	mongo::BSONObj* _obj;
};

} // archive
} // boost

BOOST_SERIALIZATION_REGISTER_ARCHIVE(boost::archive::json_iarchive)


namespace boost {
namespace archive {

inline
mongo::BSONObj const&
json_iarchive::init(std::istream& is)
{
	typedef std::istreambuf_iterator<char> it;

	_obj = new mongo::BSONObj();
	*_obj = mongo::fromjson(std::string(it(is), it()));

	return *_obj;
}

inline
json_iarchive::json_iarchive(
	std::istream& is,
	unsigned int const flags) :
		// make sure, is_json flag is set
		mongo_iarchive(init(is), flags | is_json)
{}

inline
json_iarchive::~json_iarchive()
{
	delete _obj;
}

} // archive
} // boost
