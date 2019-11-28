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

char *test_check_cmd()
{
    char *cmd = calloc(128, sizeof(char));
    mu_assert(cmd != NULL, "calloc failed");

    char *bacon = "/create api/bacon";
    strncpy(cmd, bacon, strlen(bacon));
    mu_assert(strlen(cmd) == strlen(bacon), "strncpy failed");

    char *err = check_cmd(cmd);
    mu_assert(err == NULL, "check_cmd failed");


    memset(cmd, '\0', 128);


    char *c = "/c";
    strncpy(cmd, c, strlen(c));
    mu_assert(strlen(cmd) == strlen(c), "strncpy failed");

    err = check_cmd(cmd);
    mu_assert(err != NULL, "check_cmd failed");
    mu_assert(strcmp(err, "command size invalid\n") == 0,
              "wrong err msg");


    memset(cmd, '\0', 128);


    char *empty = "\n";
    strncpy(cmd, empty, strlen(empty));
    mu_assert(strlen(cmd) == strlen(empty), "strncpy failed");

    err = check_cmd(cmd);
    mu_assert(err != NULL, "check_cmd failed");
    printf("ERR %s", err);
    mu_assert(strcmp(err, "closing connection\n") == 0,
              "wrong err msg");


    err = check_cmd(NULL);
    mu_assert(err != NULL, "check_cmd failed");
    mu_assert(strcmp(err, "internal error\n") == 0,
              "wrong err msg");


    // Cleanup.
    free(cmd);

    return NULL;
}

char *test_cmd_parts()
{
    struct bstrList *parts = NULL;

    bstring create = bfromcstr("/create");

    char *bacon = "/create bacon";
    parts = cmd_parts(bacon);
    mu_assert(parts != NULL, "cmd parts failed");
    mu_assert(parts->qty == 2, "qty check failed");
    mu_assert(bstricmp(parts->entry[0], create) == 0,
              "equality check failed");

    // Cleanup
    bstrListDestroy(parts);

    char *ham = "/create           ham";
    parts = cmd_parts(ham);
    mu_assert(parts != NULL, "cmd parts failed");
    mu_assert(parts->qty == 2, "qty check failed");
    mu_assert(bstricmp(parts->entry[0], create) == 0,
              "equality check failed");


    // Cleanup
    bdestroy(create);
    bstrListDestroy(parts);

    return NULL;
}

char *all_tests()
{
    mu_suite_start();

    mu_run_test(test_sanitize);
    mu_run_test(test_check_cmd);
    mu_run_test(test_cmd_parts);

    return NULL;
}

RUN_TESTS(all_tests);
