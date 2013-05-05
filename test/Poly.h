// Copyright 2013, Sebastian Jeltsch (sjeltsch@kip.uni-heidelberg.de)
// Distributed under the terms of the LGPLv3 or newer.

#pragma once

#include <ostream>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/export.hpp>
#include <boost/serialization/nvp.hpp>

#include "test/Base.h"

struct Poly : public Base
{
	Poly(int a = 0, int b = 0);
	virtual ~Poly();

	// wickedly the name of this member and the int member in
	// Poly's base class are the same.
	int member;

	bool operator== (Poly const& rhs) const;
	virtual void fun() {}

private:
	friend class boost::serialization::access;
	template<typename Archive>
	void serialize(Archive& ar, unsigned int const)
	{
		ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(Base)
		   & BOOST_SERIALIZATION_NVP(member);
	}
};

BOOST_SERIALIZATION_ASSUME_ABSTRACT(Poly)
BOOST_CLASS_EXPORT_KEY(Poly)

std::ostream& operator<< (std::ostream& os, Poly const& d);
