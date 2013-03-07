#pragma once

// Copyright (c) 2013, Sebastian Jeltsch (sjeltsch@kip.uni-heidelberg.de)
// Distributed under the Boost Software License, Version 1.0.
// (See http://www.boost.org/LICENSE_1_0.txt)

#include <boost/type_traits.hpp>

// we push the assert macro on the compiler stack and restore it afer we
// included the mongo header, because older version redefine the standard
// macro.
#pragma push_macro("assert")
#include <mongo/client/dbclient.h>
#pragma pop_macro("assert")
#include <cassert>

#define FUSION_MAX_VECTOR_SIZE 20
#define FUSION_MAX_MAP_SIZE 20

#include <boost/fusion/container/map.hpp>
#include <boost/fusion/include/has_key.hpp>
#include <boost/fusion/include/at_key.hpp>
#include <boost/fusion/include/value_at_key.hpp>
#include <boost/utility/enable_if.hpp>

#include <boost/archive/basic_archive.hpp>
#include <boost/serialization/item_version_type.hpp>
#include <boost/serialization/collection_size_type.hpp>
#include <boost/type_traits/has_equal_to.hpp>

namespace boost {
namespace archive {
namespace detail {

template<typename T>
class cond_deleter
{
private:
	bool cond;
public:
	cond_deleter(bool del = true) : cond(del) {}

	void operator() (T* b)
	{
		if (cond) {
			delete b;
		}
	}
};


template<typename T>
struct is_compressable
{
	static const bool value =
		boost::has_trivial_default_constructor<T>::value &&
		(boost::has_equal_to<T>::value || boost::is_arithmetic<T>::value);
};

} // detail


// BSONObj supports the following native C++ types:
// long long, bool, int, double, std::string
// This means all other types need to be mapped accordingly
typedef fusion::map<
		fusion::pair<char, int>,
		fusion::pair<signed char, int>,
		fusion::pair<unsigned char, int>,
		fusion::pair<wchar_t, int>,
		fusion::pair<short, int>,
		fusion::pair<unsigned short, int>,
		fusion::pair<unsigned int, long long>,
		fusion::pair<long, long long>,
		fusion::pair<unsigned long, long long>,
		fusion::pair<unsigned long long, long long>, // problematic
		fusion::pair<float, double>,
		fusion::pair<long double, double> // problematic
	> bson_type_mapping;


typedef fusion::map<
		fusion::pair<archive::class_id_type, int_least16_t>,
		fusion::pair<archive::class_id_optional_type, int_least16_t>, // strong typedef to class_id_type
		fusion::pair<archive::class_id_reference_type, int_least16_t>, // strong typedef to class_id_type
		fusion::pair<archive::object_id_type, uint_least32_t>,
		fusion::pair<archive::object_reference_type, uint_least32_t>, // strong typedef to object_id_type
		fusion::pair<archive::version_type, uint_least32_t>,
		fusion::pair<archive::tracking_type, bool>,
		fusion::pair<archive::class_name_type, archive::class_name_type> // uses overloaded load function
	> meta_type_mapping;

typedef fusion::map<
		fusion::pair<archive::class_id_type, char const*>,
		fusion::pair<archive::class_id_optional_type, char const*>,
		fusion::pair<archive::class_id_reference_type, char const*>,
		fusion::pair<archive::object_id_type, char const*>,
		fusion::pair<archive::object_reference_type, char const*>,
		fusion::pair<archive::version_type, char const*>,
		fusion::pair<archive::tracking_type, char const*>,
		fusion::pair<archive::class_name_type, char const*>
	> meta_type_names_t;

meta_type_names_t const meta_type_names(
	fusion::make_pair<archive::class_id_type>("_class_id"),
	fusion::make_pair<archive::class_id_optional_type>("_class_id_optional"),
	fusion::make_pair<archive::class_id_reference_type>("_class_id_reference"),
	fusion::make_pair<archive::object_id_type>("_object_id"),
	fusion::make_pair<archive::object_reference_type>("_object_reference"),
	fusion::make_pair<archive::version_type>("_version"),
	fusion::make_pair<archive::tracking_type>("_tracking"),
	fusion::make_pair<archive::class_name_type>("_class_name")
);

} // archive
} // boost
