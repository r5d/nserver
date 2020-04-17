/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright © 2010, Zed A. Shaw.
 * Copyright © 2020 rsiddharth <s@ricketyspace.net>
 */

#include "minunit.h"
#include <stats.h>
#include <string.h>
#include <math.h>

static char *st_str = NULL;

const int NUM_SAMPLES = 10;
double samples[] = {
                    6.1061334, 9.6783204, 1.2747090, 8.2395131, 0.3333483,
                    6.9755066, 1.0626275, 7.6587523, 4.9382973, 9.5788115
};

Stats expect = {
                .sumsq = 425.1641,
                .sum = 55.84602,
                .min = 0.333,
                .max = 9.678,
                .n = 10
};

double expect_mean = 5.584602;
double expect_stddev = 3.547868;

#define EQ(X,Y,N) (round((X) * pow(10, N)) == round((Y) * pow(10, N)))

char *test_operations()
{
    int i = 0;
    Stats *st = Stats_create();
    mu_assert(st != NULL, "Fail to create stats.");

    for (i = 0; i < NUM_SAMPLES; i++) {
        Stats_sample(st, samples[i]);
    }

    Stats_dump(st);

    mu_assert(EQ(st->sumsq, expect.sumsq, 3), "sumsq not valid");
    mu_assert(EQ(st->sum, expect.sum, 3), "sum not valid");
    mu_assert(EQ(st->min, expect.min, 3), "min not valid");
    mu_assert(EQ(st->max, expect.max, 3), "max not valid");
    mu_assert(EQ(st->n, expect.n, 3), "n not valid");
    mu_assert(EQ(expect_mean, Stats_mean(st), 3), "mean not valid");
    mu_assert(EQ(expect_stddev, Stats_stddev(st), 3),
                 "stddev not valid");

    return NULL;
}

char *test_recreate()
{
    Stats *st = Stats_recreate(
            expect.sum, expect.sumsq, expect.n, expect.min, expect.max);

    mu_assert(st->sum == expect.sum, "sum not equal");
    mu_assert(st->sumsq == expect.sumsq, "sumsq not equal");
    mu_assert(st->n == expect.n, "n not equal");
    mu_assert(st->min == expect.min, "min not equal");
    mu_assert(st->max == expect.max, "max not equal");
    mu_assert(EQ(expect_mean, Stats_mean(st), 3), "mean not valid");
    mu_assert(EQ(expect_stddev, Stats_stddev(st), 3),
              "stddev not valid");

    return NULL;
}

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

    mu_run_test(test_operations);
    mu_run_test(test_recreate);

    mu_run_test(test_stats_stringify);
    mu_run_test(test_stats_unstringify);

    return NULL;
}

RUN_TESTS(all_tests);
