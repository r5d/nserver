#include <necho.h>

int slurpsock(RingBuffer *buf, int sock)
{
    ssize_t bytes;

    if (RingBuffer_available_data(buf) ==  0)  {
        buf->start = buf->end = 0;
    }

    bytes = recv(sock, RingBuffer_starts_at(buf),
              RingBuffer_available_space(buf), 0);
    check(bytes >= 0, "Failed to read from socket: %d", sock);

    RingBuffer_commit_write(buf, bytes);

    return bytes;

 error:
    return -1;
}

int barfsock(int sock, RingBuffer *buf, int len)
{
    int bytes = 0;

    bstring data = RingBuffer_get_all(buf);
    check(data != NULL, "barfsock: Failed to get from the buffer.");

    bytes = send(sock, bdata(data), len, 0);
    check(bytes >= 0, "barfsock: send failed");

    return bytes;
 error:
    return -1;
}

void echoserve(int sock)
{
    int rc = 0;
    RingBuffer *buf = RingBuffer_create(1024 * 10);
    ssize_t bytes;

    do {
        bytes = slurpsock(buf, sock);
        check(bytes >= 0, "echoserve: slurpsock failed");

        if (bytes < 1) {
            break;
        }

        rc = barfsock(sock, buf, bytes);
        check(rc != -1, "echoserve: echo failed");
    } while(1);

    rc = close(sock);
    check(rc == 0, "echoserve: close failed");

    exit(0);
 error:
    rc = close(sock);
    check(rc == 0, "echoserve: close failed");
    exit(1);
}
