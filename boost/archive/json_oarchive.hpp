#pragma once

// Copyright (c) 2013, Sebastian Jeltsch (sjeltsch@kip.uni-heidelberg.de)
// Distributed under the Boost Software License, Version 1.0.
// (See http://www.boost.org/LICENSE_1_0.txt)

#include <ostream>

#include "boost/archive/mongo_oarchive.hpp"

namespace boost {
namespace archive {

class json_oarchive :
	public mongo_oarchive
{
private:
	mongo::BSONObjBuilder _b;
	std::ostream& _os;

public:
	json_oarchive(std::ostream& os, unsigned int const flags = 0) :
		mongo_oarchive(_b, flags), _b(), _os(os)
	{}

	~json_oarchive()
	{
		_os << _b.obj().jsonString(mongo::Strict, true);
	}
};

} // archive
} // boost

BOOST_SERIALIZATION_REGISTER_ARCHIVE(boost::archive::json_oarchive)
