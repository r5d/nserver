/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright © 2020 rsiddharth <s@ricketyspace.net>
 */

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
    char *err = (char *) calloc(RSP_SIZE, sizeof(char));

    char *cmd = (char *) calloc(128, sizeof(char));
    mu_assert(cmd != NULL, "calloc failed");

    char *bacon = "/create api/bacon";
    strncpy(cmd, bacon, strlen(bacon));
    mu_assert(strlen(cmd) == strlen(bacon), "strncpy failed");

    int rc = check_cmd(cmd, err);
    mu_assert(rc == 0, "check_cmd failed");

    memset(err, '\0', RSP_SIZE);
    memset(cmd, '\0', 128);

    char *c = "/c";
    strncpy(cmd, c, strlen(c));
    mu_assert(strlen(cmd) == strlen(c), "strncpy failed");

    rc = check_cmd(cmd, err);
    mu_assert(rc < 0, "check_cmd failed");
    mu_assert(strcmp(err, "invalid command\n") == 0,
              "wrong err msg");

    memset(err, '\0', RSP_SIZE);
    memset(cmd, '\0', 128);


    char *empty = "\n";
    strncpy(cmd, empty, strlen(empty));
    mu_assert(strlen(cmd) == strlen(empty), "strncpy failed");

    rc = check_cmd(cmd, err);
    mu_assert(rc < 0, "check_cmd failed");
    mu_assert(strcmp(err, "closing connection\n") == 0,
              "wrong err msg");


    rc = check_cmd(NULL, err);
    mu_assert(rc < 0, "check_cmd failed");
    mu_assert(strcmp(err, "internal error\n") == 0,
              "wrong err msg");


    // Cleanup.
    free(err);
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

char *test_find_function()
{
    struct bstrList *parts = NULL;
    int funk = 0;

    char *bacon = "/create bacon";
    parts = cmd_parts(bacon);
    mu_assert(parts != NULL, "cmd_parts failed");
    funk = find_function(parts);
    mu_assert(funk == NS_CREATE, "find function create failed");

    char *ham = "/create        ham";
    parts = cmd_parts(ham);
    mu_assert(parts != NULL, "cmd_parts failed");
    funk = find_function(parts);
    mu_assert(funk == NS_CREATE, "find function create failed");

    ham = "/CREate        ham";
    parts = cmd_parts(ham);
    mu_assert(parts != NULL, "cmd_parts failed");
    funk = find_function(parts);
    mu_assert(funk == NS_CREATE, "find function create failed");

    char *sample = "/sample bacon 42";
    parts = cmd_parts(sample);
    mu_assert(parts != NULL, "cmd_parts failed");
    funk = find_function(parts);
    mu_assert(funk == NS_SAMPLE, "find function sample failed");

    sample = "/SAMPLE bacon 42";
    parts = cmd_parts(sample);
    mu_assert(parts != NULL, "cmd_parts failed");
    funk = find_function(parts);
    mu_assert(funk == NS_SAMPLE, "find function sample failed");

    char *mean = "/mean bacon";
    parts = cmd_parts(mean);
    mu_assert(parts != NULL, "cmd_parts failed");
    funk = find_function(parts);
    mu_assert(funk == NS_MEAN, "find function mean failed");

    char *dump = "/dump bacon";
    parts = cmd_parts(dump);
    mu_assert(parts != NULL, "cmd_parts failed");
    funk = find_function(parts);
    mu_assert(funk == NS_DUMP, "find function dump failed");

    char *delete = "/delete bacon";
    parts = cmd_parts(delete);
    mu_assert(parts != NULL, "cmd_parts failed");
    funk = find_function(parts);
    mu_assert(funk == NS_DELETE, "find function delete failed");

    char *list = "/list";
    parts = cmd_parts(list);
    mu_assert(parts != NULL, "cmp_parts failed");
    funk = find_function(parts);
    mu_assert(funk == NS_LIST, "find function list failed");

    list = "/LIST";
    parts = cmd_parts(list);
    mu_assert(parts != NULL, "cmp_parts failed");
    funk = find_function(parts);
    mu_assert(funk == NS_LIST, "find function list failed");

    char *store = "/store jowl";
    parts = cmd_parts(store);
    mu_assert(parts != NULL, "cmp_parts failed");
    funk = find_function(parts);
    mu_assert(funk == NS_STORE, "find function list failed");

    store = "/STore    jowl";
    parts = cmd_parts(store);
    mu_assert(parts != NULL, "cmp_parts failed");
    funk = find_function(parts);
    mu_assert(funk == NS_STORE, "find function list failed");

    char *load = "/load jowl ears";
    parts = cmd_parts(load);
    mu_assert(parts != NULL, "cmp_parts failed");
    funk = find_function(parts);
    mu_assert(funk == NS_LOAD, "find function list failed");

    load = "/LoAD             jowl ears";
    parts = cmd_parts(load);
    mu_assert(parts != NULL, "cmp_parts failed");
    funk = find_function(parts);
    mu_assert(funk == NS_LOAD, "find function list failed");

    char *nop = "/meant bacon";
    parts = cmd_parts(nop);
    mu_assert(parts != NULL, "cmd_parts failed");
    funk = find_function(parts);
    mu_assert(funk == NS_NOP, "find function mean failed");

    return NULL;
}

