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
