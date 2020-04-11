#include <math.h>
#include <stats.h>
#include <stdlib.h>
#include <dbg.h>

Stats *Stats_recreate(double sum, double sumsq, unsigned long n,
                      double min, double max)
{
    Stats *st  = malloc(sizeof(Stats));
    check_mem(st);

    st->sum =  sum;
    st->sumsq = sumsq;
    st->n  = n;
    st->min = min;
    st->max = max;

    return st;

 error:
    return NULL;
}

Stats *Stats_create()
{
    return Stats_recreate(0.0, 0.0, 0L, 0.0, 0.0);
}

void Stats_sample(Stats *st, double s)
{
    st->sum  += s;
    st->sumsq += s * s;

    if (st->n == 0) {
        st->min = s;
        st->max = s;
    } else {
        if (st->min > s)
            st->min = s;
        if (st->max < s)
            st->max = s;
    }

    st->n += 1;
}

char *Stats_dump(Stats *st)
{
    size_t char_sz = sizeof(char);
    size_t dstr_len = 280 * char_sz;

    // allocate space for dump string.
    char *dstr = calloc(dstr_len, char_sz);
    check_mem(dstr);

    // dump into dump str.
    int rc = snprintf(dstr, dstr_len,
                      "sum: %f, sumsq: %f, n: %ld, "
                      "min: %f, max: %f, mean: %f, stddev: %f\n",
                      st->sum, st->sumsq, st->n, st->min, st->max,
                      Stats_mean(st), Stats_stddev(st));
    check(rc > 0, "stats dump failed");

    return dstr;
 error:
    return NULL;
}

char *Stats_stringify(Stats *st)
{
    size_t stats_str_len = 80;

    // allocate space for stringified stats.
    char *stats_str = calloc(stats_str_len, sizeof(char));
    check_mem(stats_str);

    // stringify the stats
    int rc = snprintf(stats_str, stats_str_len,
                      "%.2f::%.2f::%ld::%.2f::%.2f",
                      st->sum, st->sumsq, st->n, st->min, st->max);
    check(rc > 0, "stringify stats failed");

    return stats_str;
 error:
    if (stats_str) {
        free(stats_str);
    }
    return NULL;
}
