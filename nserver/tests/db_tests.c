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

char *all_tests()
{
    mu_suite_start();

    mu_run_test(test_db_init);
    mu_run_test(test_db_store);

    return NULL;
}

RUN_TESTS(all_tests);
