#include "minunit.h"
#include <protocol.h>

char *test_sscreate()
{
    int rc = 0;

    rc = sscreate("crimson");
    mu_assert(rc == 0, "sscreate failed 0");

    rc = sscreate("/vermilion");
    mu_assert(rc == 0, "sscreate failed 1");

    return NULL;
}

char *test_sssample()
{
    int rc = 0;

    rc = sssample("crimson", 3);
    mu_assert(rc == 0, "sssample failed 0");

    rc = sssample("crimson", 9);
    mu_assert(rc == 0, "sssample failed 1");

    rc = sssample("crimson", 12);
    mu_assert(rc == 0, "sssample failed 2");

    rc = sssample("/vermilion", 20);
    mu_assert(rc == 0, "sssample failed 3");

    rc = sssample("/vermilion", 27);
    mu_assert(rc == 0, "sssample failed 4");

    rc = sssample("/vermilion", 4);
    mu_assert(rc == 0, "sssample failed 5");

    rc = sssample("/ruby", 48);
    mu_assert(rc == -1, "sssample failed 6");

    return NULL;
}

char *all_tests()
{
    mu_suite_start();

    mu_run_test(test_sscreate);
    mu_run_test(test_sssample);

    return NULL;
}

RUN_TESTS(all_tests);
