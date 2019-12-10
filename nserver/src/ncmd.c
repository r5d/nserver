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

char *check_cmd(char *cmd)
{
    char *err = NULL;

    check(cmd != NULL, "cmd is NULL");

    int rc = sanitize(cmd);
    check(rc != -1, "sanitize failed");

    size_t len = strlen(cmd);
    if (len >= CMD_MIN_SIZE && len <= CMD_MAX_SIZE) {
        return NULL;
    }

    if (len == 0) {
        err = "closing connection\n";
    } else {
        err = "command size invalid\n";
    }

    return err;
 error:
    err = "internal error\n";
    return err;
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

char *call_function(int func, struct bstrList *cmd_parts)
{
    char *msg = NULL;
    int msg_sz = 200;
    msg = (char *) calloc(msg_sz + 1, sizeof(char));
    check_mem(msg);

    if (func < 0 || cmd_parts == NULL || cmd_parts->qty < 1) {
        strncpy(msg, "error: args invalid\n", msg_sz);

        return msg;
    }

    switch (func) {
    case NS_CREATE:
        if(check_args(cmd_parts, 2) != 0) {
            strncpy(msg, "error: command invalid\n", msg_sz);
            break;
        }
        if (sscreate(bdata(cmd_parts->entry[1])) != 0) {
            strncpy(msg, "error: create failed\n", msg_sz);
            break;
        }
        strncpy(msg, "OK\n", msg_sz);

        break;
    case NS_SAMPLE:
        if(check_args(cmd_parts, 3) != 0) {
            strncpy(msg, "error: command invalid\n", msg_sz);
            break;
        }

        double sample = strtod(bdata(cmd_parts->entry[2]), NULL);
        if (sssample(bdata(cmd_parts->entry[1]), sample) != 0) {
            strncpy(msg, "error: sample failed\n", msg_sz);
            break;
        }
        strncpy(msg, "OK\n", msg_sz);

        break;
    case NS_MEAN:
        if(check_args(cmd_parts, 2) != 0) {
            strncpy(msg, "error: command invalid\n", msg_sz);
            break;
        }

        double mean = ssmean(bdata(cmd_parts->entry[1]));
        if (mean < 0) {
            strncpy(msg, "error: mean failed\n", msg_sz);
            break;
        }
        if (sprintf(msg, "Mean: %.2f\n", mean) < 0) {
            strncpy(msg, "error: mean failed\n", msg_sz);
        }
        break;
    case NS_DUMP:
        if(check_args(cmd_parts, 2) != 0) {
            strncpy(msg, "error: command invalid\n", msg_sz);
            break;
        }

        char *dump = ssdump(bdata(cmd_parts->entry[1]));
        if (dump == NULL) {
            strncpy(msg, "error: dump failed\n", msg_sz);
            break;
        }

        // Clean up msg.
        free(msg);

        msg = dump;
        break;
    case NS_DELETE:
        if(check_args(cmd_parts, 2) != 0) {
            strncpy(msg, "error: command invalid\n", msg_sz);
            break;
        }

        if (ssdelete(bdata(cmd_parts->entry[1])) != 0) {
            strncpy(msg, "error: delete failed\n", msg_sz);
            break;
        }
        strncpy(msg, "OK\n", msg_sz);

        break;
    case NS_LIST:
        if(check_args(cmd_parts, 1) != 0) {
            strncpy(msg, "error: command invalid\n", msg_sz);
            break;
        }

        char *list = sslist();
        if (list == NULL) {
            strncpy(msg, "error: list failed\n", msg_sz);
            break;
        }

        // Clean up msg.
        free(msg);

        msg = list;
        break;
    default:
        strncpy(msg, "error: function not found\n", msg_sz);
        break;
    }

    return msg;
 error:
    return NULL;
}

}
