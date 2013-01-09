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
	mongo::BSONObj _o;

public:
    json_iarchive(std::istream& is, unsigned int flags = 0) :
		mongo_iarchive(_o, flags | is_json),
		_o()
	{
		std::istreambuf_iterator<char> eof;
		std::string s(std::istreambuf_iterator<char>(is), eof);
		_o = mongo::fromjson(s);
	}
};

} // archive
} // boost

BOOST_SERIALIZATION_REGISTER_ARCHIVE(boost::archive::json_iarchive)
