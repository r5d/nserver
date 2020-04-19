/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright © 2020 rsiddharth <s@ricketyspace.net>
 */

#ifndef _nserve_h
#define _nserve_h

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/time.h>

#include <bstrlib.h>
#include <ncmd.h>
#include <dbg.h>

void nserve(int sock);

#endif
