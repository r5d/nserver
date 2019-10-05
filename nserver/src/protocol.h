#ifndef _protocol_h
#define _protocol_h

#include <bstrlib.h>
#include <hashmap.h>
#include <stats.h>
#include <dbg.h>

int sscreate(char *key);
int ssdelete(char *key);
int sssample(char *key, double s);
double ssmean(char *key);
char *ssdump(char *key);


#endif
