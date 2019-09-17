#ifndef _necho_h
#define _necho_h

#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>

#include <bstrlib.h>
#include <dbg.h>
#include <ringbuffer.h>

int echo(int sock, char *buf, int len);
void echoserve(int sock);

#endif
