#ifndef D2_VAR_H
#define D2_VAR_H

#include "error13.h"

#define VAL_PI	3.1415926535897932384626433832795
#define VAL_E	2.7182818284590452353602874713527

#define ANSVARNAME	"ans"
#define PIVARNAME	"pi"
#define EVARNAME	"e"
#define CLIPVARNAME	"clip"

#define ANSVARDESC	"last answer"
#define PIVARDESC	"pi number"
#define EVARDESC	"e number"
#define CLIPVARDESC	"clipboard contents"

struct d2_var {
	char flags;
	char *name;
	char *desc;
	size_t nrec;
    fnum_t val;
	struct d2_rec *rectab;	//all vars are arrays!
	struct d2_var *next;
};

struct d2_ctx;

#ifdef __cplusplus
extern "C" {
#endif

e13_t d2_var_val(struct d2_ctx *ctx, char *name, long double *val);
e13_t d2_assign_var(struct d2_ctx *ctx, struct d2_tok *tok, struct d2_tok* tok1);

#ifdef __cplusplus
}
#endif

#endif				//D2_VAR_H
