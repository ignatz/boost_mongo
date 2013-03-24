#pragma once

#include <gtest/gtest.h>

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
