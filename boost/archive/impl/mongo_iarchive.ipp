#pragma once

// Copyright 2013, Sebastian Jeltsch (sjeltsch@kip.uni-heidelberg.de)
// Distributed under the terms of the LGPLv3 or newer.

#include <boost/serialization/throw_exception.hpp>
#include <boost/archive/iterators/dataflow_exception.hpp>
#include "boost/archive/mongo_archive_exception.hpp"
#include "boost/archive/mongo_iarchive.hpp"

namespace boost {
namespace archive {

inline
mongo_iarchive::mongo_iarchive(
	mongo::BSONObj const& obj,
	unsigned int const flags) :
		_stack(1), _flags(flags)
{
	obj.elems(_stack.back());

	// remove _id field added by mongodb
	if (obj.hasField("_id")) {
		assert(strcmp("_id", top().fieldName()) == 0);
		pop_element();
	}
}

inline
void mongo_iarchive::load_binary(void* address, std::size_t count)
{
	int len;
	const char* data = top().binData(len);
	if (size_t(len) != count) {
		using boost::serialization::throw_exception;
		throw_exception(mongo_archive_exception(
			mongo_archive_exception::mongo_archive_binary));
	}
	std::copy(data, data+len, static_cast<char*>(address));
}

inline
mongo_iarchive::type& mongo_iarchive::top()
{
	if (_stack.size() == 0 || _stack.back().size() == 0) {
		// when you see this error it's most certainly, because your data
		// structure has changed since serialization or your simply reading
		// the wrong data.
		using boost::serialization::throw_exception;
		throw_exception(mongo_archive_exception(
			mongo_archive_exception::mongo_archive_mismatch));
	}
	return _stack.back().front();
}

inline
void mongo_iarchive::pop_element()
{
	if (_stack.size() == 0 || _stack.back().size() == 0) {
		// same as above.
		using boost::serialization::throw_exception;
		throw_exception(mongo_archive_exception(
			mongo_archive_exception::mongo_archive_mismatch));
	}
	_stack.back().pop_front();
}

inline
void mongo_iarchive::load_start(char const* name)
{
	if (name) {
		_is_obj.push_back(top().isABSONObj());
		if (_is_obj.back()) {
			value_type tmp;
			top().Obj().elems(tmp);
			_stack.push_back(value_type());
			_stack.back().swap(tmp);
		}
	}
}

inline
void mongo_iarchive::load_end(char const* name)
{
	if (name) {
		if(_is_obj.back()) {
			_stack.pop_back();
		}
		_is_obj.pop_back();
		pop_element();
	}
}

template<typename T>
typename boost::enable_if_c<
		!fusion::result_of::has_key<meta_type_mapping, T>::type::value
	>::type
mongo_iarchive::load_override(T& t, int const x)
{
	// If your program fails to compile here, its most likely due to
	// not specifying an nvp wrapper around the variable to
	// be serialized.
	BOOST_MPL_ASSERT((serialization::is_wrapper<T>));
	this->detail::common_iarchive<mongo_iarchive>::load_override(t, x);
}

template<typename T>
typename boost::enable_if_c<
		fusion::result_of::has_key<meta_type_mapping, T>::type::value
	>::type
mongo_iarchive::load_override(T& t, int const)
{
	using namespace boost::fusion::result_of;
	load_field_and_cast<typename value_at_key<meta_type_mapping, T>::type>(
		fusion::at_key<T>(meta_type_names), t);
}

template<typename T>
void mongo_iarchive::load_override(
	boost::serialization::nvp<T> const& t, int const x)
{
	if (t.name() && strcmp(t.name(), top().fieldName()) != 0) {
		// The name of the NVP doesn't match the current BSONElement. This
		// could mean that you changed the data structure or serialization
		// code (particularly NVPs) since serialization, or you're simply
		// reading the wrong data.
		using boost::serialization::throw_exception;
		throw_exception(mongo_archive_exception(
			mongo_archive_exception::mongo_archive_name_error));
	}

	// if `t` is neither enum nor pointer type but a custom type (meaning the
	// current BSONElement is actually a BSONObj), dive one level deeper into
	// the BSON hierarchy.
	load_start(t.name());

	// pass the task on to the lower levels, which will later on bubble up again
	// to one of the local load overloads.
	detail::common_iarchive<mongo_iarchive>::load_override(t.value(), x);

	// if `t` has been a custom type, we come back up one hierarchical level and
	// now need to clean up the already read BSONObj from the stack, but in any
	// case we need to pop the current BSONElement.
	load_end(t.name());
}

template<typename T>
typename boost::enable_if_c<!detail::is_compressible<T>::value>::type
mongo_iarchive::load_override(
	boost::serialization::array<T> const& a, int const x)
{
	using boost::serialization::make_nvp;
	for (size_t ii = 0; ii<a.count(); ++ii)
	{
		std::string s = boost::lexical_cast<std::string>(ii);
		load_override(make_nvp(s.c_str(), *(a.address()+ii)), x);
	}
}

template<typename T>
typename boost::enable_if_c<detail::is_compressible<T>::value>::type
mongo_iarchive::load_override(
	boost::serialization::array<T> const& a, int const x)
{
	using boost::serialization::make_nvp;
	using boost::lexical_cast;

	// normally serialized array
	if (!(_flags & sparse_array)) {
		for (size_t ii = 0; ii<a.count(); ++ii)
		{
			if (lexical_cast<std::string>(ii) != top().fieldName()) {
				using boost::serialization::throw_exception;
				throw_exception(mongo_archive_exception(
					mongo_archive_exception::mongo_archive_sparse_array));
			}
			load_override(make_nvp(top().fieldName(), *(a.address()+ii)), x);
		}
	// sparsely serialized array
	} else {
		for (size_t ii = 0; ii<a.count(); ++ii)
		{
			if (_stack.back().size() > 0 &&
					lexical_cast<std::string>(ii) == top().fieldName()) {
				load_override(make_nvp(top().fieldName(), *(a.address()+ii)), x);
			} else {
				*(a.address()+ii) = T();
			}
		}

	}
}

inline
void mongo_iarchive::load_override(class_id_optional_type&, int const)
{
	if (strcmp(top().fieldName(), fusion::at_key<class_id_optional_type>(
			meta_type_names)) == 0) {
		pop_element();
	}
}

template<typename U, typename T>
void mongo_iarchive::load_field_and_cast(const char* /*field_name*/, T& t)
{
	load(static_cast<U&>(t));
	pop_element();
}

template<typename T>
typename boost::enable_if_c<
		!fusion::result_of::has_key<bson_type_mapping, T>::type::value &&
		boost::is_arithmetic<T>::value
	>::type
mongo_iarchive::load(T& t)
{
	if (_flags & is_json) {
		t = top().number();
	} else {
		top().Val(t);
	}
}

template<typename T>
typename boost::enable_if_c<
		!fusion::result_of::has_key<bson_type_mapping, T>::type::value &&
		!boost::is_arithmetic<T>::value
	>::type
mongo_iarchive::load(T& t)
{
	std::istringstream ss(top().String());
	ss >> t;
}

template<typename T>
typename boost::enable_if_c<
		fusion::result_of::has_key<bson_type_mapping, T>::type::value
	>::type
mongo_iarchive::load(T& t)
{
	if (_flags & is_json) {
		t = top().number();
	} else {
		using namespace boost::fusion::result_of;
		typename value_at_key<bson_type_mapping, T>::type tmp;
		top().Val(tmp);
		t = tmp;
	}
}

inline
void mongo_iarchive::load(class_name_type& t)
{
	std::string const& str = top().String();
	if (str.size() > BOOST_SERIALIZATION_MAX_KEY_SIZE - 1) {
		using boost::serialization::throw_exception;
		throw_exception(archive_exception(
				archive_exception::invalid_class_name));
	}
	char* tptr = t;
	str.copy(t, std::string::npos);
	tptr[str.size()] = '\0';
}

inline
void mongo_iarchive::load(boost::serialization::collection_size_type& t)
{
	load(static_cast<size_t&>(t));
}

inline
void mongo_iarchive::load(boost::serialization::item_version_type& t)
{
	load(static_cast<unsigned int&>(t));
}

inline
void mongo_iarchive::load(std::string& s)
{
	s = top().String();
}

inline
void mongo_iarchive::load(std::wstring& ws)
{
	ws.clear();
	std::string s(top().String());
	char const* start = s.data();
	char const* const end = start + s.size();

	int ret;
	std::mbstate_t state = std::mbstate_t();
	wchar_t wc;
	while ((ret = std::mbrtowc(&wc, start, end-start, &state)) > 0) {
		ws += wc;
		start+=ret;
	}
	if (ret<0) {
		boost::serialization::throw_exception(
			iterators::dataflow_exception(
				iterators::dataflow_exception::invalid_conversion));
	}
}

inline
void mongo_iarchive::load(bool& b)
{
	b = top().booleanSafe();
}
inline
void mongo_iarchive::load(long& l)
{
	l = top().numberLong();
}
inline
void mongo_iarchive::load(long long& ll)
{
	ll = top().numberLong();
}


namespace detail {

template<typename T>
void load_array_type<mongo_iarchive>::invoke(
	mongo_iarchive& ar, T& t)
{
	typedef typename boost::remove_extent<T>::type value_type;

	// consider alignment
	size_t const c = sizeof(t) / sizeof(value_type);

	boost::serialization::collection_size_type count;
	ar >> BOOST_SERIALIZATION_NVP(count);
	if (size_t(count) > c) {
		using boost::serialization::throw_exception;
		throw_exception(archive_exception(
			archive_exception::array_size_too_short));
	}
	ar >> serialization::make_array(static_cast<value_type*>(&t[0]), count);
}

} // detail
} // archive
} // boost
