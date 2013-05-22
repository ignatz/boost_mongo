#pragma once

// Copyright 2013, Sebastian Jeltsch (sjeltsch@kip.uni-heidelberg.de)
// Distributed under the terms of the LGPLv3 or newer.

#include "boost/archive/detail/polymorphic_iarchive_route2.hpp"
#include "boost/archive/mongo_iarchive.hpp"

#include <boost/archive/detail/abi_prefix.hpp> // must be the last header

namespace boost {
namespace archive {

class polymorphic_mongo_iarchive :
	public detail::polymorphic_iarchive_route2<mongo_iarchive>
{
public:
	polymorphic_mongo_iarchive(
		mongo::BSONObj& obj,
		unsigned int const flags = 0);

	// NVPs are unpacked in polymorphic_iarchive(nvp<T> const& t, int)
	// and then save is called upon the `t.value()`. Therefore, we need to
	// control the internal mongo archive stack from here.
	void load_start(char const* name);
	void load_end(char const* name);
};

} // archive
} // boost

#include <boost/archive/detail/abi_suffix.hpp> // pops abi_suffix.hpp pragmas

// required by export
BOOST_SERIALIZATION_REGISTER_ARCHIVE(
	boost::archive::polymorphic_mongo_iarchive)


namespace boost {
namespace archive {

inline
polymorphic_mongo_iarchive::polymorphic_mongo_iarchive(
	mongo::BSONObj& obj,
	unsigned int const flags) :
		detail::polymorphic_iarchive_route2<mongo_iarchive>(
			obj, flags)
{}

inline
void polymorphic_mongo_iarchive::load_start(char const* name)
{
	mongo_iarchive::load_start(name);
}

inline
void polymorphic_mongo_iarchive::load_end(char const* name)
{
	mongo_iarchive::load_end(name);
}

} // archive
} // boost
