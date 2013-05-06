#pragma once

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// test_tools.hpp
//
// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com . 
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.

#include <cstdlib>

#define BOOST_FILESYSTEM_VERSION 3
#include <boost/filesystem.hpp>

// (C) Copyright 2010 Dean Michael Berris.
// Instead of using the potentially dangrous tempnam function that's part
// of the C standard library, on Unix/Linux we use the more portable and
// "safe" unique_path function provided in the Boost.Filesystem library.

#include <boost/archive/tmpdir.hpp>

namespace boost {
namespace archive {
    char const * tmpnam(char * buffer) {
        static char name[512] = {0};
        if (name[0] == 0) {
            boost::filesystem::path tempdir(tmpdir());
            boost::filesystem::path tempfilename =
                boost::filesystem::unique_path("serialization-%%%%");
            boost::filesystem::path temp = tempdir / tempfilename;
            std::strcat(name, temp.string().c_str());
        }
        if (buffer != 0) std::strcpy(buffer, name);
        return name;
    }
} // archive
} // boost


#if ! defined(BOOST_ARCHIVE_TEST)
#define BOOST_ARCHIVE_TEST text_archive.hpp
#endif

#include <boost/preprocessor/stringize.hpp>
#include BOOST_PP_STRINGIZE(BOOST_ARCHIVE_TEST)

#ifndef TEST_STREAM_FLAGS
    #define TEST_STREAM_FLAGS (std::ios_base::openmode)0
#endif

#ifndef TEST_ARCHIVE_FLAGS
    #define TEST_ARCHIVE_FLAGS 0
#endif

#define BOOST_TEST_ALTERNATIVE_INIT_API
#include <boost/test/unit_test.hpp>

int test_main(int /* argc */, char* /* argv */[]);

inline
void call_test_main()
{
	test_main(0, static_cast<char**>(NULL));
}

inline
bool init_unit_test_suite()
{
	// you CAN'T use testing tools here
	boost::unit_test::framework::master_test_suite().add( BOOST_TEST_CASE(&call_test_main) );
	return true;
}

int main(int argc, char* argv[])
{
	 return ::boost::unit_test::unit_test_main(
		 &init_unit_test_suite, argc, argv);
}
