/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright © 2010, Zed A. Shaw.
 * Copyright © 2020 rsiddharth <s@ricketyspace.net>
 */

#include <nserve.h>
#include <nsocket.h>
#include <signal.h>
#include <sys/wait.h>

#define BACKLOG 10

void handle_sigchild(int sig)
{
    sig = 0; // ignore it
    while(waitpid(-1, NULL, WNOHANG) > 0) {
    }
}

int setup_sigaction()
{
    struct sigaction sa = {
         .sa_handler = handle_sigchild,
         .sa_flags = SA_RESTART | SA_NOCLDSTOP
    };
    sigemptyset(&sa.sa_mask);
    int rc = sigaction(SIGCHLD, &sa, 0);
    check(rc != -1, "signal handler setup failed.");

    return 0;
 error:
    return -1;
}

int main(void)
{
    int rc = 0;
    int sockfd_s = 0, sockfd_c = 0;
    struct sockaddr sockaddr_c;
    socklen_t sockaddr_c_len;
    pid_t pidc;

    // create a sigaction that handles SIGCHLD
    rc = setup_sigaction();
    check(rc != -1, "sigaction setup failed");

    sockfd_s = get_socket();
    check(sockfd_s > 0, "nserver: unable to get socket");

    rc = listen(sockfd_s, BACKLOG);
    check(rc == 0, "nserver: listen failed");

    do {
        printf("Waiting for connection...\n");
        sockfd_c = accept(sockfd_s, &sockaddr_c, &sockaddr_c_len);

        pidc = fork();
        check(pidc != -1, "nserver: fork failed");

        if (pidc == 0) {
            close(sockfd_s);
            nserve(sockfd_c);
        }

        rc = close(sockfd_c);
        check(rc == 0, "nserver: close failed");
    } while(1);

    rc = close(sockfd_s);
    check(rc == 0, "nserver: close failed");

    return 0;

 error:
    if (sockfd_s) {
        rc = close(sockfd_s);
    }
    if (sockfd_c) {
        rc = close(sockfd_c);
    }

    return -1;
}
