// Copyright 2013, Sebastian Jeltsch (sjeltsch@kip.uni-heidelberg.de)
// Distributed under the terms of the LGPLv3 or newer.

#pragma once

#include <ostream>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/export.hpp>
#include <boost/serialization/nvp.hpp>

struct Base
{
	Base(int b);
	virtual ~Base();

	int member;

	virtual void fun() = 0;

	bool operator== (Base const& rhs) const;

private:
	friend class boost::serialization::access;
	template<typename Archive>
	void serialize(Archive& ar, unsigned int const)
	{
		ar & BOOST_SERIALIZATION_NVP(member);
	}
};

std::ostream& operator<< (std::ostream& os, Base const& b);

BOOST_SERIALIZATION_ASSUME_ABSTRACT(Base)
BOOST_CLASS_EXPORT_KEY(Base)
