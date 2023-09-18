#ifndef D2_VAR_H
#define D2_VAR_H

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
	struct d2_rec *rectab;	//all vars are arrays!
	struct d2_var *next;
};

#endif				//D2_VAR_H
