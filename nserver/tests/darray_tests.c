/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright © 2010, Zed A. Shaw.
 * Copyright © 2020 rsiddharth <s@ricketyspace.net>
 */

#include "minunit.h"
#include <darray.h>

static DArray *array = NULL;
static int *val1 = NULL;
static int *val2 = NULL;

int testcmp(int **a, int **b)
{
    int x = **a, y = **b;

    if (x == y) {
        return 0;
    } else if (x < y) {
        return -1;
    } else {
        return 1;
    }
}

char *test_create()
{
    // Test fail.
    array = DArray_create(0, 0);
    mu_assert(array == NULL, "array must be NULL.");

    // Test success.
    array = DArray_create(sizeof(int), 100);
    mu_assert(array != NULL, "DArray_create failed.");
    mu_assert(array->contents != NULL, "contents are wrong in array.");
    mu_assert(array->end == 0, "end isn't at the right spot");
    mu_assert(array->element_size == sizeof(int),
              "element size is wrong.");
    mu_assert(array->max == 100, "wrong max length on initial size");

    return NULL;
}

char *test_destroy()
{
    DArray_destroy(array);

    return NULL;
}

char *test_new()
{
    // Test fail.
    val1 = DArray_new(NULL);
    mu_assert(val1 == NULL, "val1 must be NULL");

    // Test success
    val1 = DArray_new(array);
    mu_assert(val1 != NULL, "failed to make a new element");

    val2 = DArray_new(array);
    mu_assert(val2 != NULL, "failed to make a new element");

    return NULL;
}

char *test_set()
{
    int rc = 0;

    // Test fail.

    // case where array is NULL
    rc = DArray_set(NULL, 0, val1);
    mu_assert(rc == -1, "rc must be -1");

    // case where i < 0
    rc = DArray_set(array, -1, val1);
    mu_assert(rc == -1, "rc must be -1");

    // case where i > array->max
    rc = DArray_set(array, array->max + 1, val1);
    mu_assert(rc == -1, "rc must be -1");

    // Test success.

    // must set val1 at index 0
    rc = DArray_set(array, 0, val1);
    mu_assert(rc == 0, "rc must be 0");

    // must set val2 at index 1
    rc = DArray_set(array, 1, val2);
    mu_assert(rc == 1, "rc must be 1");

    return NULL;
}

char *test_get()
{
    void *rc = NULL;

    // Test fail.

    // case where array is NULL
    rc = DArray_get(NULL, 0);
    mu_assert(rc == NULL, "rc must be NULL");

    // case where i < 0
    rc = DArray_get(array, -2);
    mu_assert(rc == NULL, "rc must be NULL");

    // case where i > array->max
    rc = DArray_get(array, array->max + 1);
    mu_assert(rc == NULL, "rc must be NULL");

    // Test succcess.
    mu_assert(DArray_get(array, 0) == val1, "Wrong first value");
    mu_assert(DArray_get(array, 1) == val2, "Wrong second value");

    return NULL;
}

char *test_remove()
{
    int *val_check = NULL;

    // Test fail.

    // case where array is NULL.
    val_check = DArray_remove(NULL, 0);
    mu_assert(val_check == NULL, "val_check must be NULL");
    mu_assert(array->contents[0] != NULL,
              "array->contents[0] must not be NULL");

    // case where i < 0
    val_check = DArray_remove(array, -1);
    mu_assert(val_check == NULL, "val_check must be NULL");

    // case where i > array->max
    val_check = DArray_remove(array, array->max + 1);
    mu_assert(val_check == NULL, "val_check must be NULL");

    // Test success.
    val_check = DArray_remove(array, 0);
    mu_assert(val_check !=NULL, "Should not get NULL");
    mu_assert(*val_check == *val1, "Should get the first value.");
    mu_assert(DArray_get(array, 0) == NULL, "Should be gone.");
    DArray_free(val_check);

    val_check = DArray_remove(array, 1);
    mu_assert(val_check != NULL, "Should not get NULL.");
    mu_assert(*val_check == *val2, "Should get second value.");
    mu_assert(DArray_get(array, 1) == NULL, "Should be gone");
    DArray_free(val_check);

    return NULL;
}

char *test_expand_contract()
{
    int old_max = array->max;
    DArray_expand(array);
    mu_assert((unsigned int) array->max == old_max + array->expand_rate,
              "Wrong size after expand.");

    // Check if newly allocated space is all set to 0.
    int i = 0;
    for (i = old_max; i < array->max; i++) {
        mu_assert(array->contents[i] == 0, "contents must be 0");
    }

    int rc = 0;
    rc = DArray_contract(array);
    mu_assert(rc == 0, "rc must be 0");
    mu_assert((unsigned int) array->max == array->expand_rate + 1,
              "Should stay at the expand_rate at least.");

    rc = DArray_contract(array);
    mu_assert(rc == 0, "rc must be 0");
    mu_assert((unsigned int) array->max == array->expand_rate + 1,
              "Should stay at the expand_rate at least");

    return NULL;
}

