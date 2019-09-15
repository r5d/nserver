#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include <dbg.h>
#include <nsocket.h>

int get_socket()
{
    int sockfd = 0;
    int rc = 0;
    int y = 1;

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
    for (addr = servinfo;  addr != NULL; sockfd = 0,
             addr = addr->ai_next)  {
        sockfd = socket(addr->ai_family, addr->ai_socktype,
                          addr->ai_protocol);
        if (sockfd < 1) {
            continue;
        }

        // dodge the "address already in use" error.
        rc = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR,
                        &y, sizeof(int));
        if (rc != 0) {
            continue;
        }

        // assign name to socket.
        rc = bind(sockfd, addr->ai_addr, addr->ai_addrlen);
        if (rc != 0) {
            continue;
        }

        break;
    }
    check(sockfd > 0, "unable to get socket");

    // Cleanup.
    freeaddrinfo(servinfo);

    return sockfd;
 error:
    if (servinfo)
        freeaddrinfo(servinfo);

    return -1;
}
