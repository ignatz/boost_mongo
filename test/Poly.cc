// Copyright 2013, Sebastian Jeltsch (sjeltsch@kip.uni-heidelberg.de)
// Distributed under the terms of the LGPLv3 or newer.

#include "test/Poly.h"

// archives must be included before EXPORT macros
#include "boost/archive/mongo_oarchive.hpp"
#include "boost/archive/mongo_iarchive.hpp"

BOOST_CLASS_EXPORT_IMPLEMENT(Poly)

Poly::Poly(int a, int b) : Base(b), member(a) {}
Poly::~Poly() {}

bool Poly::operator== (Poly const& rhs) const
{
	Base const& base = static_cast<Base const&>(*this);
	return base == static_cast<Base const&>(rhs) &&
		member == rhs.member;
}

std::ostream& operator<< (std::ostream& os, Poly const& d)
{
	Base const& base = static_cast<Base const&>(d);
	os << "Poly( "
		<< base <<  ", "
		<< "member: " << d.member << ", "
		<< " )";
	return os;
}
