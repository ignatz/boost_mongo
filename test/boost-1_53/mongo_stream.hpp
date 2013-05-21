#pragma once

// Copyright 2013, Sebastian Jeltsch (sjeltsch@kip.uni-heidelberg.de)
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <string>
#include <map>
#include <stdexcept>

#include <mongo/client/dbclient.h>

typedef std::map<std::string, mongo::BSONObj> map_t;
static map_t gMap;


// Output
class mongo_ostream
{
public:
	template<typename Flags>
	mongo_ostream(const char* fname, Flags const& f) :
		mKey(fname), mBuilder()
	{}

	mongo_ostream(const char* fname) :
		mKey(fname), mBuilder()
	{}
	~mongo_ostream()
	{
		gMap[mKey] = mBuilder.obj();
	}

	typedef mongo::BSONObjBuilder type;
	operator type&       ()       { return mBuilder; }
	operator type const& () const { return mBuilder; }
	type const& builder() const { return mBuilder; }

private:
	std::string mKey;
	type mBuilder;
};

typedef mongo_ostream test_ostream;


// Input
class mongo_istream
{
public:
	template<typename Flags>
	mongo_istream(const char* fname, Flags const& f) :
		mKey(fname)
	{
		init();
	}

	mongo_istream(const char* fname) : mKey(fname)
	{
		init();
	}

	void init()
	{
		map_t::iterator it = gMap.find(mKey);
		if (it == gMap.end()) {
			throw std::runtime_error("uninitialized mongo_stream sink");
		}
		mObject = it->second;
	}

	typedef mongo::BSONObj type;
	operator type&       ()       { return mObject; }
	operator type const& () const { return mObject; }
	type const& obj() const { return mObject; }

private:
	std::string mKey;
	type mObject;
};

typedef mongo_istream test_istream;
