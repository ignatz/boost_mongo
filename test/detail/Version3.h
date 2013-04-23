#pragma once

#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>

// Same as Version0 defined in 'test/detail/Version0.h", except this version
// has a different serialization order.
struct Version3
{
	int a, b;

	bool operator== (Version3 const& rhs) const
	{
		return a == rhs.a && b == rhs.b;
	}
	bool operator!= (Version3 const& rhs) const
	{
		return !(*this == rhs);
	}

private:
	friend class boost::serialization::access;
	template<typename Archive>
	void serialize(Archive& ar, unsigned int const)
	{
		ar & BOOST_SERIALIZATION_NVP(b)
		   & BOOST_SERIALIZATION_NVP(a);
	}
};
