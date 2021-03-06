/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright © 2020 rsiddharth <s@ricketyspace.net>
 */

#ifndef _nsocket_h
#define _nsocket_h

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include <dbg.h>

#define PORT "7899"

int get_socket();

#endif
