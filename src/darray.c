/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright © 2010, Zed A. Shaw.
 * Copyright © 2020 rsiddharth <s@ricketyspace.net>
 */

#include <darray.h>
#include <darray_algos.h>
#include <assert.h>

DArray *DArray_create(size_t element_size, size_t initial_max)
{
    DArray *array = malloc(sizeof(DArray));
    check_mem(array);
    array->max = initial_max;
    check(array->max > 0, "You must set initial_max > 0.");

    array->contents = calloc(initial_max, sizeof(void *));
    check_mem(array->contents);

    array->end = 0;
    array->element_size = element_size;
    array->expand_rate = DEFAULT_EXPAND_RATE;

    return array;

 error:
    if (array)
        free(array);
    return NULL;
}

void DArray_clear(DArray *array)
{
    check(array != NULL, "array cannot be NULL");

    int i = 0;
    if (array->element_size > 0) {
        for (i = 0; i < array->max; i++) {
            if (array->contents[i] != NULL) {
                free(array->contents[i]);
            }
        }
    }

 error:
    return;
}

static inline int DArray_resize(DArray *array, size_t newsize)
{
    check(array != NULL, "array cannot be NULL");
    check((int) newsize >= array->end, "newsize must be >= end.");

    array->max = newsize;
    check(array->max > 0, "The newsize must be > 0.");

    void *contents = realloc(array->contents,
                             array->max * sizeof(void *));
    // Check contents and assume realloc doesn't harm the original on error.

    check_mem(contents);

    array->contents = contents;

    return 0;
 error:
    return -1;
}

int DArray_expand(DArray *array)
{
    size_t old_max = array->max;
    check(DArray_resize(array, array->max + array->expand_rate) == 0,
          "Failed to expand array to new size: %d",
          array->max + (int) array->expand_rate);

    memset(array->contents + old_max, 0, (array->expand_rate) * sizeof(void *));

    return 0;

 error:
    return -1;
}

int DArray_contract(DArray *array)
{
    check(array != NULL, "array cannot be NULL");

    int new_size = array->end < (int) array->expand_rate ?
        (int) array->expand_rate : array->end;

    return DArray_resize(array, new_size + 1);
 error:
    return -1;
}

void DArray_destroy(DArray *array)
{
    if (array) {
        if (array->contents)
            free(array->contents);
        free(array);
    }
}

void DArray_clear_destroy(DArray *array)
{
    DArray_clear(array);
    DArray_destroy(array);
}

int DArray_push(DArray *array, void *el)
{
    check(array != NULL, "array cannot be NULL");

    array->contents[array->end] = el;
    array->end++;

    if (DArray_end(array) >= DArray_max(array)) {
        return DArray_expand(array);
    } else {
        return 0;
    }

 error:
    return -1;
}

void *DArray_pop(DArray *array)
{
    check(array != NULL, "array cannot be NULL");
    check(array->end - 1 >= 0, "Attempt to pop from empty array.");

    void *el = DArray_remove(array, array->end - 1);
    array->end--;

    if (DArray_end(array) > (int)array->expand_rate
        && DArray_end(array) % array->expand_rate) {
        DArray_contract(array);
    }

    return el;
 error:
    return NULL;
}

int DArray_sort_add(DArray *array, void *el, DArray_compare cmp)
{
    int rc;
    rc = DArray_push(array, el);
    check(rc == 0, "Error pushing element.");

    // sort the array.
    rc = DArray_heapsort(array, cmp);
    check(rc == 0, "Error sorting array.");

    return 0;
 error:
    return -1;
}

DArray *DArray_shallow_copy(DArray *array)
{
    DArray *copy  = DArray_create(array->element_size, array->max);
    check(copy != NULL, "Error creating DArray copy.");

    int i = 0;
    for (i = 0; i < array->max; i++) {
            copy->contents[i] = array->contents[i];
    }

    return copy;
 error:
    return NULL;
}

int DArray_find(DArray *array, void *el, DArray_compare cmp)
{
    int low = 0;
    int high = DArray_end(array) - 1;
    void *c = NULL;

    while (low <= high) {
        int middle =  low + (high - low) / 2;
        c = DArray_get(array, middle);

        if (cmp(&el, &c) < 0) {
            high = middle - 1;
        } else if (cmp(&el, &c) > 0) {
            low = middle + 1;
        } else {
            return middle;
        }
    }

    return -1;
}
