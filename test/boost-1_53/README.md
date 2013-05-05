# Boost 1.53 Serialization Test

The tests contained in this directory originate from the boost source
distribution in Version 1.53.
All tests testing only a particular feature of `boost::serialization` or a
particular archive type have been excluded.

## Changes

The tests provided within this directory show some minor modifications
compared to the ones distributed as part of the Boost Software Package.

Most of them were necessary, because `mongo_archive`s unlike `xml_archive`s
are not NVP key agnostic. The archives expect identical NVP keys during
serialization as well as deserialization.

## LICENSE

Other than the `boost_mongo` library (LGPLv3) all tests and modifications are
distributed under the Boost Software License.

Use, modification and distribution is subject to the Boost Software
License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
