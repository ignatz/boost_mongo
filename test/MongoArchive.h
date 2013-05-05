// Copyright 2013, Sebastian Jeltsch (sjeltsch@kip.uni-heidelberg.de)
// Distributed under the terms of the LGPLv3 or newer.

#pragma once

#include <gtest/gtest.h>
#include <string>

#include "boost/archive/mongo_oarchive.hpp"
#include "boost/archive/mongo_iarchive.hpp"

#include <boost/serialization/serialization.hpp>
#include <boost/serialization/nvp.hpp>

class MongoArchive :
	public ::testing::Test
{
protected:
	MongoArchive();
	virtual ~MongoArchive();

	mongo::BSONObj getObject();

	template<typename T>
	void serialize(T const& val);

	template<typename U>
	void deserialize(mongo::BSONObj const& obj, U& b) const;

	static const char* getName()
	{
		return name.c_str();
	}

private:
	static const std::string name;

	mongo::BSONObjBuilder mBuilder;
	boost::archive::mongo_oarchive out;
};


template<typename T>
void MongoArchive::serialize(T const& val)
{
	out << boost::serialization::make_nvp(getName(), val);
}

template<typename U>
void MongoArchive::deserialize(mongo::BSONObj const& obj, U& b) const
{
	boost::archive::mongo_iarchive in(obj);
	in >> boost::serialization::make_nvp(getName(), b);
}
