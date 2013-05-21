#pragma once

// Copyright 2013, Sebastian Jeltsch (sjeltsch@kip.uni-heidelberg.de)
// Distributed under the terms of the LGPLv3 or newer.

#include "boost/archive/mongo_oarchive.hpp"

#include <boost/archive/detail/polymorphic_oarchive_route.hpp>
#include <boost/archive/detail/abi_prefix.hpp> // must be the last header

namespace boost {
namespace archive {

class polymorphic_mongo_oarchive :
	public polymorphic_oarchive,
	public mongo_oarchive
{
private:
	// these are used by the serialization library.
	virtual void save_object(
		void const* x,
		detail::basic_oserializer const& bos)
	{
		mongo_oarchive::save_object(x, bos);
	}
	virtual void save_pointer(
		void const* t,
		detail::basic_pointer_oserializer const* bpos_ptr)
	{
		mongo_oarchive::save_pointer(t, bpos_ptr);
	}
	virtual void save_null_pointer()
	{
		mongo_oarchive::save_null_pointer();
	}
	// primitive types the only ones permitted by polymorphic archives
	virtual void save(bool const t)
	{
		mongo_oarchive::save(t);
	}
	virtual void save(char const t)
	{
		mongo_oarchive::save(t);
	}
	virtual void save(signed char const t)
	{
		mongo_oarchive::save(t);
	}
	virtual void save(unsigned char const t)
	{
		mongo_oarchive::save(t);
	}
	virtual void save(wchar_t const t)
	{
		mongo_oarchive::save(t);
	}
	virtual void save(short const t)
	{
		mongo_oarchive::save(t);
	}
	virtual void save(unsigned short const t)
	{
		mongo_oarchive::save(t);
	}
	virtual void save(int const t)
	{
		mongo_oarchive::save(t);
	}
	virtual void save(unsigned int const t)
	{
		mongo_oarchive::save(t);
	}
	virtual void save(long const t)
	{
		mongo_oarchive::save(t);
	}
	virtual void save(unsigned long const t)
	{
		mongo_oarchive::save(t);
	}
	virtual void save(long long const t)
	{
		mongo_oarchive::save(t);
	}
	virtual void save(unsigned long long const t)
	{
		mongo_oarchive::save(t);
	}
	virtual void save(float const t)
	{
		mongo_oarchive::save(t);
	}
	virtual void save(double const t)
	{
		mongo_oarchive::save(t);
	}
	virtual void save(std::string const& t)
	{
		mongo_oarchive::save(t);
	}
	virtual void save(std::wstring const& t)
	{
		mongo_oarchive::save(t);
	}
	virtual library_version_type get_library_version() const
	{
		return mongo_oarchive::get_library_version();
	}
	virtual unsigned int get_flags() const
	{
		return mongo_oarchive::get_flags();
	}
	virtual void save_binary(void const* t, std::size_t const size)
	{
		mongo_oarchive::save_binary(t, size);
	}

	// NVPs are unpacked in polynomic_oarchive(nvp<T> const& t, int)
	// and then save is called upon the `t.value()`. Therefore, we need to
	// control the internal mongo archive stack from here
	virtual void save_start(char const* name)
	{
		if (name) {
			_name = name;
			_stack.push_back(std::make_pair(value_type(new type), false));
		}
	}
	virtual void save_end(char const* name)
	{
		if (name) {
			if (!top_inserted()) {
				previous_builder().append(name, top_builder().obj());
			}
			_stack.pop_back();
		}
	}
	virtual void end_preamble()
	{
		mongo_oarchive::end_preamble();
	}
	virtual void register_basic_serializer(detail::basic_oserializer const& bos)
	{
		mongo_oarchive::register_basic_serializer(bos);
	}

public:
	// this can't be inheriteded because they appear in mulitple
	// parents
	typedef mpl::bool_<false> is_loading;
	typedef mpl::bool_<true> is_saving;

	// the << operator
	template<class T>
	polymorphic_oarchive& operator<< (T const& t)
	{
		return polymorphic_oarchive::operator<<(t);
	}

	// the & operator
	template<class T>
	polymorphic_oarchive& operator& (T const& t)
	{
		return polymorphic_oarchive::operator&(t);
	}

	// register type function
	template<class T>
	detail::basic_pointer_oserializer const*
	register_type(T* t = NULL)
	{
		return mongo_oarchive::register_type(t);
	}

	// all current archives take a stream as constructor argument
	polymorphic_mongo_oarchive(mongo::BSONObjBuilder& obj,
							   unsigned int const flags = 0) :
		mongo_oarchive(obj, flags)
	{}

	virtual ~polymorphic_mongo_oarchive() {};
};

} // namespace archive
} // namespace boost

#include <boost/archive/detail/abi_suffix.hpp> // pops abi_suffix.hpp pragmas

// required by export
BOOST_SERIALIZATION_REGISTER_ARCHIVE(
	boost::archive::polymorphic_mongo_oarchive)
