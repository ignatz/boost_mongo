#pragma once

#include <string>
#include <iostream>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/string.hpp>

struct Simple
{
	int a;
	unsigned int b;
	enum Name { x, y, z };
	Name n;
	std::string str;

	Simple() :
		a(0), b(0), n(x), str("me string")
	{}

	bool operator== (Simple const& other) const
	{
		// bitwise compare
		return (strncmp((char*)this, (char*)&other, sizeof(Simple)) == 0);
	}

private:
	friend class boost::serialization::access;
	template<typename Archive>
	void serialize(Archive& ar, unsigned int const)
	{
		using namespace boost::serialization;
		ar & BOOST_SERIALIZATION_NVP(a)
		   & BOOST_SERIALIZATION_NVP(b)
		   & make_nvp("enum", n)
		   & make_nvp("str", str);
	}
};

inline
std::ostream& operator<< (std::ostream& os, Simple const& d)
{
	os << "Simple: { "
		<< "a: "   << d.a << ", "
		<< "b: "   << d.b << ", "
		<< "n: "   << d.n << ", "
		<< "str: " << d.str
		<< " }";
	return os;
}
