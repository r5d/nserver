/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright © 2010, Zed A. Shaw.
 * Copyright © 2020 rsiddharth <s@ricketyspace.net>
 */

#undef NDEBUG
#include <stdint.h>
#include <darray_algos.h>
#include <hashmap.h>
#include <dbg.h>
#include <bstrlib.h>

static int default_compare(void *a, void *b)
{
    return bstrcmp((bstring) a, (bstring) b);
}

/**
 * Simple Bob Jenkin's hash algorithm taken from the wikipedia
 * description.
 */
static uint32_t default_hash(void *a)
{
    size_t len = blength((bstring) a);
    char *key = bdata((bstring) a);
    uint32_t hash = 0;
    uint32_t i = 0;

    for (hash = i = 0; i < len; ++i) {
        hash += key[i];
        hash += (hash << 10);
        hash ^= (hash >> 6);
    }

    hash += (hash << 3);
    hash ^= (hash >> 11);
    hash += (hash << 15);

    return hash;
}

/**
 * Fucked up version of FNV hash
 */
uint32_t fnv_hash(void *a)
{
    size_t len = blength((bstring) a);
    char *key = bdata((bstring) a);
    uint32_t hash =  0;
    uint32_t i =  0;
    uint32_t fnv_prime = 16777619;
    uint32_t fnv_offset_basis = 2166136261;

    hash = fnv_offset_basis;
    for (i = 0; i < len; ++i) {
        hash *= fnv_prime;
        hash ^= key[i];
    }

    return hash;
}

Hashmap *Hashmap_create(Hashmap_compare compare, Hashmap_hash hash)
{
    Hashmap *map = calloc(1, sizeof(Hashmap));
    check_mem(map);

    map->compare = compare == NULL ? default_compare : compare;
    map->hash = hash == NULL ? default_hash : hash;
    map->salt = rand() >> 16;
    map->buckets = DArray_create(
                                 sizeof(DArray *), DEFAULT_NUMBER_OF_BUCKETS);
    map->buckets->end = map->buckets->max; // fake out expanding it.
    check_mem(map->buckets);

    return map;

 error:
    if (map) {
        Hashmap_destroy(map);
    }

    return NULL;
}

void Hashmap_destroy(Hashmap *map)
{
    int i = 0;
    int j = 0;

    if (map) {
        if (map->buckets) {
            for (i = 0; i < DArray_count(map->buckets); i++) {
                DArray *bucket = DArray_get(map->buckets, i);
                if (bucket) {
                    for (j = 0; j < DArray_count(bucket); j++) {
                        free(DArray_get(bucket, j));
                    }
                    DArray_destroy(bucket);
                }
            }
            DArray_destroy(map->buckets);
        }

        free(map);
    }
}

static inline HashmapNode *Hashmap_node_create(int hash, void *key,
                                               void *data)
{
    HashmapNode *node = calloc(1, sizeof(HashmapNode));
    check_mem(node);

    node->key = key;
    node->data = data;
    node->hash = hash;

    return node;

 error:
    return NULL;
}

static inline DArray *Hashmap_find_bucket(Hashmap *map, void *key,
                                          int create,
                                          uint32_t *hash_out)
{
    uint32_t hash = map->hash(key) + map->salt;
    int bucket_n = hash % DEFAULT_NUMBER_OF_BUCKETS;
    check(bucket_n >= 0, "Invalid bucket found %d", bucket_n);
    // store it for the return so the caller can use it
    *hash_out = hash;

    DArray *bucket = DArray_get(map->buckets, bucket_n);

    if (!bucket && create) {
        // new bucket, set it up
        bucket = DArray_create(
                               sizeof(void *), DEFAULT_NUMBER_OF_BUCKETS);
        check_mem(bucket);
        DArray_set(map->buckets, bucket_n, bucket);
    }

    return bucket;

 error:
    return NULL;
}

int Hashmap_delete_bucket(Hashmap *map, uint32_t hash)
{
    int bucket_n = hash % DEFAULT_NUMBER_OF_BUCKETS;
    check(bucket_n >= 0, "Invalid bucket found %d", bucket_n);

    // Get bucket.
    DArray *bucket = DArray_get(map->buckets, bucket_n);
    if(bucket) {
        // Remove bucket.
        DArray_clear_destroy(bucket);
        DArray_remove(map->buckets, bucket_n);
    }

    return 0;
 error:
    return -1;
}

