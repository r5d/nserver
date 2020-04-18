/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright © 2020 rsiddharth <s@ricketyspace.net>
 */

#ifndef _db_h
#define _db_h

#include <gdbm.h>
#include <dbg.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

int db_init();
int db_store(char *key, char *value);
char *db_load(char *key);

#endif

