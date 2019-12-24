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
    char *out = (char *) calloc(sizeof(char), RSP_SIZE + 1);
    check_mem(out);

    char *cmd = (char *) calloc(sizeof(char), CMD_MAX_SIZE  + 1);
    check_mem(cmd);

    int rc = 0;
    do {
        // clear out, cmd.
        memset(out, '\0', RSP_SIZE + 1);
        memset(cmd, '\0', CMD_MAX_SIZE + 1);

        // Read command from socket.
        ssize_t bytes = slurpsock(cmd, CMD_MAX_SIZE, sock);
        check(bytes >= 0, "nserve: slurpsock failed");

        rc = check_cmd(cmd, out);
        if (rc < 0) {
            // cmd invalid; quit.
            rc = barfsock(out, strlen(out), sock);
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
