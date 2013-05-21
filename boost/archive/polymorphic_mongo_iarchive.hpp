#pragma once

// Copyright 2013, Sebastian Jeltsch (sjeltsch@kip.uni-heidelberg.de)
// Distributed under the terms of the LGPLv3 or newer.

#include "boost/archive/mongo_iarchive.hpp"

#include <boost/archive/detail/polymorphic_iarchive_route.hpp>
#include <boost/archive/detail/abi_prefix.hpp> // must be the last header

namespace boost {
namespace archive {

class polymorphic_mongo_iarchive :
	public polymorphic_iarchive,
	public mongo_iarchive
{
private:
	std::vector<bool> _is_obj;

	// these are used by the serialization library.
	virtual void load_object(
		void* t,
		detail::basic_iserializer const& bis)
	{
		mongo_iarchive::load_object(t, bis);
	}
	virtual const detail::basic_pointer_iserializer* load_pointer(
		void*& t,
		detail::basic_pointer_iserializer const* bpis_ptr,
		detail::basic_pointer_iserializer const* (*finder)(
			boost::serialization::extended_type_info const& type))
	{
		return mongo_iarchive::load_pointer(t, bpis_ptr, finder);
	}
	virtual void set_library_version(
		library_version_type archive_library_version)
	{
		mongo_iarchive::set_library_version(archive_library_version);
	}
	virtual library_version_type get_library_version() const
	{
		return mongo_iarchive::get_library_version();
	}
	virtual unsigned int get_flags() const
	{
		return mongo_iarchive::get_flags();
	}
	virtual void delete_created_pointers()
	{
		mongo_iarchive::delete_created_pointers();
	}
	virtual void reset_object_address(
		void const* new_address,
		void const* old_address)
	{
		mongo_iarchive::reset_object_address(new_address, old_address);
	}
	virtual void load_binary(void* t, std::size_t size)
	{
		mongo_iarchive::load_binary(t, size);
	}

	// primitive types the only ones permitted by polymorphic archives
	virtual void load(bool& t)
	{
		mongo_iarchive::load(t);
	}
	virtual void load(char& t)
	{
		mongo_iarchive::load(t);
	}
	virtual void load(signed char& t)
	{
		mongo_iarchive::load(t);
	}
	virtual void load(unsigned char& t)
	{
		mongo_iarchive::load(t);
	}
	virtual void load(wchar_t& t)
	{
		mongo_iarchive::load(t);
	}
	virtual void load(short& t)
	{
		mongo_iarchive::load(t);
	}
	virtual void load(unsigned short& t)
	{
		mongo_iarchive::load(t);
	}
	virtual void load(int& t)
	{
		mongo_iarchive::load(t);
	}
	virtual void load(unsigned int& t)
	{
		mongo_iarchive::load(t);
	}
	virtual void load(long& t)
	{
		mongo_iarchive::load(t);
	}
	virtual void load(unsigned long& t)
	{
		mongo_iarchive::load(t);
	}
	virtual void load(long long& t)
	{
		mongo_iarchive::load(t);
	}
	virtual void load(unsigned long long& t)
	{
		mongo_iarchive::load(t);
	}
	virtual void load(float& t)
	{
		mongo_iarchive::load(t);
	}
	virtual void load(double& t)
	{
		mongo_iarchive::load(t);
	}
	virtual void load(std::string& t)
	{
		mongo_iarchive::load(t);
	}
	virtual void load(std::wstring& t)
	{
		mongo_iarchive::load(t);
	}

	// NVPs are unpacked in polynomic_oarchive(nvp<T> const& t, int)
	// and then save is called upon the `t.value()`. Therefore, we need to
	// control the internal mongo archive stack from here
	virtual void load_start(char const* name)
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

virtual void load_end(char const* name)
	{
		if (name) {
			if(_is_obj.back()) {
				_stack.pop_back();
			}
			pop_element();
			_is_obj.pop_back();
		}
}

virtual void register_basic_serializer(
		detail::basic_iserializer const& bis)
	{
		mongo_iarchive::register_basic_serializer(bis);
	}

public:
	// this can't be inheriteded because they appear in mulitple
	// parents
	typedef mpl::bool_<true> is_loading;
	typedef mpl::bool_<false> is_saving;

	// the >> operator
	template<class T>
	polymorphic_iarchive& operator>> (T& t)
	{
		return polymorphic_iarchive::operator>>(t);
	}

	// the & operator
	template<class T>
	polymorphic_iarchive& operator& (T& t)
	{
		return polymorphic_iarchive::operator&(t);
	}

	// register type function
	template<class T>
	detail::basic_pointer_iserializer const*
	register_type(T* t = NULL)
	{
		return mongo_iarchive::register_type(t);
	}

	polymorphic_mongo_iarchive(mongo::BSONObj const& obj,
							   unsigned int const flags = 0) :
		mongo_iarchive(obj, flags),
		_is_obj()
	{}

	virtual ~polymorphic_mongo_iarchive() {};
};

} // archive
} // boost

#include <boost/archive/detail/abi_suffix.hpp> // pops abi_suffix.hpp pragmas

// required by export
BOOST_SERIALIZATION_REGISTER_ARCHIVE(
	boost::archive::polymorphic_mongo_iarchive)
