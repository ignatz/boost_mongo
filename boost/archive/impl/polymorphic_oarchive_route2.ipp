#pragma once

// Copyright 2013, Sebastian Jeltsch (sjeltsch@kip.uni-heidelberg.de)
// Distributed under the terms of the LGPLv3 or newer.

#include "boost/archive/detail/polymorphic_oarchive_route2.hpp"

namespace boost {
namespace archive {
namespace detail {

template<typename Archive>
void polymorphic_oarchive_route2<Archive>::save_object(
	void const* x,
	detail::basic_oserializer const& bos)
{
	Archive::save_object(x, bos);
}

template<typename Archive>
void polymorphic_oarchive_route2<Archive>::save_pointer(
	void const* t,
	detail::basic_pointer_oserializer const* bpos_ptr)
{
	Archive::save_pointer(t, bpos_ptr);
}

template<typename Archive>
void polymorphic_oarchive_route2<Archive>::save_null_pointer()
{
	Archive::save_null_pointer();
}

template<typename Archive>
void polymorphic_oarchive_route2<Archive>::save(bool const t)
{
	Archive::save(t);
}

template<typename Archive>
void polymorphic_oarchive_route2<Archive>::save(char const t)
{
	Archive::save(t);
}

template<typename Archive>
void polymorphic_oarchive_route2<Archive>::save(signed char const t)
{
	Archive::save(t);
}

template<typename Archive>
void polymorphic_oarchive_route2<Archive>::save(unsigned char const t)
{
	Archive::save(t);
}

template<typename Archive>
void polymorphic_oarchive_route2<Archive>::save(wchar_t const t)
{
	Archive::save(t);
}

template<typename Archive>
void polymorphic_oarchive_route2<Archive>::save(short const t)
{
	Archive::save(t);
}

template<typename Archive>
void polymorphic_oarchive_route2<Archive>::save(unsigned short const t)
{
	Archive::save(t);
}

template<typename Archive>
void polymorphic_oarchive_route2<Archive>::save(int const t)
{
	Archive::save(t);
}

template<typename Archive>
void polymorphic_oarchive_route2<Archive>::save(unsigned int const t)
{
	Archive::save(t);
}

template<typename Archive>
void polymorphic_oarchive_route2<Archive>::save(long const t)
{
	Archive::save(t);
}

template<typename Archive>
void polymorphic_oarchive_route2<Archive>::save(unsigned long const t)
{
	Archive::save(t);
}

template<typename Archive>
void polymorphic_oarchive_route2<Archive>::save(long long const t)
{
	Archive::save(t);
}

template<typename Archive>
void polymorphic_oarchive_route2<Archive>::save(unsigned long long const t)
{
	Archive::save(t);
}

template<typename Archive>
void polymorphic_oarchive_route2<Archive>::save(float const t)
{
	Archive::save(t);
}

template<typename Archive>
void polymorphic_oarchive_route2<Archive>::save(double const t)
{
	Archive::save(t);
}

template<typename Archive>
void polymorphic_oarchive_route2<Archive>::save(std::string const& t)
{
	Archive::save(t);
}

template<typename Archive>
void polymorphic_oarchive_route2<Archive>::save(std::wstring const& t)
{
	Archive::save(t);
}

template<typename Archive>
library_version_type
polymorphic_oarchive_route2<Archive>::get_library_version() const
{
	return Archive::get_library_version();
}

template<typename Archive>
unsigned int polymorphic_oarchive_route2<Archive>::get_flags() const
{
	return Archive::get_flags();
}

template<typename Archive>
void polymorphic_oarchive_route2<Archive>::save_binary(
	void const* t, std::size_t const size)
{
	Archive::save_binary(t, size);
}

template<typename Archive>
void polymorphic_oarchive_route2<Archive>::save_start(char const* name)
{
	Archive::save_start(name);
}

template<typename Archive>
void polymorphic_oarchive_route2<Archive>::save_end(char const* name)
{
	Archive::save_end(name);
}

template<typename Archive>
void polymorphic_oarchive_route2<Archive>::end_preamble()
{
	Archive::end_preamble();
}

template<typename Archive>
void polymorphic_oarchive_route2<Archive>::register_basic_serializer(
	detail::basic_oserializer const& bos)
{
	Archive::register_basic_serializer(bos);
}

template<typename Archive>
template<class T>
polymorphic_oarchive&
polymorphic_oarchive_route2<Archive>::operator<< (T const& t)
{
	return polymorphic_oarchive::operator<<(t);
}

template<typename Archive>
template<class T>
polymorphic_oarchive&
polymorphic_oarchive_route2<Archive>::operator& (T const& t)
{
	return polymorphic_oarchive::operator&(t);
}

template<typename Archive>
template<class T>
detail::basic_pointer_oserializer const*
polymorphic_oarchive_route2<Archive>::register_type(T* t)
{
	return Archive::register_type(t);
}

template<typename Archive>
template<typename T>
polymorphic_oarchive_route2<Archive>::polymorphic_oarchive_route2(
	T& t,
	unsigned int const flags) :
		Archive(t, flags)
{}

template<typename Archive>
polymorphic_oarchive_route2<Archive>::~polymorphic_oarchive_route2()
{}

} // detail
} // archive
} // boost
