#pragma once

// Copyright (c) 2013, Sebastian Jeltsch (sjeltsch@kip.uni-heidelberg.de)
// Distributed under the Boost Software License, Version 1.0.
// (See http://www.boost.org/LICENSE_1_0.txt)

#include <exception>
#include <cassert>
#include <string>

#include <boost/archive/detail/abi_prefix.hpp> // must be the last header

namespace boost {
namespace archive {

// Note: can't inherit from mongo_archive_exception, because internal
// layout changed over past few boost versions.
class mongo_archive_exception :
    public virtual std::exception
{
public:
    enum exception_code {
        mongo_archive_mismatch,
        mongo_archive_name_error,
        mongo_archive_sparse_array,
        mongo_archive_binary
    };

    mongo_archive_exception(exception_code c,
							const char * = NULL,
							const char * = NULL) :
		std::exception(), _msg()
    {
		_msg = "Programming error";

        switch(c) {
			case mongo_archive_mismatch:
				_msg = "BSONObj type/size mismatch";
				break;
			case mongo_archive_name_error:
				_msg= "Invalid BSONObj field name";
				break;
			case mongo_archive_sparse_array:
				_msg = "Mongo archive sparse_array flag mismatch";
				break;
			case mongo_archive_binary:
				_msg = "Binary data size missmatch";
				break;
			default:
				assert(false);
				break;
        }
    }

    virtual ~mongo_archive_exception() throw () {}

    virtual const char *what( ) const throw()
	{
		return _msg.c_str();
	}

protected:
	std::string _msg;
};

} // archive
} // boost

#include <boost/archive/detail/abi_suffix.hpp> // pops abi_suffix.hpp pragmas