int Hashmap_set(Hashmap *map, void *key, void *data)
{
    uint32_t hash = 0;
    DArray *bucket = Hashmap_find_bucket(map, key, 1, &hash);
    check(bucket, "Error can't create bucket");

    HashmapNode *node = Hashmap_node_create(hash, key, data);
    check_mem(node);

    DArray_push(bucket, node);

    // Sort the bucket.
    int rc = DArray_heapsort(bucket, (DArray_compare) map->compare);
    check(rc == 0, "Error sorting bucket");

    return 0;

 error:
    return -1;
}

static inline int Hashmap_get_node(Hashmap *map, uint32_t hash,
                                   DArray *bucket, void *key)
{
    int i = 0;

    for (i = 0; i < DArray_end(bucket); i++) {
        debug("TRY: %d", i);
        HashmapNode *node = DArray_get(bucket, i);
        if (node->hash == hash && map->compare(node->key, key) == 0) {
            return i;
        }
    }

    return -1;
}

void *Hashmap_get(Hashmap *map, void *key)
{
    uint32_t hash = 0;
    DArray *bucket = Hashmap_find_bucket(map, key, 0, &hash);
    if (!bucket) return NULL;

    int i = Hashmap_get_node(map, hash, bucket, key);
    if (i == -1) return NULL;

    HashmapNode *node = DArray_get(bucket, i);
    check(node != NULL,
          "Failed to get node from bucket when it should exist.");

    return node->data;

 error:
    return NULL;
}

/**
 * Sets key <-> data iff key is not already present in Hashmap.
 */
int Hashmap_set_fucked(Hashmap *map, void *key, void *data)
{
    uint32_t hash = 0;
    DArray *bucket = Hashmap_find_bucket(map, key, 1, &hash);
    check(bucket, "Error can't create bucket");

    int i = Hashmap_get_node(map, hash, bucket, key);
    if (i >= 0) {
        // Element with `key` already exists. Do nothing.
        return 0;
    }

    // Element with `key` does not exist. Add it to Hashmap.
    HashmapNode *node = Hashmap_node_create(hash, key, data);
    check_mem(node);

    DArray_push(bucket, node);

    // Sort the bucket.
    int rc = DArray_heapsort(bucket, (DArray_compare) map->compare);
    check(rc == 0, "Error sorting bucket");

    return 1;
 error:
    return -1;
}

int Hashmap_traverse(Hashmap *map, Hashmap_traverse_cb traverse_cb)
{
    int i = 0;
    int j = 0;
    int rc = 0;

    for (i = 0; i < DArray_count(map->buckets); i++) {
        DArray *bucket = DArray_get(map->buckets, i);
        if (bucket) {
            for (j = 0; j < DArray_count(bucket); j++) {
                HashmapNode *node = DArray_get(bucket, j);
                rc = traverse_cb(node);
                if (rc != 0)
                    return rc;
            }
        }
    }

    return 0;
}

/**
 * Returns all keys of the hashmap as a DArray.
 *
 * Use DArray_destroy on the 'keys' returned by this function after
 * use.
 */
DArray *Hashmap_keys(Hashmap *map)
{
    check(map != NULL, "map is NULL");

    DArray *keys = DArray_create(sizeof(void *),
                                 DEFAULT_NUMBER_OF_KEYS);
    check(keys != NULL, "Unable to initialize keys");

    int i = 0;
    int j = 0;

    for (i = 0; i < DArray_count(map->buckets); i++) {
        DArray *bucket = DArray_get(map->buckets, i);
        if (bucket) {
            for (j = 0; j < DArray_count(bucket); j++) {
                HashmapNode *node = DArray_get(bucket, j);

                if (node) {
                    DArray_push(keys, node->key);
                }
            }
        }
    }

    return keys;
 error:
    return NULL;
}

void *Hashmap_delete(Hashmap *map, void *key)
{
    uint32_t hash = 0;
    DArray *bucket = Hashmap_find_bucket(map, key, 0, &hash);
    if (!bucket)
        return NULL;

    int i = Hashmap_get_node(map, hash, bucket, key);
    if (i == -1)
        return NULL;

    HashmapNode *node = DArray_get(bucket, i);
    void *data = node->data;
    free(node);

    HashmapNode *ending = DArray_pop(bucket);

    if (ending != node) {
        // alright looks like it's not the last one, swap it.
        DArray_set(bucket, i, ending);
    } else {
        // alright looks like it's the last one, destroy bucket.
        check(Hashmap_delete_bucket(map, hash) == 0,
              "Error destroy bucket");
    }

    return data;
 error:
    return NULL;
}
