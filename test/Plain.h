#pragma once
#include <boost/serialization/serialization.hpp>

class Plain
{
	friend class boost::serialization::access;
	template<typename Archive>
	void serialize(Archive&, unsigned int const) {}
};
