#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include <dbg.h>
#include <nsocket.h>

#define BACKLOG 10

int echo(int sock, char *buf, int len)
{
    int bytes = 0;

    bytes = send(sock, buf, len, 0);
    check(bytes >= 0, "statserv: send failed");

    return bytes;
 error:
    return -1;
}

void serve(int sock)
{
    int rc = 0;
    int buf_len = 1000;
    char buf[buf_len];
    ssize_t bytes;

    do {
        bytes = recv(sock, buf, buf_len, 0);
        check(bytes >= 0, "statserv: recv failed");

        if (bytes < 1) {
            break;
        }

        rc = echo(sock, buf, bytes);
        check(rc != -1, "statserv: echo failed");
    } while(1);

    rc = close(sock);
    check(rc == 0, "statserv: close failed");

    exit(0);
 error:
    rc = close(sock);
    check(rc == 0, "statserv: close failed");
    exit(1);
}

int main(void)
{
    int rc = 0;
    int sockfd_s = 0, sockfd_c = 0;
    struct sockaddr sockaddr_c;
    socklen_t sockaddr_c_len;
    pid_t pidc;

    sockfd_s = get_socket();
    check(sockfd_s > 0, "stateserv: unable to get socket");

    rc = listen(sockfd_s, BACKLOG);
    check(rc == 0, "statserve: listen failed");

    do {
        printf("Waiting for connection...\n");
        sockfd_c = accept(sockfd_s, &sockaddr_c, &sockaddr_c_len);

        pidc = fork();
        check(pidc != -1, "statserve: fork failed");

        if (pidc == 0) {
            serve(sockfd_c);
        }

        rc = close(sockfd_c);
        check(rc == 0, "statserv: close failed");
    } while(1);

    rc = close(sockfd_s);
    check(rc == 0, "statserv: close failed");

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
