/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright © 2010, Zed A. Shaw.
 * Copyright © 2020 rsiddharth <s@ricketyspace.net>
 */

#ifndef _DArray_h
#define _DArray_h
#include <stdlib.h>
#include <assert.h>
#include <dbg.h>

typedef struct DArray {
    int end;
    int max;
    size_t element_size;
    size_t expand_rate;
    void **contents;
} DArray;

typedef int (*DArray_compare) (const void *a, const void *b);

DArray *DArray_create(size_t element_size, size_t initial_max);

void DArray_destroy(DArray *array);

void DArray_clear(DArray *array);

int DArray_expand(DArray *array);

int DArray_contract(DArray *array);

int DArray_push(DArray *array, void *el);

void *DArray_pop(DArray *array);

int DArray_sort_add(DArray *array, void *el, DArray_compare cmp);

int DArray_find(DArray *array, void *el, DArray_compare cmp);

void DArray_clear_destroy(DArray *array);

/**
 * Creates a new DArray using `DArray_create` and copies the
 * `contents` from array to the newly created DArray.
 *
 * Returns newly created DArray.
 */
DArray *DArray_shallow_copy(DArray *array);

#define DArray_last(A) ((A)->contents[(A)->end - 1])
#define DArray_first(A) ((A)->contents[0])
#define DArray_end(A) ((A)->end)
#define DArray_count(A) DArray_end(A)
#define DArray_max(A) ((A)->max)

#define DEFAULT_EXPAND_RATE 300

static inline int DArray_set(DArray *array, int i, void *el)
{
    check(array != NULL, "array cannot be NULL");
    check(i >= 0, "i cannot be lesser than 0");

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstrict-overflow"
    check(i < array->max, "darray attempt to set past max");
#pragma GCC diagnostic pop

    if (i > array->end)
        array->end = i;

    array->contents[i] = el;
    return i;
 error:
    return -1;
}

static inline void *DArray_get(DArray *array, int i)
{
    check(array != NULL, "array cannot be NULL");
    check(i >= 0, "i cannot be lesser than 0");

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstrict-overflow"
    check(i < array->max, "darray attempt to get past max");
#pragma GCC diagnostic pop

    return array->contents[i];
 error:
    return NULL;
}

static inline void *DArray_remove(DArray *array, int i)
{
    check(array != NULL, "array cannot be NULL");
    check(i >= 0, "i cannot be lesser than 0");

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstrict-overflow"
    check(i < array->max, "darray attempt to get past max");
#pragma GCC diagnostic pop

    void *el = array->contents[i];

    array->contents[i] = NULL;

    return el;
 error:
    return NULL;
}

static inline void *DArray_new(DArray *array)
{
    check(array != NULL, "array cannot be NULL");
    check(array->element_size > 0,
          "Can't use DArray_new on 0 size darrays.");

    return calloc(1, array->element_size);

 error:
    return NULL;
}

#define DArray_free(E) free((E))

#endif
