// Copyright 2013, Sebastian Jeltsch (sjeltsch@kip.uni-heidelberg.de)
// Distributed under the terms of the LGPLv3 or newer.

#include "test/MongoArchive.h"
#include "test/Base.h"

#include <boost/preprocessor/arithmetic/dec.hpp>
#include <boost/preprocessor/arithmetic/inc.hpp>
#include <boost/preprocessor/comparison/not_equal.hpp>
#include <boost/preprocessor/repetition/for.hpp>
#include <boost/preprocessor/tuple/elem.hpp>
#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/seq/fold_left.hpp>

#define PRED(r, state) \
    BOOST_PP_NOT_EQUAL( \
        BOOST_PP_TUPLE_ELEM(2, 0, state), \
        BOOST_PP_INC(BOOST_PP_TUPLE_ELEM(2, 1, state)))

#define OP(r, state) ( \
        BOOST_PP_INC(BOOST_PP_TUPLE_ELEM(2, 0, state)), \
        BOOST_PP_TUPLE_ELEM(2, 1, state))

#define MACRO(r, state) (x)
#define CONCAT(s, state, x) BOOST_PP_CAT(state, x)

#define N_TIMES_X(N) BOOST_PP_SEQ_FOLD_LEFT( \
    CONCAT, \
    BOOST_PP_SEQ_HEAD(BOOST_PP_FOR((1, N), PRED, OP, MACRO)), \
    BOOST_PP_SEQ_TAIL(BOOST_PP_FOR((1, N), PRED, OP, MACRO)))

#define CLASS_NAME_LENGTH BOOST_PP_DEC(BOOST_SERIALIZATION_MAX_KEY_SIZE)
#define TEST_CLASS_NAME N_TIMES_X( CLASS_NAME_LENGTH )


class TEST_CLASS_NAME :
	public Base
{
public:
	TEST_CLASS_NAME(int x = int()) : Base(5), member(x) {}
	virtual ~TEST_CLASS_NAME () {}

	int member;
	virtual void fun() {}

	virtual bool operator== (TEST_CLASS_NAME const& rhs) const
	{
		Base const& base = static_cast<Base const&>(*this);
		return base == static_cast<Base const&>(rhs) &&
			member == rhs.member;
	}

private:
	friend class boost::serialization::access;
	template<typename Archive>
	void serialize(Archive& ar, unsigned int const)
	{
		ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(Base)
		   & BOOST_SERIALIZATION_NVP(member);
	}
};

BOOST_CLASS_EXPORT_KEY(TEST_CLASS_NAME)
BOOST_CLASS_EXPORT_IMPLEMENT(TEST_CLASS_NAME)


TEST_F(MongoArchive, LongClassNameRegression)
{
	typedef TEST_CLASS_NAME type;
	Base *x(new type(42)), *y;

	type const& xp = dynamic_cast<type const&>(*x);
	ASSERT_EQ(42, xp.member);
	ASSERT_EQ(5 , x->member);

	serialize(x);
	deserialize(getObject(), y);

	type const& yp = dynamic_cast<type const&>(*y);
	ASSERT_EQ(xp, yp);
}
