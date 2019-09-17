#include <necho.h>
#include <nsocket.h>

#define BACKLOG 10

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
            echoserve(sockfd_c);
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
