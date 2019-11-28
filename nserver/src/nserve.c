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
