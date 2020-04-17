/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright © 2010, Zed A. Shaw.
 * Copyright © 2020 rsiddharth <s@ricketyspace.net>
 */

#include <darray_algos.h>

int DArray_qsort(DArray *array, DArray_compare cmp)
{
    qsort(array->contents, DArray_count(array), sizeof(void *), cmp);
    return 0;
}

int DArray_heapsort(DArray *array, DArray_compare cmp)
{
    return heapsort(array->contents, DArray_count(array),
                    sizeof(void *), cmp);
}

int DArray_mergesort(DArray *array, DArray_compare cmp)
{
    return mergesort(array->contents, DArray_count(array),
                     sizeof(void *), cmp);
}

// Fucked Quick Sort.
int DArray_fucked_qsort_partition(DArray *array, DArray_compare cmp,
                                  int low, int high)
{
    int i = 0, j = 0, cmp_rc;
    void *pivot = NULL, *tmp = NULL;

    pivot = array->contents[high];
    i = low - 1;

    for (j = low; j < high; j++) {
        cmp_rc = cmp(&array->contents[j], &pivot);
        if (cmp_rc < 0) {
            i = i + 1;

            // swap
            tmp = array->contents[j];
            array->contents[j] = array->contents[i];
            array->contents[i] = tmp;
        }
    }

    cmp_rc = cmp(&array->contents[high], &array->contents[i + 1]);
    if (cmp_rc < 0) {
            tmp = array->contents[high];
            array->contents[high] = array->contents[i + 1];
            array->contents[i + 1] = tmp;
    }

    return i + 1;
}

int DArray_fucked_qsort_recurse(DArray *array, DArray_compare cmp,
                        int low, int high)
{
    int rc = 0, p = 0;
    if (low < high) {
        p = DArray_fucked_qsort_partition(array, cmp, low, high);
        check(p >= 0, "Failed to partition [%d-%d]", low, high);

        rc = DArray_fucked_qsort_recurse(array, cmp, low, p - 1);
        check(rc == 0, "Failed to quick sort sub array [%d-%d]", low, p-1);

        rc = DArray_fucked_qsort_recurse(array, cmp, p + 1, high);
        check(rc == 0, "Failed to quick sort sub array [%d-%d]", p+1, high);
    }

    return 0;
 error:
    return -1;
}

int DArray_fucked_qsort(DArray *array, DArray_compare cmp)
{
    int rc;
    rc = DArray_fucked_qsort_recurse(array, cmp, 0, DArray_end(array) - 1);
    check(rc == 0, "Error sorting array.");

    return 0;
 error:
    return -1;
}

// Fucked Heap Sort.
#define DArray_fucked_heapsort_iparent(i) ((floor(i - 1) / 2))
#define DArray_fucked_heapsort_ileft_child(i) ((2*i + 1))
#define DArray_fucked_heapsort_iright_child(i) ((2*i + 2))

static inline void DArray_fucked_heapsort_swap(DArray *array, int a, int b)
{
    void *tmp = array->contents[a];
    array->contents[a] = array->contents[b];
    array->contents[b] = tmp;
}

int DArray_fucked_heapsort_sift_down(DArray *array, DArray_compare cmp,
                              int start, int end)
{

    int root = start;
    int child = 0;
    int swap = 0;

    while (DArray_fucked_heapsort_ileft_child(root) <= end) {
        child = DArray_fucked_heapsort_ileft_child(root);
        swap = root;

        if (cmp(&array->contents[root], &array->contents[child]) < 0) {
            swap = child;
        }
        if (((child + 1) <= end)
            && (cmp(&array->contents[swap], &array->contents[child + 1]) < 0)) {
            swap = child + 1;
        }
        if (swap == root) {
            break;
        }

        DArray_fucked_heapsort_swap(array, root, swap);
        root = swap;
    }

    return 0;
}

int DArray_fucked_heapsort_heapify(DArray *array, DArray_compare cmp)
{
    int count = DArray_count(array);
    int start = (int) DArray_fucked_heapsort_iparent(count - 1);
    int rc = 0;


    while (start >= 0) {
        rc = DArray_fucked_heapsort_sift_down(array, cmp, start, count - 1);
        check(rc == 0, "Error sifting down at %d %d", start, count - 1);

        start = start - 1;
    }

    return 0;
 error:
    return -1;
}

int DArray_fucked_heapsort(DArray *array, DArray_compare cmp)
{
    int rc = 0, end = 0;

    // First heapify array.
    rc = DArray_fucked_heapsort_heapify(array, cmp);
    check(rc == 0, "Error heapifying");

    end = DArray_count(array) - 1;
    while (end > 0) {
        DArray_fucked_heapsort_swap(array, end, 0);

        end = end - 1;
        rc = DArray_fucked_heapsort_sift_down(array, cmp, 0, end);
        check(rc == 0, "Error sifting down at %d %d", 0, end);
    }

    return 0;
 error:
    return -1;
}

// Fucked Merge Sort.
void DArray_fucked_topdown_merge(DArray *a, DArray *b,
                                int begin, int middle,
                                int end, DArray_compare cmp)
{
    int i = 0, j = 0, k = 0;

    i = begin;
    j = middle;

    for (k = begin; k < end; k++) {
        if (i < middle &&
            (j >= end || cmp(&a->contents[i], &a->contents[j]) < 0)) {
            DArray_set(b, k, DArray_get(a, i));
            i = i + 1;
        } else {
            DArray_set(b, k, DArray_get(a, j));
            j = j + 1;
        }
    }

    return;
}

void DArray_fucked_topdown_split_merge(DArray *b, DArray *a,
                                      int begin, int end,
                                      DArray_compare cmp)
{
    if ((end - begin) < 2) {
        return;
    }
    int middle = (end + begin) / 2;

    DArray_fucked_topdown_split_merge(a, b, begin, middle, cmp);
    DArray_fucked_topdown_split_merge(a, b, middle, end, cmp);

    DArray_fucked_topdown_merge(b, a, begin, middle, end, cmp);

    return;
}

int DArray_fucked_mergesort(DArray *array, DArray_compare cmp)
{
    DArray *copy = DArray_shallow_copy(array);
    check(copy != NULL, "Error shallow copying array");

    DArray *a = NULL, *b = NULL;
    int begin = 0, end = 0;

    a = array;
    b = copy;
    begin  = 0;
    end = DArray_end(a);
    DArray_fucked_topdown_split_merge(b, a, begin, end, cmp);

    // Clean up copy
    DArray_destroy(copy);

    return 0;
 error:
    return -1;
}
