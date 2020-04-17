/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright © 2010, Zed A. Shaw.
 * Copyright © 2020 rsiddharth <s@ricketyspace.net>
 */

#ifndef stats_h
#define stats_h

#include <math.h>
#include <stdlib.h>
#include <bstrlib.h>
#include <dbg.h>

typedef struct Stats {
    double sum;
    double sumsq;
    unsigned long n;
    double min;
    double max;
} Stats;

Stats *Stats_recreate(double sum, double sumsq, unsigned long n,
                      double min, double max);

Stats *Stats_create();

void Stats_sample(Stats *st, double s);

char *Stats_dump(Stats *st);

char *Stats_stringify(Stats *st);

Stats *Stats_unstringify(char *st_str);

static inline double Stats_mean(Stats *st)
{
    return st->sum / st->n;
}

static inline double Stats_stddev(Stats *st)
{
    return sqrt((st->sumsq - (st->sum * st->sum / st->n)) /
                (st->n - 1));
}

#endif