char *test_call_function()
{
    struct bstrList *parts = NULL;

    char *msg = (char *) calloc(RSP_SIZE + 1, sizeof(char));
    mu_assert(msg != NULL, "msg invalid");

    char *bacon = "/create bacon";
    parts = cmd_parts(bacon);
    mu_assert(parts != NULL, "cmd_parts failed");
    int rc = call_function(NS_CREATE, parts, msg);
    mu_assert(rc == 0, "call function failed");
    mu_assert(strcmp(msg, "OK\n") == 0, "call function failed");

    char *ham = "/create ham";
    parts = cmd_parts(ham);
    mu_assert(parts != NULL, "cmd_parts failed");
    rc = call_function(NS_CREATE, parts, msg);
    mu_assert(rc == 0, "call function failed");
    mu_assert(strcmp(msg, "OK\n") == 0, "call function failed");

    char *beef = "/create beef";
    parts = cmd_parts(beef);
    mu_assert(parts != NULL, "cmd_parts failed");
    rc = call_function(NS_CREATE, parts, msg);
    mu_assert(rc == 0, "call function failed");
    mu_assert(strcmp(msg, "OK\n") == 0, "call function failed");

    char *bacon_sample = "/sample bacon 4.2";
    parts = cmd_parts(bacon_sample);
    mu_assert(parts != NULL, "cmd_parts failed");
    rc = call_function(NS_SAMPLE, parts, msg);
    mu_assert(rc == 0, "call function failed");
    mu_assert(strcmp(msg, "Mean: 4.20\n") == 0, "call function failed");

    bacon_sample = "/Sample bacon 6.9";
    parts = cmd_parts(bacon_sample);
    mu_assert(parts != NULL, "cmd_parts failed");
    rc = call_function(NS_SAMPLE, parts, msg);
    mu_assert(rc == 0, "call function failed");
    mu_assert(strcmp(msg, "Mean: 5.55\n") == 0, "call function failed");

    char *bacon_mean = "/mean bacon";
    parts = cmd_parts(bacon_mean);
    mu_assert(parts != NULL, "cmd_parts failed");
    rc = call_function(NS_MEAN, parts, msg);
    mu_assert(rc == 0, "call function failed");
    mu_assert(strcmp(msg, "Mean: 5.55\n") == 0, "call function failed");

    char *bacon_dump = "/dump bacon";
    parts = cmd_parts(bacon_dump);
    mu_assert(parts != NULL, "cmd_parts failed");
    rc = call_function(NS_DUMP, parts, msg);
    mu_assert(rc == 0, "call function failed");
    mu_assert(strcmp(msg, "sum: 11.100000, sumsq: 65.250000, n: 2, min: 4.200000, max: 6.900000, mean: 5.550000, stddev: 1.909188\n") == 0, "call function failed");

    char *bacon_delete = "/delete bacon";
    parts = cmd_parts(bacon_delete);
    mu_assert(parts != NULL, "cmd_parts failed");
    rc = call_function(NS_DELETE, parts, msg);
    mu_assert(rc == 0, "call function failed");
    mu_assert(strcmp(msg, "OK\n") == 0, "call function failed");

    char *list = "/list";
    parts = cmd_parts(list);
    mu_assert(parts != NULL, "cmd_parts failed");
    rc = call_function(NS_LIST, parts, msg);
    mu_assert(rc == 0, "call function failed");
    mu_assert(strcmp(msg, "ham\nbeef\n") == 0
              || strcmp(msg, "beef\nham\n") == 0,
              "call function failed");


    char *ham_store = "/store ham";
    parts = cmd_parts(ham_store);
    mu_assert(parts != NULL, "cmd_parts failed");
    rc = call_function(NS_STORE, parts, msg);
    mu_assert(rc == 0, "call function failed");
    mu_assert(strcmp(msg, "OK\n") == 0, "call function failed");


    char *beef_store = "/store beef";
    parts = cmd_parts(beef_store);
    mu_assert(parts != NULL, "cmd_parts failed");
    rc = call_function(NS_STORE, parts, msg);
    mu_assert(rc == 0, "call function failed");
    mu_assert(strcmp(msg, "OK\n") == 0, "call function failed");

    char *roastedham_load = "/load ham roastedham";
    parts = cmd_parts(roastedham_load);
    mu_assert(parts != NULL, "cmd_parts failed");
    rc = call_function(NS_LOAD, parts, msg);
    mu_assert(rc == 0, "call function failed");
    mu_assert(strcmp(msg, "OK\n") == 0, "call function failed");

    char *grilledbeef_load = "/load beef grilledbeef";
    parts = cmd_parts(grilledbeef_load);
    mu_assert(parts != NULL, "cmd_parts failed");
    rc = call_function(NS_LOAD, parts, msg);
    mu_assert(rc == 0, "call function failed");
    mu_assert(strcmp(msg, "OK\n") == 0, "call function failed");

    // delete ham, roastedham, beef, and roastedbeef.
    char *ham_delete = "/delete ham";
    parts = cmd_parts(ham_delete);
    mu_assert(parts != NULL, "cmd_parts failed");
    rc = call_function(NS_DELETE, parts, msg);
    mu_assert(rc == 0, "call function failed");
    mu_assert(strcmp(msg, "OK\n") == 0, "call function failed");

    char *roastedham_delete = "/delete roastedham";
    parts = cmd_parts(roastedham_delete);
    mu_assert(parts != NULL, "cmd_parts failed");
    rc = call_function(NS_DELETE, parts, msg);
    mu_assert(rc == 0, "call function failed");
    mu_assert(strcmp(msg, "OK\n") == 0, "call function failed");

    char *beef_delete = "/delete beef";
    parts = cmd_parts(beef_delete);
    mu_assert(parts != NULL, "cmd_parts failed");
    rc = call_function(NS_DELETE, parts, msg);
    mu_assert(rc == 0, "call function failed");
    mu_assert(strcmp(msg, "OK\n") == 0, "call function failed");

    char *grilledbeef_delete = "/delete grilledbeef";
    parts = cmd_parts(grilledbeef_delete);
    mu_assert(parts != NULL, "cmd_parts failed");
    rc = call_function(NS_DELETE, parts, msg);
    mu_assert(rc == 0, "call function failed");
    mu_assert(strcmp(msg, "OK\n") == 0, "call function failed");

    return NULL;
}

