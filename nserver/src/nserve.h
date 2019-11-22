#ifndef _nserve_h
#define _nserve_h

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>

#include <bstrlib.h>
#include <dbg.h>
#include <ringbuffer.h>

#define CMD_SIZE 120

void nserve(int sock);

#endif
