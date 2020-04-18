/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright © 2010, Zed A. Shaw.
 * Copyright © 2020 rsiddharth <s@ricketyspace.net>
 */

#include "minunit.h"
#include <hashmap.h>
#include <assert.h>
#include <bstrlib.h>

Hashmap *map = NULL;
Hashmap *map_alt = NULL;
static int traverse_called = 0;
static int traverse_duplicates = 0;
struct tagbstring test0 = bsStatic("test data 0");
struct tagbstring test1 = bsStatic("test data 1");
struct tagbstring test2 = bsStatic("test data 2");
struct tagbstring test3 = bsStatic("xest data 3");
struct tagbstring expect0 = bsStatic("THE VALUE 0");
struct tagbstring expect1 = bsStatic("THE VALUE 1");
struct tagbstring expect2 = bsStatic("THE VALUE 2");
struct tagbstring expect3 = bsStatic("THE VALUE 3");

static int traverse_good_cb(HashmapNode *node)
{
    debug("KEY: %s", bdata((bstring) node->key));
    traverse_called++;
    return 0;
}

static int traverse_fail_cb(HashmapNode *node)
{
    debug("KEY %s", bdata((bstring) node->key));
    traverse_called++;

    if (traverse_called == 2) {
        return 1;
    } else {
        return 0;
    }
}

static int traverse_duplicates_cb(HashmapNode *node)
{
    if ((bstring) node->data == &expect1) {
        ++traverse_duplicates;
    }

    return 0;
}

int non_empty_buckets(Hashmap *map)
{
    int i = 0, non_empty = 0;

    for (i = 0; i < DArray_count(map->buckets); i++) {
        DArray *bucket = DArray_get(map->buckets, i);
        if (bucket) {
            non_empty += 1;
        }
    }

    return non_empty;
}

char *test_create()
{
    map = Hashmap_create(NULL, NULL);
    mu_assert(map != NULL, "Failed to create map.");

    map_alt = Hashmap_create(NULL, (Hashmap_hash) fnv_hash);
    mu_assert(map != NULL, "Failde to create map_alt.");

    return NULL;
}

char *test_destroy()
{
    Hashmap_destroy(map);
    Hashmap_destroy(map_alt);
    return NULL;
}

char *test_get_test()
{
    int rc = Hashmap_set(map, &test1, &expect1);
    mu_assert(rc == 0, "Failed to set &test1");
    bstring result = Hashmap_get(map, &test1);
    mu_assert(result == &expect1, "Wron value for test1.");

    rc = Hashmap_set(map, &test2, &expect2);
    mu_assert(rc == 0, "Failed to test test2");
    result = Hashmap_get(map, &test2);
    mu_assert(result == &expect2, "Wrong value for test2.");

    rc = Hashmap_set(map, &test3, &expect3);
    mu_assert(rc == 0, "Failed to set test3");
    result = Hashmap_get(map, &test3);
    mu_assert(result == &expect3, "Wrong value for test3");

    return NULL;
}

char *test_traverse()
{
    int rc = Hashmap_traverse(map, traverse_good_cb);
    mu_assert(rc == 0, "Failed to traverse");
    mu_assert(traverse_called == 3, "Wrong count traverse");

    traverse_called = 0;
    rc = Hashmap_traverse(map, traverse_fail_cb);
    mu_assert(rc == 1, "Failed to traverse.");
    mu_assert(traverse_called == 2, "Wrong count traverse for fail.");

    return NULL;
}

char *test_keys()
{
    DArray *keys = Hashmap_keys(map);
    mu_assert(keys != NULL, "Unable to get hashmap's keys");
    mu_assert(DArray_count(keys) == 3, "Expected three keys");

    for (int i = 0; i < DArray_count(keys); i++) {
        char *k = bdata((bstring) DArray_get(keys, i));

        debug("KEY: %s", k);
    }

    // clean up.
    DArray_destroy(keys);

    return NULL;
}

char *test_delete()
{
    bstring deleted = (bstring) Hashmap_delete(map, &test1);
    mu_assert(deleted != NULL, "Got NULL on delete");
    mu_assert(deleted == &expect1, "Should get test1");
    bstring result = Hashmap_get(map, &test1);
    mu_assert(result == NULL, "Should delete");

    deleted = (bstring) Hashmap_delete(map, &test2);
    mu_assert(deleted != NULL, "Got NULL on delete");
    mu_assert(deleted == &expect2, "Should get test2");
    result = Hashmap_get(map, &test2);
    mu_assert(result == NULL, "Should delete");

    deleted = (bstring) Hashmap_delete(map, &test3);
    mu_assert(deleted != NULL, "Got NULL on delete.");
    mu_assert(deleted == &expect3, "Should get test3");
    result = Hashmap_get(map, &test3);
    mu_assert(result == NULL, "Should delete.");

    return NULL;
}

