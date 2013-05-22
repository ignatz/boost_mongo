#pragma once

// Copyright 2013, Sebastian Jeltsch (sjeltsch@kip.uni-heidelberg.de)
// Distributed under the terms of the LGPLv3 or newer.

#include "boost/archive/detail/polymorphic_iarchive_route2.hpp"

namespace boost {
namespace archive {
namespace detail {

template<typename Archive>
void polymorphic_iarchive_route2<Archive>::load_object(
	void* t,
	detail::basic_iserializer const& bis)
{
	Archive::load_object(t, bis);
}

template<typename Archive>
detail::basic_pointer_iserializer const*
polymorphic_iarchive_route2<Archive>::load_pointer(
	void*& t,
	detail::basic_pointer_iserializer const* bpis_ptr,
	finder_type finder)
{
	return Archive::load_pointer(t, bpis_ptr, finder);
}

template<typename Archive>
void polymorphic_iarchive_route2<Archive>::set_library_version(
	library_version_type archive_library_version)
{
	Archive::set_library_version(archive_library_version);
}

template<typename Archive>
library_version_type
polymorphic_iarchive_route2<Archive>::get_library_version() const
{
	return Archive::get_library_version();
}

template<typename Archive>
unsigned int polymorphic_iarchive_route2<Archive>::get_flags() const
{
	return Archive::get_flags();
}

template<typename Archive>
void polymorphic_iarchive_route2<Archive>::delete_created_pointers()
{
	Archive::delete_created_pointers();
}

template<typename Archive>
void polymorphic_iarchive_route2<Archive>::reset_object_address(
	void const* new_address,
	void const* old_address)
{
	Archive::reset_object_address(new_address, old_address);
}

template<typename Archive>
void polymorphic_iarchive_route2<Archive>::load_binary(void* t, std::size_t size)
{
	Archive::load_binary(t, size);
}

template<typename Archive>
void polymorphic_iarchive_route2<Archive>::load(bool& t)
{
	Archive::load(t);
}

template<typename Archive>
void polymorphic_iarchive_route2<Archive>::load(char& t)
{
	Archive::load(t);
}

template<typename Archive>
void polymorphic_iarchive_route2<Archive>::load(signed char& t)
{
	Archive::load(t);
}

template<typename Archive>
void polymorphic_iarchive_route2<Archive>::load(unsigned char& t)
{
	Archive::load(t);
}

template<typename Archive>
void polymorphic_iarchive_route2<Archive>::load(wchar_t& t)
{
	Archive::load(t);
}

template<typename Archive>
void polymorphic_iarchive_route2<Archive>::load(short& t)
{
	Archive::load(t);
}

template<typename Archive>
void polymorphic_iarchive_route2<Archive>::load(unsigned short& t)
{
	Archive::load(t);
}

template<typename Archive>
void polymorphic_iarchive_route2<Archive>::load(int& t)
{
	Archive::load(t);
}

template<typename Archive>
void polymorphic_iarchive_route2<Archive>::load(unsigned int& t)
{
	Archive::load(t);
}

template<typename Archive>
void polymorphic_iarchive_route2<Archive>::load(long& t)
{
	Archive::load(t);
}

template<typename Archive>
void polymorphic_iarchive_route2<Archive>::load(unsigned long& t)
{
	Archive::load(t);
}

template<typename Archive>
void polymorphic_iarchive_route2<Archive>::load(long long& t)
{
	Archive::load(t);
}

template<typename Archive>
void polymorphic_iarchive_route2<Archive>::load(unsigned long long& t)
{
	Archive::load(t);
}

template<typename Archive>
void polymorphic_iarchive_route2<Archive>::load(float& t)
{
	Archive::load(t);
}

template<typename Archive>
void polymorphic_iarchive_route2<Archive>::load(double& t)
{
	Archive::load(t);
}

template<typename Archive>
void polymorphic_iarchive_route2<Archive>::load(std::string& t)
{
	Archive::load(t);
}

template<typename Archive>
void polymorphic_iarchive_route2<Archive>::load(std::wstring& t)
{
	Archive::load(t);
}

template<typename Archive>
void polymorphic_iarchive_route2<Archive>::load_start(char const* name)
{
	Archive::load_start(name);
}

template<typename Archive>
void polymorphic_iarchive_route2<Archive>::load_end(char const* name)
{
	Archive::load_end(name);
}

template<typename Archive>
void polymorphic_iarchive_route2<Archive>::register_basic_serializer(
	detail::basic_iserializer const& bis)
{
	Archive::register_basic_serializer(bis);
}

template<typename Archive>
template<class T>
polymorphic_iarchive&
polymorphic_iarchive_route2<Archive>::operator>> (T& t)
{
	return polymorphic_iarchive::operator>>(t);
}

template<typename Archive>
template<class T>
polymorphic_iarchive&
polymorphic_iarchive_route2<Archive>::operator& (T& t)
{
	return polymorphic_iarchive::operator&(t);
}

template<typename Archive>
template<class T>
detail::basic_pointer_iserializer const*
polymorphic_iarchive_route2<Archive>::register_type(T* t)
{
	return Archive::register_type(t);
}

template<typename Archive>
template<typename T>
polymorphic_iarchive_route2<Archive>::polymorphic_iarchive_route2(
	T const& t,
	unsigned int const flags) :
		Archive(t, flags)
{}

template<typename Archive>
polymorphic_iarchive_route2<Archive>::~polymorphic_iarchive_route2()
{}

} // detail
} // archive
} // boost
