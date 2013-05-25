// Copyright 2013, Sebastian Jeltsch (sjeltsch@kip.uni-heidelberg.de)
// Distributed under the terms of the LGPLv3 or newer.

#define BOOST_ARCHIVE_SOURCE
#include "boost/archive/json_oarchive.hpp"
#include <boost/archive/detail/archive_serializer_map.hpp>
#include <boost/archive/impl/archive_serializer_map.ipp>

namespace boost {
namespace archive {

template class detail::archive_serializer_map<json_oarchive>;

} // namespace archive
} // namespace boost
