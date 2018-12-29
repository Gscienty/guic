#include "test/testcase.h"
#include <string.h>

static const char *__suite_name;

GUIC_TEST(__suite, __test) { }

int filter_all(struct __guic_testunit *t)
{
    (void) t;
    return 1;
}

int filter_special(struct __guic_testunit *t)
{
    return strncmp(t->suite, __suite_name, strlen(__suite_name)) == 0;
}

int __guic_test_main(int argc, char *argv[])
{
    suite_filter_fptr filter = filter_all;
    int total = 0;
    int idx = 0;
    int fail_count = 0;
    int success_count = 0;
    int jmp_ret;

    if (argc == 2) {
        __suite_name = argv[1];
        filter = filter_special;
    }

    struct __guic_testunit *testunit_start = &GUIC_TEST_STRUCT_NAME(__suite, __test);
    struct __guic_testunit *testunit_end = &GUIC_TEST_STRUCT_NAME(__suite, __test);

    while (1) {
        struct __guic_testunit *t = testunit_start - 1;
        if (t->magic != GUIC_TEST_MAGIC)
            break;
        testunit_start--;
    }

    while (1) {
        struct __guic_testunit *t = testunit_end + 1;
        if (t->magic != GUIC_TEST_MAGIC)
            break;
        testunit_end++;
    }
    testunit_end++;

    static struct __guic_testunit *testunit;
    for (testunit = testunit_start; testunit != testunit_end; testunit++) {
        if (testunit == &GUIC_TEST_STRUCT_NAME(__suite, __test))
            continue;
        if (filter(testunit))
            total++;
    }

    for (testunit = testunit_start; testunit != testunit_end; testunit++) {
        if (testunit == &GUIC_TEST_STRUCT_NAME(__suite, __test) || !filter(testunit))
            continue;
        idx++;
        printf("TEST %d/%d %s.%s ", idx, total, testunit->suite, testunit->test);
        fflush(stdout);

        jmp_ret = setjmp(err_jmp);
        if (!jmp_ret) {
            testunit->fptr();
            success_count++;
            printf(ANSI_GREEN "[Success]\n" ANSI_WHITE);
        }
        else {
            fail_count++;
            printf(ANSI_RED "[Fail]\n" ANSI_WHITE);
        }
    }

    printf("Results: %d tests (%d successed, %d failed)\n", total, success_count, fail_count);

    return 0;
}

void assert_success() { }

void assert_failure()
{
    longjmp(err_jmp, 1);
}

#ifdef DEBUG

int main(int argc, char *argv[])
{
    return __guic_test_main(argc, argv);
}

#endif
