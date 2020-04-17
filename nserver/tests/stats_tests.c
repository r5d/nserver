/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright © 2010, Zed A. Shaw.
 * Copyright © 2020 rsiddharth <s@ricketyspace.net>
 */

#include "minunit.h"
#include <stats.h>
#include <string.h>

/**
 * tests for other stats functions are in
 * ../../liblcthw/tests/stats_tests.c
 */

static char *st_str = NULL;

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

    st_str = Stats_stringify(st);
    mu_assert(st_str != NULL, "stats stringify failed");

    char *expected_st_str = "8238.34:4260238.83:28:28.39:238.27";
    mu_assert(strncmp(st_str, expected_st_str, strlen(expected_st_str)) == 0,
              "stringified str invalid");

    // cleanup
    free(st);

    return NULL;
}

char *test_stats_unstringify()
{
    mu_assert(st_str != NULL, "st_str not initialized");

    Stats *st = Stats_unstringify(st_str);
    mu_assert(st != NULL, "stats unstringify failed");

    mu_assert(st->sum == 8238.34, "stats sum incorrect");
    mu_assert(st->sumsq == 4260238.83, "stats sumsq incorrect");
    mu_assert(st->n == 28, "stats n incorrect");
    mu_assert(st->min == 28.39, "stats min incorrect");
    mu_assert(st->max == 238.27, "stats max incorrect");

    // clean up
    free(st);
    free(st_str);

    return NULL;
}

char *all_tests()
{
    mu_suite_start();

    mu_run_test(test_stats_stringify);
    mu_run_test(test_stats_unstringify);

    return NULL;
}

RUN_TESTS(all_tests);
