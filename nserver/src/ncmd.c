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
