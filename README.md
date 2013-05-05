# A Mongo/JSON Boost Serialization Archiver

This library provides an boost serialization archiver for `mongoDB`.

You can easily store and retrieve all your data in human readable format to
and from your database by means of `boost::serialization`.

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

Furthermore, if you don't care about the mongo database driver overhead, you
can use the library as JSON serialization backend.

## Requirements
* mongoDB C++ client driver

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
