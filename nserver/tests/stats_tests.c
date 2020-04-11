#include "minunit.h"
#include <stats.h>
#include <string.h>

/**
 * tests for other stats functions are in
 * ../../liblcthw/tests/stats_tests.c
 */

char *test_stats_stringify()
{
    Stats *st = Stats_create();
    mu_assert(st != NULL, "stats create failed");

    // fill with dummy data.
    st->sum = 8238.33892;
    st->sumsq = 4260238.8292;
    st->n = 28;
    st->min =  28.3921;
    st->max = 238.27;

    char *st_str = Stats_stringify(st);
    mu_assert(st_str != NULL, "stats stringify failed");

    char *expected_st_str = "8238.34:4260238.83:28:28.39:238.27";
    mu_assert(strncmp(st_str, expected_st_str, strlen(expected_st_str)) == 0,
              "stringified str invalid");

    // cleanup
    free(st_str);
    free(st);

    return NULL;
}

char *all_tests()
{
    mu_suite_start();

    mu_run_test(test_stats_stringify);

    return NULL;
}

RUN_TESTS(all_tests);
