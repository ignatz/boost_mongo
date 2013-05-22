// Copyright 2013, Sebastian Jeltsch (sjeltsch@kip.uni-heidelberg.de)
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <boost/archive/polymorphic_json_oarchive.hpp>
#include <boost/archive/polymorphic_json_iarchive.hpp>
#include <fstream>

typedef boost::archive::polymorphic_json_oarchive test_oarchive;
typedef boost::archive::polymorphic_json_iarchive test_iarchive;

typedef std::ofstream test_ostream;
typedef std::ifstream test_istream;
