// Copyright 2013, Sebastian Jeltsch (sjeltsch@kip.uni-heidelberg.de)
// Distributed under the terms of the LGPLv3 or newer.

#pragma once

#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>

// Base Version
struct Version0
{
	int a, b;

	bool operator== (Version0 const& rhs) const
	{
		return a == rhs.a && b == rhs.b;
	}
	bool operator!= (Version0 const& rhs) const
	{
		return !(*this == rhs);
	}

private:
	friend class boost::serialization::access;
	template<typename Archive>
	void serialize(Archive& ar, unsigned int const)
	{
		ar & BOOST_SERIALIZATION_NVP(a)
		   & BOOST_SERIALIZATION_NVP(b);
	}
};
