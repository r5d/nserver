/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright © 2020 rsiddharth <s@ricketyspace.net>
 */

#include <ncmd.h>

int sanitize(char *cmd)
{
    check(cmd != NULL, "cmd is NULL");

    int len = strlen(cmd);
    check(len > 0, "cmd empty");

    // replace newline character with NUL.
    for (int i = 0; i < len; i++) {
        if (cmd[i] == '\n') {
            cmd[i] = '\0';

            break;
        }
    }
    return 0;
 error:
    return -1;
}

int check_cmd(char *cmd, char *err)
{
    check_mem(err);
    check(cmd != NULL, "cmd is NULL");

    int rc = sanitize(cmd);
    check(rc != -1, "sanitize failed");

    size_t len = strlen(cmd);
    if (len >= CMD_MIN_SIZE && len <= CMD_MAX_SIZE) {
        return 0;
    }

    if (len == 0) {
        strncpy(err, "closing connection\n", RSP_SIZE);
        return -1;
    } else {
        strncpy(err, "invalid command\n", RSP_SIZE);
        return -1;
    }

    return 0;
 error:
    strncpy(err, "internal error\n", RSP_SIZE);
    return -1;
}


struct bstrList *cmd_parts(char *cmd)
{
    bstring bcmd = NULL;
    struct bstrList *parts_tmp = NULL;

    bcmd = bfromcstr(cmd);
    check(bcmd != NULL, "bstring creation failed");

    parts_tmp = bsplit(bcmd, ' ');
    check(parts_tmp != NULL, "cmp split failed");
    check(parts_tmp->qty > 0, "qty check failed");

    struct bstrList *parts = bstrListCreate();
    check(parts != NULL, "parts create failed");

    int rc = bstrListAlloc(parts, parts_tmp->qty);
    check(rc == BSTR_OK, "parts alloc failed");
    check(parts->qty == 0, "qty check failed");

    bstring part = NULL;
    int index = 0;
    for (int i = 0; i < parts_tmp->qty; i++) {
        check(parts->qty <= parts->mlen, "parts capacity check failed");

        part  = parts_tmp->entry[i];

        if (blength(part) == 0) {
            continue;
        }

        parts->entry[index++] = bstrcpy(part);
        parts->qty++;
    }

    // Clean up.
    bdestroy(bcmd);
    bstrListDestroy(parts_tmp);

    return parts;
 error:
    // Clean up.
    if (bcmd) {
        bdestroy(bcmd);
    }
    if (parts_tmp) {
        bstrListDestroy(parts_tmp);
    }

    return NULL;
}

int find_function(struct bstrList *cmd_parts)
{
    // functions.
    struct tagbstring fcreate = bsStatic("/create");
    struct tagbstring fsample = bsStatic("/sample");
    struct tagbstring fmean = bsStatic("/mean");
    struct tagbstring fdump = bsStatic("/dump");
    struct tagbstring fdelete = bsStatic("/delete");
    struct tagbstring flist = bsStatic("/list");
    struct tagbstring fstore = bsStatic("/store");
    struct tagbstring fload = bsStatic("/load");

    check(cmd_parts != NULL, "cmd_parts is NULL");
    check(cmd_parts->qty > 0, "qty check failed");

    bstring cmd_name = cmd_parts->entry[0];
    check(blength(cmd_name) > 0, "cmd_name check failed");

    // trim cmd name
    int rc = btrimws(cmd_name);
    check(rc == BSTR_OK, "cmd name trim failed");

    // find function for cmd_name
    if (bstricmp(cmd_name, &fcreate) == 0) {
        return NS_CREATE;
    } else if (bstricmp(cmd_name, &fsample) == 0) {
        return NS_SAMPLE;
    } else if (bstricmp(cmd_name, &fmean) == 0) {
        return NS_MEAN;
    } else if (bstricmp(cmd_name, &fdump) == 0) {
        return NS_DUMP;
    } else if (bstricmp(cmd_name, &fdelete) == 0) {
        return NS_DELETE;
    } else if (bstricmp(cmd_name, &flist) == 0) {
        return NS_LIST;
    } else if (bstricmp(cmd_name, &fstore) == 0) {
        return NS_STORE;
    } else if (bstricmp(cmd_name, &fload) == 0) {
        return NS_LOAD;
    } else {
        return NS_NOP;
    }

 error:
    return NS_NOP;
}

int check_args(struct bstrList *cmd_parts, int argc)
{
    check(cmd_parts != NULL, "cmd_parts is NULL");
    check(cmd_parts->qty == argc, "qty check failed");

    bstring part = NULL;
    for (int i = 0; i < argc; i++) {
        part = cmd_parts->entry[i];

        check(blength(part) > 0, "part %d empty", i);
    }

    return 0;
 error:
    return -1;
}

