#pragma once

#include <boost/serialization/serialization.hpp>
#include <boost/serialization/export.hpp>

struct Base
{
	virtual ~Base();

private:
	friend class boost::serialization::access;
	template<typename Archive>
	void serialize(Archive&, unsigned int const) {}
};

BOOST_SERIALIZATION_ASSUME_ABSTRACT(Base)
BOOST_CLASS_EXPORT_KEY(Base)
