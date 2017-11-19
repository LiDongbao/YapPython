#include "stdafx.h"
#include <boost/test/unit_test.hpp>
#define BOOST_TEST_MAIN


/*
struct MyClass
{
	MyClass() : _value(0)
	{
		BOOST_TEST_MESSAGE("MyClass Fixture Constructor");
	}
	~MyClass()
	{
		BOOST_TEST_MESSAGE("destroy MyClass Fixture");
	}
	int _value;
};


BOOST_FIXTURE_TEST_SUITE(RootTest, MyClass)

BOOST_AUTO_TEST_CASE(SubTest1)
{
	BOOST_CHECK(_value == 1);
}

BOOST_AUTO_TEST_CASE(SubTest2)
{
	BOOST_CHECK_EQUAL(_value, 2);
	BOOST_CHECK_EQUAL(_value, 0);
}

BOOST_AUTO_TEST_SUITE_END()
*/

BOOST_AUTO_TEST_CASE(test1)
{
	BOOST_CHECK(1 == 2);
}

BOOST_AUTO_TEST_CASE(test2)
{
	BOOST_CHECK(2 == 2);
}