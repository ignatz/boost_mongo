# Boost 1.53 Serialization Test

The tests provided within this directory originate from Boost 1.53
(www.boost.org). All tests testing only a specific feature of
`boost::serialization` or a particular archive type have been excluded.

## Changes

The provided tests contain minor modifications compared to the ones distributed
as part of Boost.
Mostly, because mongo archives unlike xml archives check NVP keys for
consistency. The same NVP-keys are expected during deserialialization as during
serialization, while `xml_iarchive` simply ignores them.

## LICENSE

Other than the `boost_mongo` library itself (LGPLv3) all tests and
modifications provided within this directory are distributed under the terms
of the *Boost Software License* to meet the intent of the original authors.

Use, modification and distribution is subject to the Boost Software
License, Version 1.0. (http://www.boost.org/LICENSE_1_0.txt)
