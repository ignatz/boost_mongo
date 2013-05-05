#pragma once

// Copyright 2013, Sebastian Jeltsch (sjeltsch@kip.uni-heidelberg.de)
// Distributed under the terms of the LGPLv3 or newer.

#include <list>

#include <boost/archive/detail/common_iarchive.hpp>
#include <boost/archive/shared_ptr_helper.hpp>
#include "boost/archive/mongo_common.hpp"

#include <boost/archive/detail/abi_prefix.hpp> // must be the last header

namespace boost {
namespace archive {

class mongo_iarchive :
	public detail::common_iarchive<mongo_iarchive>,
	public detail::shared_ptr_helper
{
public:
	enum flags {
		sparse_array = 1,
		is_json = 2,
		flags_last = 2
	};

	mongo_iarchive(mongo::BSONObj const& obj,
				   unsigned int const flags = 0);

	void load_binary(void* address, std::size_t count);

	struct use_array_optimization
	{
		template<typename T>
		struct apply
		{
			typedef boost::mpl::true_ type;
		};
	};

protected:
	friend class detail::interface_iarchive<mongo_iarchive>;
	friend class load_access;

	typedef mongo::BSONElement type;
	typedef std::list<type> value_type;

	std::vector<value_type> _stack;
	unsigned int const _flags;

	type& top();
	void pop_element();

	// Anything not an attribute and not a name-value pair is an
	// error and should be trapped here.
	template<typename T>
	typename boost::enable_if_c<
			!fusion::result_of::has_key<meta_type_mapping, T>::type::value
		>::type
	load_override(T& t, int const);

	// overload for special boost serialization types
	template<typename T>
	typename boost::enable_if_c<
			fusion::result_of::has_key<meta_type_mapping, T>::type::value
		>::type
	load_override(T& t, int const);

	// main overload for name-value pairs. Internally, serialization
	// is dispatched to the `common_iarchive` base, which in turn dispatches
	// the task to the `load` overloads, defined further below.
	template<typename T>
	void load_override(boost::serialization::nvp<T> const& t, int const);

	// overload for non-compressible array types
	template<typename T>
	typename boost::enable_if_c<!detail::is_compressible<T>::value>::type
	load_override(boost::serialization::array<T> const& a, int const);

	// overload for compressible array types
	template<typename T>
	typename boost::enable_if_c<detail::is_compressible<T>::value>::type
	load_override(boost::serialization::array<T> const& a, int const);

	// class_id_optional must be ignored (like basic_xml_iarchive)
	void load_override(class_id_optional_type&, int const);

	// overload for pointer types
	template<typename T>
	void load_enum_or_pointer(T& t, int const);

	// overload for enum types
	void load_enum_or_pointer(int& t, int const);

	// helper function
	template<typename U, typename T>
	void load_field_and_cast(const char* /*field_name*/, T& t);


	// detail_common_iarchive::load_override calls one of the
	// following `load` overloads.

	// overload for arithmetic types natively supported by mongodb.
	template<typename T>
	typename boost::enable_if_c<
			!fusion::result_of::has_key<bson_type_mapping, T>::type::value &&
			boost::is_arithmetic<T>::value
		>::type
	load(T& t);

	// this overload fits whenever a class is declared to have
	// `primitive_type`-serialization, therefore use object's
	// stream operators.
	template<typename T>
	typename boost::enable_if_c<
			!fusion::result_of::has_key<bson_type_mapping, T>::type::value &&
			!boost::is_arithmetic<T>::value
		>::type
	load(T& t);

	// overload for arithmetic types which require type remapping to be
	// stored in mongodb.
	template<typename T>
	typename boost::enable_if_c<
			fusion::result_of::has_key<bson_type_mapping, T>::type::value
		>::type
	load(T& t);

	// required overloads for boost serialization
	void load(class_name_type& t);
	void load(boost::serialization::collection_size_type& t);
	void load(boost::serialization::item_version_type& t);

	// required for byte and wide character string serialization
	void load(std::string& s);
	void load(std::wstring& ws);

	// required for JSON deserialization
	void load(bool& b);
	void load(long& l);
	void load(long long& ll);
};


namespace detail {

template<>
struct load_array_type<mongo_iarchive>
{
	template<typename T>
	static void invoke(mongo_iarchive& ar, T& t);
};

} // detail
} // archive
} // boost

// required by export
BOOST_SERIALIZATION_REGISTER_ARCHIVE(boost::archive::mongo_iarchive)
BOOST_SERIALIZATION_USE_ARRAY_OPTIMIZATION(boost::archive::mongo_iarchive)

#include <boost/archive/detail/abi_suffix.hpp> // pops abi_suffix.hpp pragmas

#include "boost/archive/mongo_iarchive.ipp"