char *test_push_pop()
{
    int i = 0, rc = 0;
    for (i = 0; i < 1000; i++) {
        int *val = DArray_new(array);
        *val = i * 333;
        rc = DArray_push(array, val);
        mu_assert(rc == 0, "Darray_push failed");
    }

    mu_assert(array->max == 1201, "Wrong max size.");

    for (i = 999; i >= 0; i--) {
        int *val = DArray_pop(array);
        mu_assert(val != NULL, "Shouldn't get a NULL");
        mu_assert(*val == i * 333, "Wrong value.");

        if ((DArray_end(array) > (int) array->expand_rate) &&
            (DArray_end(array) % array->expand_rate)) {
            mu_assert(array->max == DArray_end(array) + 1,
                      "DArray pop contract error");
        }

        DArray_free(val);
    }

    return NULL;
}

int is_sorted(DArray *array)
{
    int i = 0;

    for (i = 0; i < DArray_count(array) -1; i++) {
        if (*((int *) DArray_get(array, i)) > *((int *) DArray_get(array, i + 1))) {
            return 0;
        }
    }

    return 1;
}

char *test_shallow_copy()
{
    // First populate array.
    array = DArray_create(sizeof(int), 100);
    int i = 0, rc = 0;
    int *val = NULL;
    for (i = 0; i < 1000; i++) {
        val = DArray_new(array);
        *val = i * 333;
        rc = DArray_push(array, val);
        mu_assert(rc == 0, "Darray_push failed");
    }
    mu_assert(array->max == 1300, "Wrong max size.");

    // Set at 1100
    val = DArray_new(array);
    *val = 42;
    rc = DArray_set(array, 1100, val);
    mu_assert(rc == 1100, "Error setting value at 1100");

    // Set at 1110
    val = DArray_new(array);
    *val = 4242;
    rc = DArray_set(array, 1110, val);
    mu_assert(rc == 1110, "Error setting value at 1110");

    // Test shallow copy
    DArray *copy = DArray_shallow_copy(array);
    mu_assert(copy != NULL, "Shallow copy failed");

    int expected = 0;
    for (i = 0; i < 1000; i++) {
        expected = i * 333;
        val = DArray_get(copy, i);
        mu_assert(*val == expected, "Unexpected element in copy.");
    }

    expected = 42;
    val = DArray_get(copy, 1100);
    mu_assert(*val == expected, "Unexpected element at in copy.");

    expected = 4242;
    val = DArray_get(copy, 1110);
    mu_assert(*val == expected, "Unexpected element at in copy.");

    // Destroy copy.
    DArray_destroy(copy);

    // Destroy array.
    DArray_clear_destroy(array);
    return NULL;
}

char *test_sort_add()
{
    array = DArray_create(sizeof(int), 100);
    mu_assert(array != NULL, "Error initializing array");

    int i = 0, rc = 0;
    int *val = NULL;
    for (i = 0; i < 2000; i++) {
        val = DArray_new(array);
        mu_assert(val != NULL, "Error creating new element");
        *val = rand();

        rc = DArray_sort_add(array, val, (DArray_compare) testcmp);
        mu_assert(rc == 0, "Error adding element.");
        mu_assert(is_sorted(array) == 1, "array not sorted.");
    }

    DArray_clear_destroy(array);
    return NULL;
}

char *test_find()
{
    array = DArray_create(sizeof(int), 100);
    mu_assert(array != NULL, "Error initializing array");

    // test setup.
    int i = 0, rc = 0;
    int *val = NULL;
    for (i = 0; i < 2000; i++) {
        val = DArray_new(array);
        mu_assert(val != NULL, "Error creating new element");
        *val = i;

        rc = DArray_sort_add(array, val, (DArray_compare) testcmp);
        mu_assert(rc == 0, "Error adding element");
        mu_assert(is_sorted(array) == 1, "array not sorted");
    }

    // tests.
    val = DArray_new(array);
    mu_assert(val != NULL, "Error creating new element");

    for (i = 0; i < 2000; i++) {
        *val = i;

        rc = DArray_find(array, val, (DArray_compare) testcmp);
        mu_assert(rc == i, "val not found in array");
    }

    *val = 3000;
    rc = DArray_find(array, val, (DArray_compare) testcmp);
    mu_assert(rc == -1, "rc must be -1");

    // test teardown.
    DArray_clear_destroy(array);
    free(val);
    return NULL;
}

char *all_tests()
{
    mu_suite_start();

    mu_run_test(test_create);
    mu_run_test(test_new);
    mu_run_test(test_set);
    mu_run_test(test_get);
    mu_run_test(test_remove);
    mu_run_test(test_expand_contract);
    mu_run_test(test_push_pop);
    mu_run_test(test_destroy);
    mu_run_test(test_shallow_copy);
    mu_run_test(test_sort_add);
    mu_run_test(test_find);

    return NULL;
}

RUN_TESTS(all_tests);
