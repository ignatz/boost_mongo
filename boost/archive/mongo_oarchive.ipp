#pragma once

// Copyright 2013, Sebastian Jeltsch (sjeltsch@kip.uni-heidelberg.de)
// Distributed under the terms of the LGPLv3 or newer.

#include "boost/archive/mongo_oarchive.hpp"

namespace boost {
namespace archive {

inline
mongo_oarchive::mongo_oarchive(
	type& obj, unsigned int const flags) :
		_stack(1, std::make_pair(
				value_type(&obj, detail::cond_deleter<type>(false)),
				false)),
		_name(NULL), _flags(flags)
	{}

inline
void mongo_oarchive::save_binary(
	void const* address, std::size_t count)
{
	previous_builder().appendBinData(
		_name, count,
		mongo::BinDataGeneral, address);
	top_inserted() = true;
}

inline
bool& mongo_oarchive::top_inserted()
{
	assert(_stack.size()>0);
	return _stack.back().second;
}

inline
mongo_oarchive::type&
mongo_oarchive::top_builder()
{
	assert(_stack.size()>0);
	return *(_stack.back().first);
}

inline
mongo_oarchive::type&
mongo_oarchive::previous_builder()
{
	assert(_stack.size()>1);
	return *((_stack.end()-2)->first);
}

template<typename T>
void mongo_oarchive::append_to_previous_builder(T const& t)
{
	previous_builder().append(_name, t);
	top_inserted() = true;
}

template<typename T>
typename boost::enable_if_c<
		!fusion::result_of::has_key<meta_type_mapping, T>::type::value
	>::type
mongo_oarchive::save_override(T& t, int const x)
{
	// If your program fails to compile here, its most likely due to
	// not specifying an nvp wrapper around the variable to
	// be serialized.
	BOOST_MPL_ASSERT((serialization::is_wrapper<T>));
	detail::common_oarchive<mongo_oarchive>::save_override(t, x);
}

template<typename T>
typename boost::enable_if_c<
		fusion::result_of::has_key<meta_type_mapping, T>::type::value
	>::type
mongo_oarchive::save_override(T const& t, int const x)
{
	using namespace boost::fusion::result_of;
	typename value_at_key<meta_type_mapping, T>::type const& tmp =
		typename value_at_key<meta_type_mapping, T>::type(t);
	save_override(boost::serialization::make_nvp(
			fusion::at_key<T>(meta_type_names), tmp), x);
}

template<typename T>
void mongo_oarchive::save_override(
	boost::serialization::nvp<T> const& t, int const x)
{
	// we have an enum type if name is NULL ... seriously!1!!
	if (t.name() == NULL) {
		save_enum_or_pointer(t.const_value(), x);
		return;
	}

	_name = t.name();
	_stack.push_back(std::make_pair(value_type(new type), false));

	detail::common_oarchive<mongo_oarchive>::save_override(t.const_value(), x);

	// In case the element on top of the stack is a builtin type it has
	// been inserted already in this frame to be represented as "name : value"
	// pair. However, if the top element is a custom type the it still needs
	// to be inserted as "name : { value }" pair.
	if (!top_inserted()) {
		previous_builder().append(t.name(), top_builder().obj());
	}

	_stack.pop_back();
}

inline
void mongo_oarchive::save_override(
	class_name_type const& t, int const)
{
	top_builder().append(fusion::at_key<class_name_type>(meta_type_names), t);
}

template<typename T>
typename boost::enable_if_c<!detail::is_compressible<T>::value>::type
mongo_oarchive::save_override(
	boost::serialization::array<T> const& a, int const x)
{
	using boost::serialization::make_nvp;
	for (size_t ii = 0; ii<a.count(); ++ii)
	{
		std::string s = boost::lexical_cast<std::string>(ii);
		save_override(make_nvp(s.c_str(), *(a.address()+ii)), x);
	}
}

template<typename T>
typename boost::enable_if_c<detail::is_compressible<T>::value>::type
mongo_oarchive::save_override(
	boost::serialization::array<T> const& a, int const x)
{
	typedef typename boost::decay<T>::type base_type;
	using boost::serialization::make_nvp;
	for (size_t ii = 0; ii<a.count(); ++ii)
	{
		if (_flags & sparse_array && *(a.address()+ii) == base_type())
			continue;

		std::string s = boost::lexical_cast<std::string>(ii);
		save_override(make_nvp(s.c_str(), *(a.address()+ii)), x);
	}
}

template<typename T>
void mongo_oarchive::save_enum_or_pointer(T const& t, int const x)
{
	detail::common_oarchive<mongo_oarchive>::save_override(t, x);
}

inline
void mongo_oarchive::save_enum_or_pointer(int const& e, int const)
{
	save(e);
}

template<typename T>
typename boost::enable_if_c<
		!fusion::result_of::has_key<bson_type_mapping, T>::type::value &&
		boost::is_arithmetic<T>::value
	>::type
mongo_oarchive::save(T const& t)
{
	append_to_previous_builder(t);
}

template<typename T>
typename boost::enable_if_c<
		!fusion::result_of::has_key<bson_type_mapping, T>::type::value &&
		!boost::is_arithmetic<T>::value
	>::type
mongo_oarchive::save(T const& t)
{
	std::ostringstream ss;
	ss << t;
	save(ss.str());
}

template<typename T>
typename boost::enable_if_c<
		fusion::result_of::has_key<bson_type_mapping, T>::type::value
	>::type
mongo_oarchive::save(T const& t)
{
	using namespace boost::fusion::result_of;
	append_to_previous_builder(
		typename value_at_key<bson_type_mapping, T>::type(t));
}

inline
void mongo_oarchive::save(boost::serialization::collection_size_type const& t)
{
	save(size_t(t));
}

inline
void mongo_oarchive::save(boost::serialization::item_version_type const& t)
{
	save(unsigned(t));
}

inline
void mongo_oarchive::save(std::string const& s)
{
	append_to_previous_builder(s);
}

inline
void mongo_oarchive::save(std::wstring const& ws)
{
	// convert wstring to multi-byte string first, which can then
	// be stored in mongo db.
	std::string s;
	std::mbstate_t state;
	std::wstring::const_iterator oit;
	for (oit = ws.begin(); oit < ws.end(); ++oit) {
		std::string mb(MB_CUR_MAX, '\0');
		std::wcrtomb(&mb[0], *oit, &state);

		// append only non-0 characters to string
		std::string::const_iterator it = mb.begin();
		while (*it != '\0' && it != mb.end())
			s += *it++;
	}
	save(s);
}


namespace detail {

template<typename T>
void save_array_type<mongo_oarchive>::invoke(
	mongo_oarchive& ar, T const& t)
{
	typedef typename boost::remove_extent<T>::type value_type;

	// consider alignment
	size_t const c = sizeof(t) / sizeof(value_type);

	boost::serialization::collection_size_type count(c);
	ar << BOOST_SERIALIZATION_NVP(count);
	ar << serialization::make_array(static_cast<value_type const*>(&t[0]), count);
}

} // detail
} // archive
} // boost
