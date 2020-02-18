#include "minunit.h"
#include <protocol.h>

char *test_sscreate()
{
    int rc = 0;

    rc = sscreate("/crimson");
    mu_assert(rc == 0, "sscreate failed 0");

    rc = sscreate("/vermilion");
    mu_assert(rc == 0, "sscreate failed 1");

    rc = sscreate("/crimson");
    mu_assert(rc == 1, "sscreate failed 2");

    rc = sscreate("/crimson/sky");
    mu_assert(rc == 0, "sscreate failed 3");

    return NULL;
}

char *test_sssample()
{
    double mean = 0;

    mean = sssample("/crimson", 3);
    mu_assert(mean == 3.0, "sssample failed 0");

    mean = sssample("/crimson", 9);
    mu_assert(mean == 6.0, "sssample failed 1");

    mean = sssample("/crimson", 12);
    mu_assert(mean == 8.0, "sssample failed 2");

    mean = sssample("/vermilion", 20);
    mu_assert(mean == 20.0, "sssample failed 3");

    mean = sssample("/vermilion", 27);
    mu_assert(mean == 23.5, "sssample failed 4");

    mean = sssample("/vermilion", 4);
    mu_assert(mean == 17.0, "sssample failed 5");

    mean = sssample("/ruby", 48);
    mu_assert(mean == -1, "sssample failed 6");

    mean = sssample("/crimson/sky", 42);
    mu_assert(mean == 42.0, "sssample failed 7");

    mean = sssample("/crimson", 10);
    mu_assert(mean == 15.20, "sssample failed 8");

    return NULL;
}

char *test_ssmean()
{
    double m = 0;

    m = ssmean("/crimson");
    mu_assert(m == 15.20, "ssmean failed 0");

    m = ssmean("/vermilion");
    mu_assert(m == 17, "ssmean failed 1");

    m = ssmean("/ruby");
    mu_assert(m == -1, "ssmean failed 2");

    return NULL;
}

char *test_ssdump()
{
    char *dstr = NULL;

    dstr = ssdump("/crimson");
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

char *test_sslist()
{
    char *ks = sslist();
    mu_assert(ks != NULL, "sslist failed");
    mu_assert(strlen(ks) == 19, "length check failed");

    debug("KEYS:\n %s", ks);

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
    /*mu_run_test(test_sslist);
    mu_run_test(test_ssdelete);*/

    return NULL;
}

RUN_TESTS(all_tests);
