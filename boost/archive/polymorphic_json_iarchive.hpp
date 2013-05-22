#pragma once

// Copyright 2013, Sebastian Jeltsch (sjeltsch@kip.uni-heidelberg.de)
// Distributed under the terms of the LGPLv3 or newer.

#include "boost/archive/detail/polymorphic_iarchive_route2.hpp"
#include "boost/archive/json_iarchive.hpp"

#include <boost/archive/detail/abi_prefix.hpp> // must be the last header

namespace boost {
namespace archive {

class polymorphic_json_iarchive :
	public detail::polymorphic_iarchive_route2<json_iarchive>
{
public:
	polymorphic_json_iarchive(
		std::istream& is,
		unsigned int const flags = 0);

	// NVPs are unpacked in polymorphic_iarchive(nvp<T> const& t, int)
	// and then save is called upon the `t.value()`. Therefore, we need to
	// control the internal json archive stack from here.
	void load_start(char const* name);
	void load_end(char const* name);
};

} // archive
} // boost

#include <boost/archive/detail/abi_suffix.hpp> // pops abi_suffix.hpp pragmas

// required by export
BOOST_SERIALIZATION_REGISTER_ARCHIVE(
	boost::archive::polymorphic_json_iarchive)


namespace boost {
namespace archive {

inline
polymorphic_json_iarchive::polymorphic_json_iarchive(
	std::istream& is,
	unsigned int const flags) :
		detail::polymorphic_iarchive_route2<json_iarchive>(
			is, flags)
{}

inline
void polymorphic_json_iarchive::load_start(char const* name)
{
	json_iarchive::load_start(name);
}

inline
void polymorphic_json_iarchive::load_end(char const* name)
{
	json_iarchive::load_end(name);
}

} // archive
} // boost
