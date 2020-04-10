#include "minunit.h"
#include <db.h>

char *test_db_init()
{
    int rc = db_init();
    mu_assert(rc == 0, "db init failed");

    return NULL;
}

char *all_tests()
{
    mu_suite_start();

    mu_run_test(test_db_init);

    return NULL;
}

RUN_TESTS(all_tests);
