#pragma once

// Copyright (c) 2013, Sebastian Jeltsch (sjeltsch@kip.uni-heidelberg.de)
// Distributed under the Boost Software License, Version 1.0.
// (See http://www.boost.org/LICENSE_1_0.txt)

#include <vector>
#include <cstddef>
#include <string>

#include "boost/archive/mongo_common.h"

#include <boost/utility/enable_if.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/mpl/assert.hpp>

#include <boost/archive/detail/register_archive.hpp>
#include <boost/archive/detail/common_oarchive.hpp>

#include <boost/archive/detail/abi_prefix.hpp> // must be the last header

namespace boost {
namespace archive {

class mongo_oarchive :
    public detail::common_oarchive<mongo_oarchive>
{
protected:
	friend class detail::interface_oarchive<mongo_oarchive>;
	friend class save_access;

	typedef mongo::BSONObjBuilder type;
	typedef boost::shared_ptr<type> value_type;

	std::vector<value_type> _stack;
	value_type _current;
	char const* _name;
	unsigned int const _flags;

	type& last()
	{
		assert(_stack.size()>0);
		return *_stack.back();
	}

    // Anything not an attribute and not a name-value pair is an
    // error and should be trapped here.
    template<typename T>
	typename boost::enable_if_c<
			!fusion::result_of::has_key<meta_type_mapping, T>::type::value
		>::type
    save_override(T& t, int x)
    {
        // If your program fails to compile here, its most likely due to
        // not specifying an nvp wrapper around the variable to
        // be serialized.
		BOOST_MPL_ASSERT((serialization::is_wrapper<T>));
        this->detail::common_oarchive<mongo_oarchive>::save_override(t, x);
    }

    template<typename T>
	typename boost::enable_if_c<
			fusion::result_of::has_key<meta_type_mapping, T>::type::value
		>::type
    save_override(T const& t, int x)
    {
		using namespace boost::fusion::result_of;
		typename value_at_key<meta_type_mapping, T>::type tmp(
			static_cast<typename value_at_key<meta_type_mapping, T>::type>(t));
		save_override(boost::serialization::make_nvp(
				fusion::at_key<T>(meta_type_names), tmp), x);
	}

    // special treatment for name-value pairs.
    template<typename T>
    void save_override(boost::serialization::nvp<T> const& t, int)
	{
		// we have an enum type if name is NULL ... seriously!1!!
		if (t.name() == NULL) {
			save_enum(t.const_value());
			return;
		}

		_name = t.name();
		_stack.push_back(_current);
		_current = value_type(new type);

		detail::common_oarchive<mongo_oarchive>::save_override(t.const_value(), 0);

		mongo::BSONObj obj = _current->obj();
		if (!obj.isEmpty()) {
			last().append(t.name(), obj);
		}

		_current = _stack.back();
		_stack.pop_back();
    }

    // specific overrides for attributes - not name value pairs so we
    // want to trap them before the above "fall through"
	void save_override(class_name_type const& t, int);

	template<typename T>
	typename boost::enable_if_c<!detail::is_compressable<T>::value>::type
	save_override(boost::serialization::array<T> const& a, int x)
	{
		using boost::serialization::make_nvp;
		for (size_t ii = 0; ii<a.count(); ++ii)
		{
			std::string s = boost::lexical_cast<std::string>(ii);
			save_override(make_nvp(s.c_str(), *(a.address()+ii)), x);
		}
	}

	template<typename T>
	typename boost::enable_if_c<detail::is_compressable<T>::value>::type
	save_override(boost::serialization::array<T> const& a, int x)
	{
		typedef typename boost::decay<T>::type base_type;
		base_type ref = base_type();
		using boost::serialization::make_nvp;
		for (size_t ii = 0; ii<a.count(); ++ii)
		{
			if (_flags & sparse_array && *(a.address()+ii) == ref)
				continue;

			std::string s = boost::lexical_cast<std::string>(ii);
			save_override(make_nvp(s.c_str(), *(a.address()+ii)), x);
		}
	}


	// detail_common_oarchive::save_override calls save functions
	template<typename T>
	typename boost::enable_if_c<
			!fusion::result_of::has_key<bson_type_mapping, T>::type::value
		>::type
	save(T const& t);

	template<typename T>
	typename boost::enable_if_c<
			fusion::result_of::has_key<bson_type_mapping, T>::type::value
		>::type
	save(T const& t);

	// required overload for boost serialization
	void save(boost::serialization::collection_size_type const& t);

	// required for strings
	void save(std::string const& s);

	// requirest for enum types (null pointer for the win!)
	template<typename T>
	void save_enum(T const&) {}
	void save_enum(int const& e) { save(e);}

public:
	enum flags {
		sparse_array = 1,
		flags_last = 1
	};

	struct use_array_optimization
	{
		template<typename T>
		struct apply
		{
			typedef boost::mpl::true_ type;
		};
	};

    mongo_oarchive(type& obj, unsigned int flags = 0) :
		_stack(), _current(&obj, detail::cond_deleter<type>(false)),
		_name(NULL), _flags(flags)
	{}

    void save_binary(void const* address, std::size_t count);
};


namespace detail {

template<>
struct save_array_type<mongo_oarchive>
{
    template<typename T>
    static void invoke(mongo_oarchive& ar, T const& t)
	{
        typedef typename boost::remove_extent<T>::type value_type;

        // consider alignment
        std::size_t c = sizeof(t) / sizeof(value_type);

        boost::serialization::collection_size_type count(c);
        ar << BOOST_SERIALIZATION_NVP(count);
        ar << serialization::make_array(static_cast<value_type const*>(&t[0]), count);
    }

    static void invoke(mongo_oarchive& ar, char const* t)
	{
		ar.save_binary(t, std::strlen(t) + 1);
    }
};

} // detail
} // archive
} // boost

// required by export
BOOST_SERIALIZATION_REGISTER_ARCHIVE(boost::archive::mongo_oarchive)
BOOST_SERIALIZATION_USE_ARRAY_OPTIMIZATION(boost::archive::mongo_oarchive)

#include <boost/archive/detail/abi_suffix.hpp> // pops abi_suffix.hpp pragmas


namespace boost {
namespace archive {

inline
void mongo_oarchive::save_override(
	class_name_type const& t, int)
{
	_current->append("_class_name", t);
}


template<typename T>
typename boost::enable_if_c<
		!fusion::result_of::has_key<bson_type_mapping, T>::type::value
	>::type
mongo_oarchive::save(T const& t)
{
	last().append(_name, t);
}

template<typename T>
typename boost::enable_if_c<
		fusion::result_of::has_key<bson_type_mapping, T>::type::value
	>::type
mongo_oarchive::save(T const& t)
{
	using namespace boost::fusion::result_of;
	last().append(_name,
		static_cast<typename value_at_key<bson_type_mapping, T>::type const&>(t));
}

inline
void mongo_oarchive::save(boost::serialization::collection_size_type const& t)
{
	save(static_cast<size_t>(t));
}

inline
void mongo_oarchive::save(std::string const& s)
{
	save_binary(s.c_str(), s.size()+1);
}


inline
void mongo_oarchive::save_binary(
	void const* address, std::size_t count)
{
	last().append(_name, static_cast<char const*>(address), count);
}

} // namespace archive
} // namespace boost
