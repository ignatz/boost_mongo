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
private:
	std::vector<bool> _is_obj;

public:
	polymorphic_json_iarchive(
		json::BSONObj& obj,
		unsigned int const flags = 0) :
			detail::polymorphic_iarchive_route2<json_iarchive>(
				obj, flags),
			_is_obj()
	{}

	// NVPs are unpacked in polynomic_oarchive(nvp<T> const& t, int)
	// and then save is called upon the `t.value()`. Therefore, we need to
	// control the internal json archive stack from here.
	void load_start(char const* name)
	{
		if (name) {
			_is_obj.push_back(top().isABSONObj());
			if (_is_obj.back()) {
				value_type tmp;
				top().Obj().elems(tmp);
				_stack.push_back(value_type());
				_stack.back().swap(tmp);
			}
		}
	}

	void load_end(char const* name)
	{
		if (name) {
			if(_is_obj.back()) {
				_stack.pop_back();
			}
			pop_element();
			_is_obj.pop_back();
		}
	}
};

} // archive
} // boost

#include <boost/archive/detail/abi_suffix.hpp> // pops abi_suffix.hpp pragmas

// required by export
BOOST_SERIALIZATION_REGISTER_ARCHIVE(
	boost::archive::polymorphic_json_iarchive)
