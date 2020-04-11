#ifndef stats_h
#define stats_h
#include <math.h>

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
