/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright © 2020 rsiddharth <s@ricketyspace.net>
 */

#ifndef _protocol_h
#define _protocol_h


#include <bstrlib.h>
#include <darray.h>
#include <db.h>
#include <hashmap.h>
#include <stats.h>
#include <tstree.h>
#include <dbg.h>

typedef struct Record {
    bstring key;
    Stats *st;
    int deleted;
} Record;

int sscreate(char *key);
int ssdelete(char *key);
double sssample(char *key, double s);
double ssmean(char *key);
char *ssdump(char *key);
char *sslist();
int ssstore(char *key);
int ssload(char *from, char *to);


#endif
