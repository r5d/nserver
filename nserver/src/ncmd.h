#ifndef _ncmd_h
#define _ncmd_h

#include <string.h>

#include <bstrlib.h>
#include <dbg.h>
#include <protocol.h>

#define CMD_MIN_SIZE 5
#define CMD_MAX_SIZE 120
#define RSP_SIZE 200

enum FUNCTIONS {
                NS_CREATE,
                NS_SAMPLE,
                NS_MEAN,
                NS_DUMP,
                NS_DELETE,
                NS_LIST,
                NS_NOP = -1
};

int sanitize(char *cmd);
int check_cmd(char *cmd, char *err);

struct bstrList *cmd_parts(char *cmd);

int find_function(struct bstrList *cmd_parts);
int call_function(int func, struct bstrList *cmd_parts, char *out);

int process(char *cmd, char *out);

#endif
