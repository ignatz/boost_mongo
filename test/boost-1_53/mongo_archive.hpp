#pragma once

// Copyright 2013, Sebastian Jeltsch (sjeltsch@kip.uni-heidelberg.de)
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <boost/archive/mongo_oarchive.hpp>
#include <boost/archive/mongo_iarchive.hpp>

#include "mongo_stream.hpp"

typedef boost::archive::mongo_oarchive test_oarchive;
typedef boost::archive::mongo_iarchive test_iarchive;
