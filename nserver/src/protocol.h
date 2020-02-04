#ifndef _protocol_h
#define _protocol_h


#include <bstrlib.h>
#include <darray.h>
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


#endif
