#ifndef darray_algos_h
#define darray_algos_h

#include <darray.h>

int DArray_qsort(DArray *array, DArray_compare cmp);

int DArray_heapsort(DArray *array, DArray_compare cmp);

int DArray_mergesort(DArray *array, DArray_compare cmp);

int DArray_fucked_qsort(DArray *array, DArray_compare cmp);

int DArray_fucked_heapsort(DArray *array, DArray_compare cmp);

int DArray_fucked_mergesort(DArray *array, DArray_compare cmp);

#endif
