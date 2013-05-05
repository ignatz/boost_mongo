// Copyright 2013, Sebastian Jeltsch (sjeltsch@kip.uni-heidelberg.de)
// Distributed under the terms of the LGPLv3 or newer.

#include "test/MongoArchive.h"

#include <boost/serialization/array.hpp>


TEST_F(MongoArchive, AllMembersRegression)
{
	boost::array<int, 42> a, b;
	serialize(a);

	mongo::BSONObj obj = getObject();
	mongo::BSONElement elem = obj.getField(getName()).embeddedObject().getField("elems");
	ASSERT_EQ(int(a.size()+1), elem.embeddedObject().nFields());

	deserialize(obj, b);
	ASSERT_TRUE(std::equal(a.begin(), a.end(), b.begin()));
}
