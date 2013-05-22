#pragma once

// Copyright 2013, Sebastian Jeltsch (sjeltsch@kip.uni-heidelberg.de)
// Distributed under the terms of the LGPLv3 or newer.

#include "boost/archive/detail/polymorphic_oarchive_route2.hpp"
#include "boost/archive/json_oarchive.hpp"

#include <boost/archive/detail/abi_prefix.hpp> // must be the last header

namespace boost {
namespace archive {

class polymorphic_json_oarchive :
	public detail::polymorphic_oarchive_route2<json_oarchive>
{
public:
	polymorphic_json_oarchive(
		json::BSONObjBuilder& builder,
		unsigned int const flags = 0);

	// NVPs are unpacked in polymorphic_oarchive(nvp<T> const& t, int)
	// and then save is called upon the `t.value()`. Therefore, we need to
	// control the internal json archive stack from here
	void save_start(char const* name);
	void save_end(char const* name);
};

} // namespace archive
} // namespace boost

#include <boost/archive/detail/abi_suffix.hpp> // pops abi_suffix.hpp pragmas

// required by export
BOOST_SERIALIZATION_REGISTER_ARCHIVE(
	boost::archive::polymorphic_json_oarchive)


namespace boost {
namespace archive {

inline
polymorphic_json_oarchive::polymorphic_json_oarchive(
	json::BSONObjBuilder& builder,
	unsigned int const flag) :
		detail::polymorphic_oarchive_route2<json_oarchive>(
			builder, flags)
{}

inline
void polymorphic_json_oarchive::save_start(char const* name)
{
	json_oarchive::save_start(name);
}

inline
void polymorphic_json_oarchive::save_end(char const* name)
{
	json_oarchive::save_end(name);
}

} // namespace archive
} // namespace boost
