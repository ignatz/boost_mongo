// Copyright 2013, Sebastian Jeltsch (sjeltsch@kip.uni-heidelberg.de)
// Distributed under the terms of the LGPLv3 or newer.

#include "test/MongoArchive.h"

MongoArchive::MongoArchive() :
	mBuilder(),
	out(mBuilder)
{}

MongoArchive::~MongoArchive() {}

mongo::BSONObj MongoArchive::getObject()
{
	return mBuilder.obj();
}

std::string const MongoArchive::name("fancyName");
