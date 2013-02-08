#pragma once

// Copyright (c) 2013, Sebastian Jeltsch (sjeltsch@kip.uni-heidelberg.de)
// Distributed under the Boost Software License, Version 1.0.
// (See http://www.boost.org/LICENSE_1_0.txt)

#include <istream>

#include "boost/archive/mongo_iarchive.hpp"

namespace boost {
namespace archive {

class json_iarchive :
    public mongo_iarchive
{
private:
	typedef std::istreambuf_iterator<char> iterator;
	mongo::BSONObj* _obj;

	mongo::BSONObj const& init(std::istream& is)
	{
		_obj = new mongo::BSONObj();
		*_obj = mongo::fromjson(std::string(iterator(is), iterator()));
		return *_obj;
	}

public:
	json_iarchive(std::istream& is, unsigned int const flags = 0) :
		mongo_iarchive(init(is), flags | is_json)
	{}

	~json_iarchive()
	{
	  delete _obj;
	}

};

} // archive
} // boost

BOOST_SERIALIZATION_REGISTER_ARCHIVE(boost::archive::json_iarchive)
