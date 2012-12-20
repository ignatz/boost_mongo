A Mongo/JSON Boost Serialization Archiver
=========================================

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

Requirements
------------
* a `C++11` compiler (so far only tested with g++-4.7 and clang++-3.1)
* mongoDB C++ client driver

Licensing
---------
Copyright (c) 2012, Sebastian Jeltsch (sjeltsch@kip.uni-heidelberg.de)

Distributed under the Boost Software License, Version 1.0.
(See http://www.boost.org/LICENSE_1_0.txt)
