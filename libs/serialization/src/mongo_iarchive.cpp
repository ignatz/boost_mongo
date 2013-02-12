// Copyright (c) 2013, Sebastian Jeltsch (sjeltsch@kip.uni-heidelberg.de)
// Distributed under the Boost Software License, Version 1.0.
// (See http://www.boost.org/LICENSE_1_0.txt)

#define BOOST_ARCHIVE_SOURCE

#include <boost/config.hpp>
#include <boost/detail/workaround.hpp>

#include "boost/archive/mongo_iarchive.hpp"
#include <boost/archive/detail/archive_serializer_map.hpp>
#include <boost/archive/impl/archive_serializer_map.ipp>

namespace boost {
namespace archive {

template class detail::archive_serializer_map<mongo_iarchive>;

} // namespace archive
} // namespace boost
