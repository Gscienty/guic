#include "test/testcase.h"

GUIC_TEST(testunit_test, test)
{
    ASSERT(1, 1, ==);
}

GUIC_TEST(testunit_test, test2)
{
    ASSERT(1, 2, !=);
}

