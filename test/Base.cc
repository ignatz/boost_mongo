#include "test/Base.h"

// archives must be included before EXPORT macros
#include "boost/archive/mongo_oarchive.hpp"
#include "boost/archive/mongo_iarchive.hpp"

BOOST_CLASS_EXPORT_IMPLEMENT(Base)

Base::Base(int b) : member(b) {}
Base::~Base() {}

bool Base::operator== (Base const& rhs) const
{
	return member == rhs.member;
}

std::ostream& operator<< (std::ostream& os, Base const& b)
{
	return os << "Base( " << "b: " << b.member << ")";
}
