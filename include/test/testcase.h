#ifndef _GUIC_TEST_TESTCASE_H
#define _GUIC_TEST_TESTCASE_H

#include <stdint.h>
#include <stdio.h>
#include <stddef.h>
#include <setjmp.h>

#define ANSI_GREEN "\033[0;32m"
#define ANSI_WHITE "\033[01;37m"
#define ANSI_RED   "\033[0;31m"

static jmp_buf err_jmp;

typedef void (*testcase_fptr)(void);

struct __guic_testunit {
    const char *suite;
    const char *test;

    testcase_fptr fptr;

    unsigned int magic;
};

typedef int (*suite_filter_fptr)(struct __guic_testunit *);

#define GUIC_TEST_MAGIC 0xbeefface
#define GUIC_TEST_NAME(x) __guic_test_##x
#define GUIC_TEST_FUNC_NAME(suite, test) GUIC_TEST_NAME(suite##_##test##_run)
#define GUIC_TEST_STRUCT_NAME(suite_name, test_name) GUIC_TEST_NAME(suite_name##_##test_name##_##_unit)
#define GUIC_TEST_SECTION __attribute__((used, section(".guic_test"), aligned(1)))
#define GUIC_TEST_STRUCT(suite_name, test_name) \
    static struct __guic_testunit GUIC_TEST_STRUCT_NAME(suite_name, test_name) GUIC_TEST_SECTION = { \
        .suite = #suite_name, \
        .test  = #test_name, \
        .fptr  = GUIC_TEST_FUNC_NAME(suite_name, test_name), \
        .magic = GUIC_TEST_MAGIC \
    }

#define GUIC_TEST(suite, test) \
    static void GUIC_TEST_FUNC_NAME(suite, test)(void); \
    GUIC_TEST_STRUCT(suite, test); \
    static void GUIC_TEST_FUNC_NAME(suite, test)(void)

void assert_success();
void assert_failure();

#define ASSERT(expect, real, op) \
{ \
    if ((expect) op (real)) \
        assert_success(); \
    else \
        assert_failure(); \
}

int __guic_test_main(int argc, char *argv[]);

#endif
