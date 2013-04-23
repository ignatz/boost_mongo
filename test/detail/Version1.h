#pragma once

#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>

// Same as Version defined in 'test/detail/Version0.h", except this version
// contains one member less.
struct Version1
{
	int a;

	bool operator== (Version1 const& rhs) const
	{
		return a == rhs.a;
	}
	bool operator!= (Version1 const& rhs) const
	{
		return !(*this == rhs);
	}

private:
	friend class boost::serialization::access;
	template<typename Archive>
	void serialize(Archive& ar, unsigned int const)
	{
		ar & BOOST_SERIALIZATION_NVP(a);
	}
};
