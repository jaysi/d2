#ifndef D2_CONF_H
#define D2_CONF_H

#include <sys/types.h>
#include "lib13.h"

#define DEF_EXP_BUFSIZE	1024
#define DEF_TOK_BUFSIZE	128
#define DEF_NHIST       20
#define DEF_EXP_FLAGS	0
#define DEF_MAX_HIST    DEF_HIST*10
#define DEF_MIN_HIST    1
#define DEF_MAX_EXPBUF  DEF_EXP_BUFSIZE*10
#define DEF_MIN_EXPBUF  DEF_EXP_BUFSIZE
#define DEF_MAX_OUTBUF  DEF_OUT_BUFSIZE*10
#define DEF_MIN_OUTBUF  DEF_OUT_BUFSIZE
#define DEF_MAX_TOKBUF  DEF_TOK_BUFSIZE*10
#define DEF_MIN_TOKBUF  DEF_TOK_BUFSIZE
#define DEF_STAT_FILENAME   "last.d2s"
#define DEF_AUTOLOAD_FILENAME   "autoload"
#define DEF_LOCALLOCK_FILENAME  ".double.lk"
#define MAXPATHNAME	1024

#define MAXNUMLEN 21

struct d2_conf {
    size_t exp_bufsize;
    size_t tok_bufsize;
    size_t nhistory;
};

#ifdef __cplusplus
extern "C" {
#endif

e13_t d2_load_conf(struct d2_conf* conf, char* filename);

#ifdef __cplusplus
}
#endif

#endif//D2_CONF_H