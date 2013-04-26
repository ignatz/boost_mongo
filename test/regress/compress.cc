#include "test/MongoArchive.h"
#include "test/Plain.h"
#include "test/Simple.h"

#include <boost/serialization/vector.hpp>

struct ExternEqual
{};

bool operator== (ExternEqual const&, ExternEqual const&)
{
	return false;
}

TEST_F(MongoArchive, NotCompressableRegression)
{
	ASSERT_TRUE (boost::has_equal_to<Simple>::value);
	ASSERT_TRUE (boost::has_equal_to<ExternEqual>::value);
	ASSERT_FALSE(boost::has_equal_to<Plain>::value);
	ASSERT_FALSE(boost::archive::detail::is_compressable<Plain>::value);

	std::vector<Plain> a(42);
	std::vector<Plain> b;

	serialize(a);
	deserialize(getObject(), b);

	ASSERT_EQ(a.size(), b.size());
}
