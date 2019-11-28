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

int check_cmd_size(char *cmd, int sock)
{
    check(cmd != NULL, "cmd is NULL");

    size_t len = strlen(cmd);
    if (len >= CMD_MIN_SIZE && len <= CMD_MAX_SIZE) {
        return 0;
    } else if (len == 0) {
        char *q = "closing connection\n";
        size_t bytes = barfsock(q, strlen(q), sock);
        check(bytes == strlen(q), "send sock failed");
    } else {
        char *err = "cmd size invalid\n";
        size_t bytes = barfsock(err, strlen(err), sock);
        check(bytes == strlen(err), "send sock failed");
    }

    return 1;
 error:
    return -1;
}


void nserve(int sock)
{
    size_t cmd_sz = 200;
    char *cmd = calloc(sizeof(char), cmd_sz);
    check_mem(cmd);

    // Read command from socket.
    ssize_t bytes = slurpsock(cmd, cmd_sz, sock);
    check(bytes >= 0, "nserve: slurpsock failed");

    int rc = check_cmd_size(cmd, sock);
    check(rc != -1, "command size check failed");

    // Quit immediately if cmd size is too large.
    if (rc == 1) {
        goto error;
    }

    // Write response to socket.
    rc = barfsock(cmd, bytes, sock);
    check(rc != -1, "nserve: echo failed");

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
