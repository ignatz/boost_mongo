#pragma once

// Copyright 2013, Sebastian Jeltsch (sjeltsch@kip.uni-heidelberg.de)
// Distributed under the terms of the LGPLv3 or newer.

// Most of the source code in this file originates from the header below.
// However, inheriting the interface from the crtp template class was not an
// option, due to constructors being limited to std streams only.
#include <boost/archive/detail/polymorphic_oarchive_route.hpp>
#include <boost/archive/detail/abi_prefix.hpp> // must be the last header

namespace boost {
namespace archive {
namespace detail {

template<typename Archive>
class polymorphic_oarchive_route2 :
	public polymorphic_oarchive,
	public Archive
{
private:
	// these are used by the serialization library.
	virtual void save_object(
		void const* x,
		detail::basic_oserializer const& bos);
	virtual void save_pointer(
		void const* t,
		detail::basic_pointer_oserializer const* bpos_ptr);
	virtual void save_null_pointer();

	// primitive types the only ones permitted by polymorphic archives
	virtual void save(bool const t);
	virtual void save(char const t);
	virtual void save(signed char const t);
	virtual void save(unsigned char const t);
	virtual void save(wchar_t const t);
	virtual void save(short const t);
	virtual void save(unsigned short const t);
	virtual void save(int const t);
	virtual void save(unsigned int const t);
	virtual void save(long const t);
	virtual void save(unsigned long const t);
	virtual void save(long long const t);
	virtual void save(unsigned long long const t);
	virtual void save(float const t);
	virtual void save(double const t);
	virtual void save(std::string const& t);
	virtual void save(std::wstring const& t);

	virtual library_version_type get_library_version() const;
	virtual unsigned int get_flags() const;
	virtual void save_binary(void const* t, std::size_t const size);

	virtual void save_start(char const* name);
	virtual void save_end(char const* name);

	virtual void end_preamble();
	virtual void register_basic_serializer(
		detail::basic_oserializer const& bos);

public:
	// this can't be inheriteded because they appear in mulitple
	// parents
	typedef mpl::bool_<false> is_loading;
	typedef mpl::bool_<true> is_saving;

	// the << operator
	template<class T>
	polymorphic_oarchive& operator<< (T const& t);

	// the & operator
	template<class T>
	polymorphic_oarchive& operator& (T const& t);

	// register type function
	template<class T>
	detail::basic_pointer_oserializer const*
	register_type(T* t = NULL);

	template<typename T>
	polymorphic_oarchive_route2(
		T& t,
		unsigned int const flags = 0);

	virtual ~polymorphic_oarchive_route2();
};

} // detail
} // archive
} // boost

#include <boost/archive/detail/abi_suffix.hpp> // pops abi_suffix.hpp pragmas

#include "boost/archive/impl/polymorphic_oarchive_route2.ipp"
