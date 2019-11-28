#include <nserve.h>

int slurpsock(char *buf, size_t buf_sz, int sock)
{
    ssize_t bytes;

    bytes = recv(sock, buf, buf_sz, 0);
    check(bytes >= 0, "Failed to read from socket: %d", sock);

    return bytes;

 error:
    return -1;
}

int barfsock(char *buf, size_t buf_sz, int sock)
{
    ssize_t bytes = 0;

    bytes = send(sock, buf, buf_sz, 0);
    check(bytes >= 0, "barfsock: send failed");

    return bytes;
 error:
    return -1;
}

int sanitize(char *cmd)
{
    check(cmd != NULL, "cmd is NULL");

    size_t len = strlen(cmd);
    check(len > 0, "cmd empty");

    // strip newline character at the end.
    cmd[len - 1] = '\0';

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


void nserve(int sock)
{
    size_t cmd_sz = 200;
    char *cmd = calloc(sizeof(char), cmd_sz);
    check_mem(cmd);

    int rc = 0;
    char *err = NULL;
    do {
        // clear cmd.
        memset(cmd, '\0', cmd_sz);

        // Read command from socket.
        ssize_t bytes = slurpsock(cmd, cmd_sz, sock);
        check(bytes >= 0, "nserve: slurpsock failed");

        err = check_cmd(cmd);
        if (err != NULL) {
            // cmd invalid; quit.
            rc = barfsock(err, strlen(err), sock);
            check(rc != -1, "barfsock failed");

            break;
        }

        // Write response to socket.
        rc = barfsock(cmd, bytes, sock);
        check(rc != -1, "nserve: echo failed");
    } while(1);

    // Close socket.
    rc = close(sock);
    check(rc == 0, "nserve: close failed");

    // Cleanup.
    free(cmd);

    exit(0);
 error:
    rc = close(sock);
    check(rc == 0, "nserve: close failed");

    // Cleanup if needed.
    if (cmd)
        free(cmd);

    exit(1);
}
