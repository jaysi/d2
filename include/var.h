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

struct d2_var{
    char type;
    char flags;
    char* name;
    size_t nval;
    fnum_t* val;
    char* exp;
    char* desc;    
    struct d2_var* next;
    struct d2_tok* pre;
};

#endif //D2_VAR_H