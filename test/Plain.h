// Copyright 2013, Sebastian Jeltsch (sjeltsch@kip.uni-heidelberg.de)
// Distributed under the terms of the LGPLv3 or newer.

#pragma once
#include <boost/serialization/serialization.hpp>

class Plain
{
	friend class boost::serialization::access;
	template<typename Archive>
	void serialize(Archive&, unsigned int const) {}
};
