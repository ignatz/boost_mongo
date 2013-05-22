#pragma once

// Copyright 2013, Sebastian Jeltsch (sjeltsch@kip.uni-heidelberg.de)
// Distributed under the terms of the LGPLv3 or newer.

// Most of the source code in this file originates from the header below.
// However, inheriting the interface from the crtp template class was not an
// option, due to constructors being limited to std streams only.
#include <boost/archive/detail/polymorphic_iarchive_route.hpp>
#include <boost/archive/detail/abi_prefix.hpp> // must be the last header

namespace boost {
namespace archive {
namespace detail {

template<typename Archive>
class polymorphic_iarchive_route2 :
	public polymorphic_iarchive,
	public Archive
{
private:
	// these are used by the serialization library.
	virtual void load_object(
		void* t,
		detail::basic_iserializer const& bis);

	typedef detail::basic_pointer_iserializer const* (*finder_type)(
		boost::serialization::extended_type_info const& type);
	virtual detail::basic_pointer_iserializer const*
	load_pointer(
		void*& t,
		detail::basic_pointer_iserializer const* bpis_ptr,
		finder_type finder);

	virtual void set_library_version(
		library_version_type archive_library_version);
	virtual library_version_type get_library_version() const;
	virtual unsigned int get_flags() const;
	virtual void delete_created_pointers();
	virtual void reset_object_address(
		void const* new_address,
		void const* old_address);
	virtual void load_binary(void* t, std::size_t size);

	// primitive types, the only ones permitted by polymorphic archives
	virtual void load(bool& t);
	virtual void load(char& t);
	virtual void load(signed char& t);
	virtual void load(unsigned char& t);
	virtual void load(wchar_t& t);
	virtual void load(short& t);
	virtual void load(unsigned short& t);
	virtual void load(int& t);
	virtual void load(unsigned int& t);
	virtual void load(long& t);
	virtual void load(unsigned long& t);
	virtual void load(long long& t);
	virtual void load(unsigned long long& t);
	virtual void load(float& t);
	virtual void load(double& t);
	virtual void load(std::string& t);
	virtual void load(std::wstring& t);

	virtual void load_start(char const* name);
	virtual void load_end(char const* name);

	virtual void register_basic_serializer(
		detail::basic_iserializer const& bis);

public:
	// this can't be inheriteded because they appear in mulitple
	// parents
	typedef mpl::bool_<true> is_loading;
	typedef mpl::bool_<false> is_saving;

	// the >> operator
	template<class T>
	polymorphic_iarchive& operator>> (T& t);

	// the & operator
	template<class T>
	polymorphic_iarchive& operator& (T& t);

	// register type function
	template<class T>
	detail::basic_pointer_iserializer const*
	register_type(T* t = NULL);

	template<typename T>
	polymorphic_iarchive_route2(
		T& t,
		unsigned int const flags = 0);

	virtual ~polymorphic_iarchive_route2();
};

} // detail
} // archive
} // boost

#include <boost/archive/detail/abi_suffix.hpp> // pops abi_suffix.hpp pragmas

#include "boost/archive/impl/polymorphic_iarchive_route2.ipp"
