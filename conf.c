#include "include/conf.h"
#include "error13.h"

struct d2_conf def_conf = {
	DEF_IN_BUFSIZE,
	DEF_NAME_BUFSIZE,
	DEF_EXP_BUFSIZE,
	DEF_TOK_BUFSIZE,
	DEF_NHIST
};

e13_t d2_load_conf(struct d2_conf *conf, char *filename)
{
	if (!filename)
		*conf = def_conf;
	return E13_OK;
}
