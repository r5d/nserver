#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include <dbg.h>
#include <nsocket.h>

int get_socket()
{
    int sockfd = 0;
    int rc = 0;

    struct addrinfo hints;
    struct addrinfo *servinfo = NULL;
    struct addrinfo *addr =  NULL;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    rc = getaddrinfo(NULL, PORT, &hints, &servinfo);
    check(rc == 0, "get_socket: getaddrinfo failed");

    // Loop through the addresses and find one that works.
    sockfd = 0;
    addr = servinfo;
    do {
        sockfd = socket(addr->ai_family, addr->ai_socktype,
                          addr->ai_protocol);
        if (sockfd > 0) {
            break;
        }

        addr = addr->ai_next;
    } while(addr);
    check(sockfd > 0, "unable to get socket");

    // assign name to socket.
    rc = bind(sockfd, addr->ai_addr, addr->ai_addrlen);
    check(rc == 0, "get_socket: bind failed");

    // Cleanup.
    freeaddrinfo(servinfo);

    return sockfd;
 error:
    if (servinfo)
        freeaddrinfo(servinfo);

    return -1;
}