char *test_bucket_destruction()
{
    int rc = 0, non_empty = 0;
    bstring deleted  = NULL;

    // Insert some elements.
    rc = Hashmap_set(map, &test1, &expect1);
    mu_assert(rc == 0, "Failed to set &test1 0");
    rc = Hashmap_set(map, &test2, &expect2);
    mu_assert(rc == 0, "Failed to test test2");

    // Non-empty buckets test.
    non_empty = non_empty_buckets(map);
    mu_assert(non_empty == 2, "Expected two non-empty buckets");

    // Remove test1
    deleted = Hashmap_delete(map, &test1);
    mu_assert(deleted != NULL, "Error deleting test1");

    // Non-empty buckets test.
    non_empty = non_empty_buckets(map);
    mu_assert(non_empty == 1, "Expected one non-empty buckets");

    // Remove test2
    deleted = Hashmap_delete(map, &test2);
    mu_assert(deleted != NULL, "Error deleting test2");

    // Non-empty buckets test.
    non_empty = non_empty_buckets(map);
    mu_assert(non_empty == 0, "Expected one non-empty buckets");

    return NULL;
}

char *test_set_duplicates()
{
    int rc = 0;

    // Insert test1 two times.
    rc = Hashmap_set(map, &test1, &expect1);
    mu_assert(rc == 0, "Failed to set &test1 0");
    rc = Hashmap_set(map, &test1, &expect1);
    mu_assert(rc == 0, "Failed to set &test1 1");

    // Insert test2 one time.
    rc = Hashmap_set(map, &test2, &expect2);
    mu_assert(rc == 0, "Failed to test test2");

    // Insert test3 one time.
    rc = Hashmap_set(map, &test3, &expect3);
    mu_assert(rc == 0, "Failed to set test3");

    // Test there are two test1 nodes.
    traverse_duplicates = 0;
    rc = Hashmap_traverse(map, traverse_duplicates_cb);
    mu_assert(traverse_duplicates == 2,
              "traverse_duplicates must be 2");

    // Cleanup
    bstring deleted  = NULL;
    deleted = Hashmap_delete(map, &test1);
    mu_assert(deleted != NULL, "Error deleting test1 0");
    deleted  = Hashmap_delete(map, &test1);
    mu_assert(deleted != NULL, "Error deleting test1 1");
    deleted  = Hashmap_delete(map, &test2);
    mu_assert(deleted != NULL, "Error deleting test2");
    deleted  = Hashmap_delete(map, &test3);
    mu_assert(deleted != NULL, "Error deleting test3");

    return NULL;
}

char *test_set_fucked()
{
    int rc = 0;

    // Insert test1 three times.
    rc = Hashmap_set_fucked(map, &test1, &expect1);
    mu_assert(rc == 1, "Failed to set fuck &test1 0");
    rc = Hashmap_set_fucked(map, &test1, &expect1);
    mu_assert(rc == 0, "Failed to set fuck &test1 1");
    rc = Hashmap_set_fucked(map, &test1, &expect1);
    mu_assert(rc == 0, "Failed to set fuck &test1 2");

    // Insert test2 one time.
    rc = Hashmap_set_fucked(map, &test2, &expect2);
    mu_assert(rc == 1, "Failed to test test2");

    // Insert test3 one time.
    rc = Hashmap_set_fucked(map, &test3, &expect3);
    mu_assert(rc == 1, "Failed to set test3");

    // Test there are two test1 nodes.
    traverse_duplicates = 0;
    rc = Hashmap_traverse(map, traverse_duplicates_cb);
    mu_assert(traverse_duplicates == 1,
              "traverse_duplicates must be 1");

    // Cleanup
    bstring deleted  = NULL;
    deleted = Hashmap_delete(map, &test1);
    mu_assert(deleted != NULL, "Error deleting test1");
    deleted  = Hashmap_delete(map, &test1);
    mu_assert(deleted == NULL, "Error test1 must be already deleted");
    deleted  = Hashmap_delete(map, &test2);
    mu_assert(deleted != NULL, "Error deleting test2");
    deleted  = Hashmap_delete(map, &test3);
    mu_assert(deleted != NULL, "Error deleting test3");

    return NULL;
}

char *test_fnv_hash()
{
    uint32_t hash[3];

    hash[0] = fnv_hash(&test0);
    mu_assert(hash[0] > 0, "hash not set correctly for key test0");

    hash[1] = fnv_hash(&test1);
    mu_assert(hash[1] > 0, "hash not set correctly for key test1");

    hash[2] = fnv_hash(&test2);
    mu_assert(hash[2] > 0, "hash not set correctly for key test2");

    // Check all three hashes are different.
    mu_assert(hash[0] != hash[1], "hash for test0 = hash for test1");
    mu_assert(hash[0] != hash[2], "hash for test0 = hash for test2");
    mu_assert(hash[1] != hash[2], "hash for test1 = hash for test2");

    return NULL;
}

char *all_tests()
{
    mu_suite_start();

    mu_run_test(test_create);
    mu_run_test(test_get_test);
    mu_run_test(test_traverse);
    mu_run_test(test_keys);
    mu_run_test(test_delete);
    mu_run_test(test_bucket_destruction);
    mu_run_test(test_set_duplicates);
    mu_run_test(test_set_fucked);
    mu_run_test(test_fnv_hash);
    mu_run_test(test_destroy);

    return NULL;
}

RUN_TESTS(all_tests);
