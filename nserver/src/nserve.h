#ifndef _nserve_h
#define _nserve_h

#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>

#include <bstrlib.h>
#include <dbg.h>
#include <ringbuffer.h>

void nserve(int sock);

#endif