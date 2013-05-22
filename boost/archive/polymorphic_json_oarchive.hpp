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
		unsigned int const flags = 0) :
			detail::polymorphic_oarchive_route2<json_oarchive>(
				builder, flags)
	{}

	// NVPs are unpacked in polynomic_oarchive(nvp<T> const& t, int)
	// and then save is called upon the `t.value()`. Therefore, we need to
	// control the internal json archive stack from here
	void save_start(char const* name)
	{
		if (name) {
			_name = name;
			_stack.push_back(std::make_pair(
					value_type(new type), false));
		}
	}

	void save_end(char const* name)
	{
		if (name) {
			if (!top_inserted()) {
				previous_builder().append(name, top_builder().obj());
			}
			_stack.pop_back();
		}
	}
};

} // namespace archive
} // namespace boost

#include <boost/archive/detail/abi_suffix.hpp> // pops abi_suffix.hpp pragmas

// required by export
BOOST_SERIALIZATION_REGISTER_ARCHIVE(
	boost::archive::polymorphic_json_oarchive)
