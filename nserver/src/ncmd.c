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
        check(cmd_parts->qty == 2, "cmd invalid");

        return NS_CREATE;
    } else if (bstricmp(cmd_name, &fsample) == 0) {
        check(cmd_parts->qty == 3, "cmd invalid");

        return NS_SAMPLE;
    } else if (bstricmp(cmd_name, &fmean) == 0) {
        check(cmd_parts->qty == 2, "cmd invalid");

        return NS_MEAN;
    } else if (bstricmp(cmd_name, &fdump) == 0) {
        check(cmd_parts->qty == 2, "cmd invalid");

        return NS_DUMP;
    } else if (bstricmp(cmd_name, &fdelete) == 0) {
        check(cmd_parts->qty == 2, "cmd invalid");

        return NS_DELETE;
    } else if (bstricmp(cmd_name, &flist) == 0) {
        check(cmd_parts->qty == 1, "cmd invalid");

        return NS_LIST;
    } else {
        return NS_NOP;
    }

 error:
    return NS_NOP;
}