int call_function(int func, struct bstrList *cmd_parts, char *out)
{
    check(out != NULL, "out invalid");

    if (func < 0 || cmd_parts == NULL || cmd_parts->qty < 1) {
        strncpy(out, "error: args invalid\n", RSP_SIZE);

        return -1;
    }

    double mean = 0.0;
    switch (func) {
    case NS_CREATE:
        if(check_args(cmd_parts, 2) != 0) {
            strncpy(out, "error: command invalid\n", RSP_SIZE);

            return -1;
        }
        if (sscreate(bdata(cmd_parts->entry[1])) < 0) {
            strncpy(out, "error: create failed\n", RSP_SIZE);

            return -1;
        }
        strncpy(out, "OK\n", RSP_SIZE);

        break;
    case NS_SAMPLE:
        if(check_args(cmd_parts, 3) != 0) {
            strncpy(out, "error: command invalid\n", RSP_SIZE);

            return -1;
        }

        double sample = strtod(bdata(cmd_parts->entry[2]), NULL);
        mean = sssample(bdata(cmd_parts->entry[1]), sample);
        if (mean < 0) {
            strncpy(out, "error: sample failed\n", RSP_SIZE);

            return -1;
        }
        if (sprintf(out, "Mean: %.2f\n", mean) < 0) {
            strncpy(out, "error: sample failed\n", RSP_SIZE);

            return -1;
        }
        break;
    case NS_MEAN:
        if(check_args(cmd_parts, 2) != 0) {
            strncpy(out, "error: command invalid\n", RSP_SIZE);

            return -1;
        }

        mean = ssmean(bdata(cmd_parts->entry[1]));
        if (mean < 0) {
            strncpy(out, "error: mean failed\n", RSP_SIZE);

            return -1;
        }
        if (sprintf(out, "Mean: %.2f\n", mean) < 0) {
            strncpy(out, "error: mean failed\n", RSP_SIZE);

            return -1;
        }
        break;
    case NS_DUMP:
        if(check_args(cmd_parts, 2) != 0) {
            strncpy(out, "error: command invalid\n", RSP_SIZE);

            return -1;
        }

        char *dump = ssdump(bdata(cmd_parts->entry[1]));
        if (dump == NULL) {
            strncpy(out, "error: dump failed\n", RSP_SIZE);

            return -1;
        }
        strncpy(out, dump, RSP_SIZE);

        // Clean up dump
        free(dump);

        break;
    case NS_DELETE:
        if(check_args(cmd_parts, 2) != 0) {
            strncpy(out, "error: command invalid\n", RSP_SIZE);

            return -1;
        }

        if (ssdelete(bdata(cmd_parts->entry[1])) != 0) {
            strncpy(out, "error: delete failed\n", RSP_SIZE);

            return -1;
        }
        strncpy(out, "OK\n", RSP_SIZE);

        break;
    case NS_LIST:
        if(check_args(cmd_parts, 1) != 0) {
            strncpy(out, "error: command invalid\n", RSP_SIZE);

            return -1;
        }

        char *list = sslist();
        if (list == NULL) {
            strncpy(out, "error: list failed\n", RSP_SIZE);

            return -1;
        }
        strncpy(out, list, RSP_SIZE);

        // Clean up list.
        free(list);

        break;
    case NS_STORE:
        if(check_args(cmd_parts, 2) != 0) {
            strncpy(out, "error: command invalid\n", RSP_SIZE);

            return -1;
        }
        if (ssstore(bdata(cmd_parts->entry[1])) < 0) {
            strncpy(out, "error: store failed\n", RSP_SIZE);

            return -1;
        }
        strncpy(out, "OK\n", RSP_SIZE);
        break;
    case NS_LOAD:
        if(check_args(cmd_parts, 3) != 0) {
            strncpy(out, "error: command invalid\n", RSP_SIZE);

            return -1;
        }
        if (ssload(bdata(cmd_parts->entry[1]),
                   bdata(cmd_parts->entry[2])) < 0) {
            strncpy(out, "error: load failed\n", RSP_SIZE);

            return -1;
        }
        strncpy(out, "OK\n", RSP_SIZE);
        break;
    default:
        strncpy(out, "error: function not found\n", RSP_SIZE);

        return -1;
    }

    return 0;
 error:
    return -1;
}

int process(char *cmd, char *out)
{
    check(out, "out invalid");

    int rc = check_cmd(cmd, out);
    check(rc == 0, "cmd check failed");

    // split cmd into parts.
    struct bstrList *parts = cmd_parts(cmd);
    check(parts != NULL, "cmd_parts failed");
    check(parts->qty > 0, "bstrList qty check failed");

    // call find_function.
    int FUNC = find_function(parts);
    check(FUNC != -1, "find function failed");

    // call call_function
    rc = call_function(FUNC, parts, out);
    check(rc != -1, "call function failed");

    return 0;
 error:
    return -1;
}

