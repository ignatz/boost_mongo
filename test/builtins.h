// Copyright 2013, Sebastian Jeltsch (sjeltsch@kip.uni-heidelberg.de)
// Distributed under the terms of the LGPLv3 or newer.

#pragma once

#include <gtest/gtest.h>
#include <string>
#include <boost/numeric/conversion/bounds.hpp>

typedef ::testing::Types<
		bool,
		char,
		signed char,
		unsigned char,
		wchar_t,
#if __cplusplus >= 201103L
		char16_t,
		char32_t,
#endif
		short,
		unsigned short,
		int,
		unsigned int,
		long,
		unsigned long,
		long long,
		unsigned long long,
		float,
		long double,
		double
	> builtin_types;


typedef ::testing::Types<
#if __cplusplus >= 201103L
		//std::u16string,
		//std::u32string,
#endif
		std::string,
		std::wstring
	> string_types;
