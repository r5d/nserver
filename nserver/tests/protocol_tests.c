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

char *test_ssmean()
{
    double m = 0;

    m = ssmean("crimson");
    mu_assert(m == 8, "ssmean failed 0");

    m = ssmean("/vermilion");
    mu_assert(m == 17, "ssmean failed 1");

    m = ssmean("/ruby");
    mu_assert(m == -1, "ssmean failed 2");

    return NULL;
}

char *test_ssdump()
{
    char *dstr = NULL;

    dstr = ssdump("crimson");
    mu_assert(dstr != NULL, "ssdump failed 0");
    debug("DUMP: %s", dstr);

    // clean up.
    free(dstr);

    dstr = ssdump("/vermilion");
    mu_assert(dstr != NULL, "ssdump failed 1");
    debug("DUMP: %s", dstr);

    // clean up.
    free(dstr);

    dstr = ssdump("/ruby");
    mu_assert(dstr == NULL, "ssdump failed 2");
    debug("DUMP: %s", dstr);

    // clean up.
    free(dstr);

    return NULL;
}

char *test_ssdelete()
{
    int rc = 0;

    rc = ssdelete("crimson");
    mu_assert(rc == 0, "delete failed 0");

    rc = ssdelete("/vermilion");
    mu_assert(rc == 0, "delete failed 1");

    rc = ssdelete("/ruby");
    mu_assert(rc == 0, "delete failed 2");

    return NULL;
}

char *all_tests()
{
    mu_suite_start();

    mu_run_test(test_sscreate);
    mu_run_test(test_sssample);
    mu_run_test(test_ssmean);
    mu_run_test(test_ssdump);
    mu_run_test(test_ssdelete);

    return NULL;
}

RUN_TESTS(all_tests);