char *test_process()
{
    char *out = (char *) calloc(RSP_SIZE + 1, sizeof(char));
    mu_assert(out != NULL, "out invalid");

    char *bacon = "/create bacon";
    int rc = process(bacon, out);
    mu_assert(rc == 0, "process failed");
    mu_assert(strcmp(out, "OK\n") == 0, "process failed");

    char *ham = "/create ham";
    rc = process(ham, out);
    mu_assert(rc == 0, "process failed");
    mu_assert(strcmp(out, "OK\n") == 0, "process failed");

    char *beef = "/create beef";
    rc = process(beef, out);
    mu_assert(rc == 0, "process failed");
    mu_assert(strcmp(out, "OK\n") == 0, "process failed");

    char *ham_sample = "/sample ham 4.3";
    rc = process(ham_sample, out);
    mu_assert(rc == 0, "process failed");
    mu_assert(strcmp(out, "Mean: 4.30\n") == 0, "process failed");

    ham_sample = "/sample ham 6.0";
    rc = process(ham_sample, out);
    mu_assert(rc == 0, "process failed");
    mu_assert(strcmp(out, "Mean: 5.15\n") == 0, "process failed");

    char *bacon_sample = "/sample bacon 4.2";
    rc = process(bacon_sample, out);
    mu_assert(rc == 0, "process failed");
    mu_assert(strcmp(out, "Mean: 4.20\n") == 0, "process failed");

    bacon_sample = "/Sample bacon 6.9";
    rc = process(bacon_sample, out);
    mu_assert(rc == 0, "process failed");
    mu_assert(strcmp(out, "Mean: 5.55\n") == 0, "process failed");

    char *bacon_mean = "/mean bacon";
    rc = process(bacon_mean, out);
    mu_assert(rc == 0, "process failed");
    mu_assert(strcmp(out, "Mean: 5.55\n") == 0, "process failed");

    char *bacon_dump = "/dump bacon";
    rc = process(bacon_dump, out);
    mu_assert(rc == 0, "process failed");
    mu_assert(strcmp(out, "sum: 11.100000, sumsq: 65.250000, n: 2, min: 4.200000, max: 6.900000, mean: 5.550000, stddev: 1.909188\n") == 0, "process failed");

    char *bacon_delete = "/delete bacon";
    rc = process(bacon_delete, out);
    mu_assert(rc == 0, "process failed");
    mu_assert(strcmp(out, "OK\n") == 0, "process failed");

    char *list = "/list";
    rc = process(list, out);
    mu_assert(rc == 0, "process failed");
    mu_assert(strcmp(out, "ham\nbeef\n") == 0
              || strcmp(out, "beef\nham\n") == 0,
              "process failed");

    char *ham_store = "/store ham";
    rc = process(ham_store, out);
    mu_assert(rc == 0, "process failed");
    mu_assert(strcmp(out, "OK\n") == 0, "process failed");

    char *beef_store = "/store beef";
    rc = process(beef_store, out);
    mu_assert(rc == 0, "process failed");
    mu_assert(strcmp(out, "OK\n") == 0, "process failed");

    char *bacon_store = "/store bacon";
    rc = process(bacon_store, out);
    mu_assert(rc == -1, "process failed");
    mu_assert(strcmp(out, "error: store failed\n") == 0, "process failed");

    char *roastedham_load = "/load ham roastedham";
    rc = process(roastedham_load, out);
    mu_assert(rc == 0, "process failed");
    mu_assert(strcmp(out, "OK\n") == 0, "process failed");

    char *roastedham_delete = "/delete roastedham";
    rc = process(roastedham_delete, out);
    mu_assert(rc == 0, "process failed");
    mu_assert(strcmp(out, "OK\n") == 0, "process failed");

    roastedham_load = "/load ham roastedham";
    rc = process(roastedham_load, out);
    mu_assert(rc == 0, "process failed");
    mu_assert(strcmp(out, "OK\n") == 0, "process failed");

    char *roastedham_dump = "/dump roastedham";
    rc = process(roastedham_dump, out);
    mu_assert(rc == 0, "process failed");
    mu_assert(strcmp(out, "sum: 10.300000, sumsq: 54.490000, n: 2, min: 4.300000, max: 6.000000, mean: 5.150000, stddev: 1.202082\n") == 0, "process failed");

    return NULL;
}

char *all_tests()
{
    mu_suite_start();

    mu_run_test(test_sanitize);
    mu_run_test(test_check_cmd);
    mu_run_test(test_cmd_parts);
    mu_run_test(test_find_function);
    mu_run_test(test_call_function);
    mu_run_test(test_process);

    return NULL;
}

RUN_TESTS(all_tests);
