#ifndef _db_h
#define _db_h

#include <gdbm.h>
#include <dbg.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

int db_init();
int db_store(char *key, char *value);

#endif

