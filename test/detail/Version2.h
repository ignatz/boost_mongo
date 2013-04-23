#pragma once

#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>

// Same as Version0 defined in 'test/detail/Version0.h", except this version
// contains more members.
struct Version2
{
	int a, b, c, d, e;

	bool operator== (Version2 const& rhs) const
	{
		return a == rhs.a &&
			b == rhs.b &&
			c == rhs.c &&
			d == rhs.d &&
			e == rhs.e;
	}
	bool operator!= (Version2 const& rhs) const
	{
		return !(*this == rhs);
	}

private:
	friend class boost::serialization::access;
	template<typename Archive>
	void serialize(Archive& ar, unsigned int const)
	{
		ar & BOOST_SERIALIZATION_NVP(a)
		   & BOOST_SERIALIZATION_NVP(b)
		   & BOOST_SERIALIZATION_NVP(c)
		   & BOOST_SERIALIZATION_NVP(d)
		   & BOOST_SERIALIZATION_NVP(e);
	}
};
