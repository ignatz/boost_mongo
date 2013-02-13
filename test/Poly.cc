#include "test/Poly.h"

// archives must be included before EXPORT macros
#include "boost/archive/mongo_oarchive.hpp"
#include "boost/archive/mongo_iarchive.hpp"

BOOST_CLASS_EXPORT_IMPLEMENT(Poly)

Poly::Poly(int a) : Base(), member(a) {}

bool Poly::operator== (Poly const& other) const
{
	return member == other.member;
}

std::ostream& operator<< (std::ostream& os, Poly const& d)
{
	os << "Poly : { "
		<< "member: " << d.member << ", "
		<< " }";
	return os;
}
