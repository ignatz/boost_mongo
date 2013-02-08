#pragma once

// Copyright (c) 2013, Sebastian Jeltsch (sjeltsch@kip.uni-heidelberg.de)
// Distributed under the Boost Software License, Version 1.0.
// (See http://www.boost.org/LICENSE_1_0.txt)

#include <vector>
#include <list>
#include <cstddef>
#include <string>

#include "boost/archive/mongo_common.h"

#include <boost/utility/enable_if.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/mpl/assert.hpp>

#include <boost/archive/detail/register_archive.hpp>
#include <boost/archive/detail/common_iarchive.hpp>
#include <boost/archive/shared_ptr_helper.hpp>

#include <boost/archive/detail/abi_prefix.hpp> // must be the last header

namespace boost {
namespace archive {

class mongo_iarchive :
    public detail::common_iarchive<mongo_iarchive>,
	public detail::shared_ptr_helper
{
protected:
	friend class detail::interface_iarchive<mongo_iarchive>;
	friend class load_access;

	typedef mongo::BSONElement type;
	typedef std::list<type> value_type;

	std::vector<value_type> _stack;
	unsigned int const _flags;

	type& last()
	{
		assert(_stack.size()>0 && _stack.back().size()>0);
		return _stack.back().front();
	}

	void pop_last()
	{
		assert(_stack.size()>0 && _stack.back().size()>0);
		_stack.back().pop_front();
	}

    // Anything not an attribute and not a name-value pair is an
    // error and should be trapped here.
    template<typename T>
	typename boost::enable_if_c<
			!fusion::result_of::has_key<meta_type_mapping, T>::type::value
		>::type
    load_override(T& t, int x)
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
    load_override(T& t, int)
    {
		using namespace boost::fusion::result_of;
		load_field_and_cast<typename value_at_key<meta_type_mapping, T>::type&>(
			fusion::at_key<T>(meta_type_names), t);
	}

    // special treatment for name-value pairs.
    template<typename T>
    void load_override(boost::serialization::nvp<T> const& t, int x)
	{
		// we have an enum type if name is NULL ... seriously!1!!
		if (t.name() == NULL) {
			load_enum(t.value());
			return;
		}

		assert(strcmp(t.name(), last().fieldName()) == 0);
		bool const is_obj = last().isABSONObj();
		if (is_obj) {
			value_type tmp;
			last().Obj().elems(tmp);
			_stack.push_back(value_type());
			_stack.back().swap(tmp);
		}

		detail::common_iarchive<mongo_iarchive>::load_override(t.value(), x);

		if (is_obj) {
			_stack.pop_back();
		}

		pop_last();
    }

	void load_override(class_name_type& t, int);

	// specialization for any array type
	template<typename T>
	typename boost::enable_if_c<!detail::is_compressable<T>::value>::type
	load_override(boost::serialization::array<T> const& a, int x)
	{
		using boost::serialization::make_nvp;
		for (size_t ii = 0; ii<a.count(); ++ii)
		{
			std::string s = boost::lexical_cast<std::string>(ii);
			load_override(make_nvp(s.c_str(), *(a.address()+ii)), x);
		}
	}

	template<typename T>
	typename boost::enable_if_c<detail::is_compressable<T>::value>::type
	load_override(boost::serialization::array<T> const& a, int x)
	{
		using boost::serialization::make_nvp;

		if (!(_flags & sparse_array)) {
			for (size_t ii = 0; ii<a.count(); ++ii)
			{
				assert(boost::lexical_cast<std::string>(ii) == last().fieldName());
				load_override(make_nvp(last().fieldName(), *(a.address()+ii)), x);
			}
		} else {
			for (size_t ii = 0; ii<a.count(); ++ii)
			{
				if (_stack.back().size() && boost::lexical_cast<std::string>(ii) == last().fieldName()) {
					load_override(make_nvp(last().fieldName(), *(a.address()+ii)), x);
				} else {
					*(a.address()+ii) = T();
				}
			}

		}
	}

	template<typename U, typename T>
	void load_field_and_cast(const char* /*field_name*/, T& t)
	{
		load(static_cast<U>(t));
		pop_last();
	}

	template<typename T>
	typename boost::enable_if_c<
			!fusion::result_of::has_key<bson_type_mapping, T>::type::value
		>::type
	load(T& t);

	template<typename T>
	typename boost::enable_if_c<
			fusion::result_of::has_key<bson_type_mapping, T>::type::value
		>::type
	load(T& t);

	// required overload for boost serialization
	void load(boost::serialization::collection_size_type& t);
	void load(boost::serialization::item_version_type& t);

	// required for strings
	void load(std::string& s);

    template<typename T>
	void load_enum(T&) { assert(false); }
	void load_enum(int& t) { last().Val(t); }

public:
	enum flags {
		sparse_array = 1,
		is_json = 2,
		flags_last = 2
	};

	struct use_array_optimization
	{
		template<typename T>
		struct apply
		{
			typedef boost::mpl::true_ type;
		};
	};

    mongo_iarchive(mongo::BSONObj const& obj, unsigned int const flags = 0) :
		_stack(1), _flags(flags)
	{
		obj.elems(_stack.back());

		// remove _id field added by mongodb
		if (obj.hasField("_id")) {
			assert(strcmp("_id", last().fieldName()) == 0);
			pop_last();
		}
	}

    void load_binary(void* address, std::size_t count);
};


namespace detail {

template<>
struct load_array_type<mongo_iarchive>
{
    template<typename T>
    static void invoke(mongo_iarchive& ar, T& t)
	{
		typedef typename boost::remove_extent<T>::type value_type;

		// consider alignment
		size_t const c = sizeof(t) / sizeof(value_type);

		boost::serialization::collection_size_type count(c);
		ar >> BOOST_SERIALIZATION_NVP(count);
		ar >> serialization::make_array(static_cast<value_type*>(&t[0]), count);
    }

    static void invoke(mongo_iarchive& ar, char* t)
	{
		ar.load_binary(t, std::strlen(t) + 1);
    }
};

} // detail
} // archive
} // boost

// required by export
BOOST_SERIALIZATION_REGISTER_ARCHIVE(boost::archive::mongo_iarchive)
BOOST_SERIALIZATION_USE_ARRAY_OPTIMIZATION(boost::archive::mongo_iarchive)

#include <boost/archive/detail/abi_suffix.hpp> // pops abi_suffix.hpp pragmas

namespace boost {
namespace archive {

inline
void mongo_iarchive::load_override(class_name_type& t, int)
{
	std::string str = last().String();
	t.t = new char[str.size()+1];
	str.copy(t.t, std::string::npos);
}


template<typename T>
typename boost::enable_if_c<
		!fusion::result_of::has_key<bson_type_mapping, T>::type::value
	>::type
mongo_iarchive::load(T& t)
{
	if (_flags & is_json)
		t = last().number();
	else
		last().Val(t);
}

template<typename T>
typename boost::enable_if_c<
		fusion::result_of::has_key<bson_type_mapping, T>::type::value
	>::type
mongo_iarchive::load(T& t)
{
	if (_flags & is_json)
		t = last().number();
	else {
		using namespace boost::fusion::result_of;
		typename value_at_key<bson_type_mapping, T>::type tmp;
		last().Val(tmp);
		t = tmp;
	}
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
	last().Val(s);
}


inline
void mongo_iarchive::load_binary(void* address, std::size_t count)
{
	std::string str = last().String();
	assert(str.size() == count);
	str.copy(static_cast<char*>(address), count);
}

} // archive
} // boost
