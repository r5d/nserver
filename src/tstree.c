/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright © 2010, Zed A. Shaw.
 * Copyright © 2020 rsiddharth <s@ricketyspace.net>
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <dbg.h>
#include <tstree.h>

static inline TSTree *TSTree_insert_base(TSTree *root, TSTree *node,
                                         const char *key, size_t len,
                                         void *value)
{
    if (node == NULL) {
        node = (TSTree *) calloc(1, sizeof(TSTree));

        if (root == NULL) {
            root = node;
        }

        node->splitchar = *key;
    }

    if (*key < node->splitchar) {
        node->low = TSTree_insert_base(root,
                                       node->low, key, len, value);
    } else if (*key == node->splitchar) {
        if (len > 1) {
            node->equal = TSTree_insert_base(root, node->equal,
                                             key + 1, len -1 , value);
        } else {
            assert(node->value == NULL && "Duplicate insert into tst.");
            node->value = value;
        }
    } else {
        node->high = TSTree_insert_base(root, node->high,
                                        key, len, value);
    }

    return node;
}

TSTree *TSTree_insert(TSTree *node, const char *key, size_t len,
                      void *value)
{
    return TSTree_insert_base(node, node, key, len, value);
}

void *TSTree_search(TSTree *root, const char *key, size_t len)
{
    TSTree *node = root;
    size_t i = 0;

    while (i < len && node) {
        if (key[i] < node->splitchar) {
            node = node->low;
        } else if (key[i] == node->splitchar) {
            i++;
            if (i < len)
                node = node->equal;
        } else {
            node = node->high;
        }
    }

    if (node) {
        return node->value;
    } else {
        return NULL;
    }
}

void *TSTree_search_prefix(TSTree *root, const char *key, size_t len)
{
    if (len == 0)
        return NULL;

    TSTree *node = root;
    TSTree *last = NULL;
    size_t i = 0;

    while (i < len && node) {
        if (key[i] < node->splitchar) {
            node = node->low;
        } else if (key[i] == node->splitchar) {
            i++;
            if (i < len) {
                if (node->value) {
                    last = node->value;
                }
                node = node->equal;
            }
        } else {
            node = node->high;
        }
    }

    return last;
}

void TSTree_collect_keys(TSTree *node, char *key, size_t key_sz, DArray *array)
{
    if (!node) {
        return;
    }

    strcat(key, &node->splitchar);

    key_sz += 2;
    key = (char *) realloc(key, key_sz);

    if (node->value) {
        char *key_cpy = (char *) calloc(key_sz, sizeof(char));
        strcpy(key_cpy, key);
        DArray_push(array, key_cpy);
    }

    if (node->low) {
        TSTree_collect_keys(node->low, key, key_sz, array);
    }

    if (node->equal) {
        TSTree_collect_keys(node->equal, key, key_sz, array);
    }

    if (node->high) {
        TSTree_collect_keys(node->high, key, key_sz, array);
    }
}

size_t TSTree_collect_keycat(char *key, size_t key_sz, char *c) {
    // Expand key.
    key_sz += 2;
    key = (char *) realloc(key, key_sz);
    check(key != NULL, "Unable to expand key");

    // Concat.
    key = strcat(key, c);
    check(key != NULL, "key cat failed");

    return key_sz;
 error:
    if (key) {
        free(key);
    }
    return 0;
}

DArray *TSTree_collect(TSTree *root, const char *key, size_t len)
{
    char *ckey = NULL;

    DArray *array = DArray_create(sizeof(void), 1);
    check(array != NULL, "Unable to initialize DArray");

    if (len == 0)
        return array;

    TSTree *node = root;
    TSTree *last = NULL;
    size_t i = 0;

    size_t ckey_sz = 4;
    ckey = (char *) calloc(ckey_sz, sizeof(char));
    check(ckey != NULL, "Unable to initialize ckey");
    ckey[0] = '\0';

    while (i < len && node) {
        if (key[i] < node->splitchar) {
            node = node->low;
        } else if (key[i] == node->splitchar) {
            i++;
            if (i < len) {
                ckey_sz = TSTree_collect_keycat(ckey, ckey_sz,
                                                &node->splitchar);
                check(ckey_sz > 0, "keycat failed");

                if (node->value) {
                    last = node;
                }
                node = node->equal;
            }
        } else {
            node = node->high;
        }
    }
    node = node ? node : last;

    while (node && !node->value) {
        ckey_sz = TSTree_collect_keycat(ckey, ckey_sz,
                                        &node->splitchar);
        check(ckey_sz > 0, "keycat failed");

        node = node->equal;
    }

    if (node) {
        TSTree_collect_keys(node, ckey, ckey_sz, array);
    }

    // Clean up.
    free(ckey);

    return array;

 error:
    // Clean up.
    if (array) {
        DArray_destroy(array);
    }
    if (ckey) {
        free(ckey);
    }
    return NULL;
}

void TSTree_traverse(TSTree *node, TSTree_traverse_cb cb, void *data)
{
    if (!node)
        return;

    if (node->low)
        TSTree_traverse(node->low, cb, data);

    if (node->equal) {
        TSTree_traverse(node->equal, cb, data);
    }

    if (node->high)
        TSTree_traverse(node->high, cb, data);

    if (node->value)
        cb(node->value, data);
}

void TSTree_destroy(TSTree *node)
{
    if (node == NULL)
        return;

    if (node->low)
        TSTree_destroy(node->low);

    if (node->equal) {
        TSTree_destroy(node->equal);
    }

    if (node->high)
        TSTree_destroy(node->high);

    free(node);
}
