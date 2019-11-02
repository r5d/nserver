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

char *all_tests()
{
    mu_suite_start();

    mu_run_test(test_sscreate);

    return NULL;
}

RUN_TESTS(all_tests);
