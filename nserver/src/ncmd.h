#ifndef _ncmd_h
#define _ncmd_h

#include <string.h>

#include <bstrlib.h>
#include <dbg.h>

#define CMD_MIN_SIZE 5
#define CMD_MAX_SIZE 120

int sanitize(char *cmd);
char *check_cmd(char *cmd);

struct bstrList *cmd_parts(char *cmd);

#endif
