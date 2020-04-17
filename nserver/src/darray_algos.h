/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright © 2010, Zed A. Shaw.
 * Copyright © 2020 rsiddharth <s@ricketyspace.net>
 */

#ifndef darray_algos_h
#define darray_algos_h

#include <math.h>
#include <stdlib.h>
#include <darray.h>

int DArray_qsort(DArray *array, DArray_compare cmp);

int DArray_heapsort(DArray *array, DArray_compare cmp);

int DArray_mergesort(DArray *array, DArray_compare cmp);

int DArray_fucked_qsort(DArray *array, DArray_compare cmp);

int DArray_fucked_heapsort(DArray *array, DArray_compare cmp);

int DArray_fucked_mergesort(DArray *array, DArray_compare cmp);

#endif
