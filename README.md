# A MongoDB/JSON Boost Serialization Archive

[![Build Status](https://travis-ci.org/ignatz/boost_mongo.png?branch=master)](https://travis-ci.org/ignatz/boost_mongo)

`MongoDB` is a popular document database and `boost::serialization` a great
`C++` marshalling library. Wouldn't it be even greater to patch these two
together? - This is where `boost_mongo` comes in.

`boost_mongo` serializes all your custom data types into mongo collections in
human readable format.
It can be used like any other boost archive and if your code already works with
xml archives, you are good to go.
Besides, the `json_archive`s can be used for serialization to and from plain
`JSON` files.

## Usage
The mongo archives can be used just like the boost xml archives. This means,
that members need to be wrapped as name-value-pairs for serialization (see
http://www.boost.org/doc/libs/1_54_0/libs/serialization/doc/wrappers.html#nvp
for more detail).

This is a simple example, illustrating how `boost_mongo` serialization looks
like:

	struct A
	{
		int a;
	
	private:
		friend class boost::serialization::access;
		template<typename Archive>
		void serialize(Archive& ar, unsigned int const)
		{
			ar & BOOST_SERIALIZATION_NVP(a);
		}
	};
	
	// ...
	A a;
	mongo::BSONObjBuilder builder;
	mongo_oarchive out(builder);
	out << boost::serialization::make_nvp("myData", a);
	// ...

The `mongo_oarchive` and `mongo_iarchve` expect a `mongo::BSONObjBuilder` and a
`mongo::BSONObj`, respectively. These containers can then be handled by the
mongo `C++` client drivers.

## Testing
`boost_mongo` passes *all* tests shipped with the boost distribution.
There are also many dedicated `boost_mongo` tests, including CI and code
coverage.

## Requirements
* boost (>=1.46)
* mongo C++ client driver

## Licensing
Copyright 2013, Sebastian Jeltsch (sjeltsch@kip.uni-heidelberg.de)

boost_mongo is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published
by the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

boost_mongo is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with boost_mongo.  If not, see <http://www.gnu.org/licenses/>.
