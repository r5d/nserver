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


int check_cmd_size(char *cmd, int sock)
{
    check(cmd != NULL, "cmd is NULL");

    size_t len = strlen(cmd);
    if (len <= CMD_SIZE) {
        return 0;
    }

    char *err = "error: command invalid: size > 120 characters\n";
    send(sock, err, strlen(err), 0);

    return 1;
 error:
    return -1;
}


void nserve(int sock)
{
    size_t buf_sz = 200;
    char *buf = calloc(sizeof(char), buf_sz);
    check_mem(buf);

    // Read command from socket.
    ssize_t bytes = slurpsock(buf, buf_sz, sock);
    check(bytes >= 0, "nserve: slurpsock failed");

    int rc = check_cmd_size(buf, sock);
    check(rc != -1, "command size check failed");

    // Quit immediately if cmd size is too large.
    if (rc == 1) {
        goto error;
    }

    // Write response to socket.
    rc = barfsock(buf, bytes, sock);
    check(rc != -1, "nserve: echo failed");

    // Close socket.
    rc = close(sock);
    check(rc == 0, "nserve: close failed");

    // Cleanup.
    free(buf);

    exit(0);
 error:
    rc = close(sock);
    check(rc == 0, "nserve: close failed");

    // Cleanup if needed.
    if (buf)
        free(buf);

    exit(1);
}
