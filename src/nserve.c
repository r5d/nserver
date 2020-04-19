/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright © 2020 rsiddharth <s@ricketyspace.net>
 */

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

int set_recv_timeout(int sock) {
    struct timeval tv;
    tv.tv_sec = 30;
    tv.tv_usec = 0;

    int rc = setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO,
                       &tv, sizeof(tv));
    check(rc == 0, "setsockopt failed");

    return 0;
 error:
    return -1;
}

void nserve(int sock)
{
    char *out = NULL, *cmd = NULL;
    int rc = 0;

    check(sock > 0, "invalid socket");

    rc = set_recv_timeout(sock);
    check(rc == 0, "setting recv timeout failed");

    out = (char *) calloc(RSP_SIZE + 1, sizeof(char));
    check_mem(out);

    cmd = (char *) calloc(CMD_MAX_SIZE  + 1, sizeof(char));
    check_mem(cmd);

    do {
        // clear out, cmd.
        memset(out, '\0', RSP_SIZE + 1);
        memset(cmd, '\0', CMD_MAX_SIZE + 1);

        // Read command from socket.
        ssize_t bytes = slurpsock(cmd, CMD_MAX_SIZE, sock);
        check(bytes >= 0, "nserve: slurpsock failed");

        rc = process(cmd, out);
        if (rc < 0) {
            break;
        }

        // Write response to socket.
        rc = barfsock(out, strlen(out), sock);
        check(rc != -1, "nserve: echo failed");
    } while(1);

    // fallthrough.
 error:
    strncpy(out, "closing connection\n", RSP_SIZE);
    barfsock(out, strlen(out), sock);

    close(sock);

    // Cleanup if needed.
    if (cmd)
        free(cmd);
    if (out)
      free(out);

    exit(0);
}
