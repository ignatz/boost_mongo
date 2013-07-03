#pragma once

// Copyright 2013, Sebastian Jeltsch (sjeltsch@kip.uni-heidelberg.de)
// Distributed under the terms of the LGPLv3 or newer.

// push the assert macro on the compiler stack and restore it afer we
// included the mongo header. Older versions of mongo redefined the
// standard macro.
#pragma push_macro("assert")
#include <mongo/client/dbclient.h>
#pragma pop_macro("assert")
#include <cassert>

#if FUSION_MAX_VECTOR_SIZE < 20
#define FUSION_MAX_VECTOR_SIZE 20
#endif
#if FUSION_MAX_MAP_SIZE < 20
#define FUSION_MAX_MAP_SIZE 20
#endif

#include <vector>
#include <cstddef>
#include <string>
#include <sstream>

#include <boost/type_traits.hpp>

#include <boost/shared_ptr.hpp>
#include <boost/lexical_cast.hpp>

#include <boost/mpl/assert.hpp>

#include <boost/utility/enable_if.hpp>

#include <boost/fusion/container/map.hpp>
#include <boost/fusion/include/has_key.hpp>
#include <boost/fusion/include/at_key.hpp>
#include <boost/fusion/include/value_at_key.hpp>

#include <boost/archive/basic_archive.hpp>
#include <boost/archive/detail/register_archive.hpp>
#include <boost/serialization/item_version_type.hpp>
#include <boost/serialization/collection_size_type.hpp>

namespace boost {
namespace archive {
namespace detail {

template<typename T>
class cond_deleter
{
private:
	bool cond;
public:
	cond_deleter(bool del = true) :
		cond(del)
	{}

	void operator() (T* b)
	{
		if (cond) {
			delete b;
		}
	}
};


template<typename T>
struct is_compressible
{
	static const bool value =
		boost::has_trivial_default_constructor<T>::value
		&& (
#ifdef BOOST_TT_HAS_EQUAL_TO_HPP_INCLUDED
			boost::has_equal_to<T>::value
#else
			false
#endif // BOOST_TT_HAS_EQUAL_TO_HPP_INCLUDED
			|| boost::is_arithmetic<T>::value
		   )
		&& !boost::is_array<T>::value;
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
		// class_id_optional_type is strong typedef to class_id_type
		fusion::pair<archive::class_id_optional_type, int_least16_t>,
		// class_id_reference_type is strong typedef to class_id_type
		fusion::pair<archive::class_id_reference_type, int_least16_t>,
		fusion::pair<archive::object_id_type, uint_least32_t>,
		// object_reference_type is strong typedef to object_id_type
		fusion::pair<archive::object_reference_type, uint_least32_t>,
		fusion::pair<archive::version_type, uint_least32_t>,
		fusion::pair<archive::tracking_type, bool>,
		// utilize templated `load` overload (has_key<class_name_type> = true)
		fusion::pair<archive::class_name_type, archive::class_name_type>
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
