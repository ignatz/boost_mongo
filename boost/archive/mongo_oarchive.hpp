#pragma once

// Copyright 2013, Sebastian Jeltsch (sjeltsch@kip.uni-heidelberg.de)
// Distributed under the terms of the LGPLv3 or newer.

#include <utility>

#include <boost/archive/detail/common_oarchive.hpp>
#include "boost/archive/mongo_common.hpp"

#include <boost/archive/detail/abi_prefix.hpp> // must be the last header

namespace boost {
namespace archive {

class mongo_oarchive :
	public detail::common_oarchive<mongo_oarchive>
{
public:
	enum flags {
		sparse_array = 1,
		flags_last = 1
	};

	mongo_oarchive(mongo::BSONObjBuilder& obj,
				   unsigned int const flags = 0);

	void save_binary(void const* address, std::size_t count);

	struct use_array_optimization
	{
		template<typename T>
		struct apply
		{
			typedef boost::mpl::true_ type;
		};
	};

protected:
	friend class detail::interface_oarchive<mongo_oarchive>;
	friend class save_access;

	typedef mongo::BSONObjBuilder type;
	typedef boost::shared_ptr<type> value_type;

	std::vector<std::pair<value_type, bool /*inserted*/> > _stack;
	char const* _name; // stores current name of nvp for builtins
	unsigned int const _flags;

	bool& top_inserted();
	type& top_builder();
	type& previous_builder();

	template<typename T>
	void append_to_previous_builder(T const& t);

	void save_start(char const* name);
	void save_end(char const* name);

	// Anything not an attribute and not a name-value pair is an
	// error and should be trapped here.
	template<typename T>
	typename boost::enable_if_c<
			!fusion::result_of::has_key<meta_type_mapping, T>::type::value
		>::type
	save_override(T& t, int const);

	// overload for special boost serialization types
	template<typename T>
	typename boost::enable_if_c<
			fusion::result_of::has_key<meta_type_mapping, T>::type::value
		>::type
	save_override(T const& t, int const);

	// main overload for name-value pairs. Internally, serialization
	// is dispatched to the `common_oarchive` base, which in turn dispatches
	// the task to the `save` overloads, defined further below.
	template<typename T>
	void save_override(boost::serialization::nvp<T> const& t, int const);

	// specific overrides for attributes - not name value pairs so we
	// want to trap them before the above "fall through"
	void save_override(class_name_type const& a, int const);

	// overload for non-compressible array types
	template<typename T>
	typename boost::enable_if_c<!detail::is_compressible<T>::value>::type
	save_override(boost::serialization::array<T> const& a, int const);

	// overload for compressible array types
	template<typename T>
	typename boost::enable_if_c<detail::is_compressible<T>::value>::type
	save_override(boost::serialization::array<T> const& a, int const);

	// detail_common_oarchive::save_override calls one of the
	// following `save` overloads.

	// overload for arithmetic types natively supported by mongodb.
	template<typename T>
	typename boost::enable_if_c<
			!fusion::result_of::has_key<bson_type_mapping, T>::type::value &&
			boost::is_arithmetic<T>::value
		>::type
	save(T const& t);

	// this overload fits whenever a class is declared to have
	// `primitive_type`-serialization, therefore use object's
	// stream operators.
	template<typename T>
	typename boost::enable_if_c<
			!fusion::result_of::has_key<bson_type_mapping, T>::type::value &&
			!boost::is_arithmetic<T>::value
		>::type
	save(T const& t);

	// overload for arithmetic types which require type remapping to be
	// stored in mongodb.
	template<typename T>
	typename boost::enable_if_c<
			fusion::result_of::has_key<bson_type_mapping, T>::type::value
		>::type
	save(T const& t);

	// required overloads for boost serialization
	void save(boost::serialization::collection_size_type const& t);
	void save(boost::serialization::item_version_type const& t);

	// required for byte and wide character string serialization
	void save(std::string const& s);
	void save(std::wstring const& ws);
};


namespace detail {

template<>
struct save_array_type<mongo_oarchive>
{
	template<typename T>
	static void invoke(mongo_oarchive& ar, T const& t);
};

} // detail
} // archive
} // boost

// required by export
BOOST_SERIALIZATION_REGISTER_ARCHIVE(boost::archive::mongo_oarchive)
BOOST_SERIALIZATION_USE_ARRAY_OPTIMIZATION(boost::archive::mongo_oarchive)

#include <boost/archive/detail/abi_suffix.hpp> // pops abi_suffix.hpp pragmas

#include "boost/archive/impl/mongo_oarchive.ipp"
