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

void nserve(int sock)
{
    char *out = NULL, *cmd = NULL;

    out = (char *) calloc(RSP_SIZE + 1, sizeof(char));
    check_mem(out);

    cmd = (char *) calloc(CMD_MAX_SIZE  + 1, sizeof(char));
    check_mem(cmd);

    int rc = 0;
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

    exit(1);
}
