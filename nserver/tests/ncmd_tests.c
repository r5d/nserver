#include "minunit.h"
#include <ncmd.h>

char *test_sanitize()
{
    char *cmd = calloc(128, sizeof(char));
    mu_assert(cmd != NULL, "calloc failed");

    char *c_cmd = "/create api/beef\n";
    size_t c_cmd_sz = strlen(c_cmd);

    strncpy(cmd, c_cmd, c_cmd_sz + 1);
    mu_assert(strlen(cmd) == c_cmd_sz, "strncpy failed");

    sanitize(cmd);
    mu_assert(strlen(cmd) == c_cmd_sz - 1, "sanitize failed");
    mu_assert(strcmp(cmd, "/create api/beef") == 0, "sanitize failed");


    // Clear cmd.
    memset(cmd, '\0', 128);


    c_cmd = "/create api/ham\n\n\n";
    c_cmd_sz = strlen(c_cmd);

    strncpy(cmd, c_cmd, c_cmd_sz + 1);
    mu_assert(strlen(cmd) == c_cmd_sz, "strncpy failed");

    sanitize(cmd);
    mu_assert(strlen(cmd) == c_cmd_sz - 3, "sanitize failed");
    mu_assert(strcmp(cmd, "/create api/ham") == 0, "sanitize failed");


    // Clear cmd.
    memset(cmd, '\0', 128);


    c_cmd = "/create api/bacon\n/create api/turkey\n";
    c_cmd_sz = strlen(c_cmd);

    strncpy(cmd, c_cmd, c_cmd_sz + 1);
    mu_assert(strlen(cmd) == c_cmd_sz, "strncpy failed");

    sanitize(cmd);
    mu_assert(strlen(cmd) == c_cmd_sz - 20, "sanitize failed");
    mu_assert(strcmp(cmd, "/create api/bacon") == 0, "sanitize failed");


    // Cleanup.
    free(cmd);

    return NULL;
}

char *all_tests()
{
    mu_suite_start();

    mu_run_test(test_sanitize);

    return NULL;
}

RUN_TESTS(all_tests);
