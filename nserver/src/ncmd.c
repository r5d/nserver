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

