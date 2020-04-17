/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright © 2020 rsiddharth <s@ricketyspace.net>
 */

#include "minunit.h"
#include <db.h>

char *test_db_init()
{
    int rc = db_init();
    mu_assert(rc == 0, "db init failed");

    return NULL;
}

char *test_db_store()
{
    char *k = "hello";
    char *v  = "kirk";

    int rc = db_store(k, v);
    mu_assert(rc == 0, "db store failed");

    return NULL;
}

char *test_db_load()
{
    char *k = "hello";
    char *expected_v = "kirk";

    char *v = db_load(k);
    mu_assert(v != NULL, "key not found");
    mu_assert(strncmp(v, expected_v, strlen(expected_v)) == 0,
              "incorrect value for key");

    // cleanup.
    free(v);

    return NULL;
}

char *all_tests()
{
    mu_suite_start();

    mu_run_test(test_db_init);
    mu_run_test(test_db_store);
    mu_run_test(test_db_load);

    return NULL;
}

RUN_TESTS(all_tests);
