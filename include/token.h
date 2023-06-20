#ifndef D2_TOKEN_H
#define D2_TOKEN_H

#include "types.h"

#define d2_blocklist "{}"
#define d2_brlist "()"
#define d2_logiclist "<>!=\"\'{}"
#define d2_delimlist ",~$[](){}<>!=&|:+-*/^% \t\"\'\n\r;"
#define d2_packlist "\"\'"
#define d2_escape '\\'
#define d2_lf	";"
#define d2_array "[]"
#define d2_chain '@'
#define d2_chain_str "@"
#define d2_pack1 '"'
#define d2_pack2 '\''

typedef enum d2_toktype_enum {
	TOKTYPE_EMPTY,
    TOKTYPE_ALL,
    TOKTYPE_BLOCK,
    TOKTYPE_SPEC,
    TOKTYPE_NUMBER,
    TOKTYPE_MATH_BASIC,
    TOKTYPE_STRING,
    TOKTYPE_MATH_FUNC,
    TOKTYPE_MATH_LOGIC,
    TOKTYPE_FUNC,
    TOKTYPE_COMMAND,
    TOKTYPE_RESERVE,	
	TOKTYPE_INVAL
} d2_toktype_t;

typedef enum d2_tok_enum {
	TOK_EMPTY,
	TOK_SEP,
    TOK_COL,
    TOK_WHITE,
    TOK_STRPACK,

    TOK_CHAIN,//5
//    TOK_COLON,

    TOK_BR_OPEN,
    TOK_BR_CLOSE,

    TOK_BLOCK,
    TOK_BLOCK_OPEN,
    TOK_BLOCK_CLOSE,//10

    TOK_ABR_OPEN,
    TOK_ABR_CLOSE,

    TOK_INT,
    TOK_REAL,
    TOK_SCIENCE,
    TOK_DATE,

    TOK_VAR,
    TOK_ARRAY,
    TOK_STR,//19

//    TOK_EQ_LONE,
//    TOK_COLUMN,

    TOK_PLUS,
    TOK_MINUS,
    TOK_MULT,
    TOK_DIV,
    TOK_POW,
    TOK_MOD,
    TOK_ASSIGN,
    TOK_AND,
    TOK_OR,
    TOK_PLUS_PLUS,
    TOK_MINUS_MINUS,
    TOK_PLUS_EQ,
    TOK_MINUS_EQ,

    TOK_SIN,
    TOK_ASIN,
    TOK_SINH, //20
    TOK_COS,
    TOK_ACOS,
    TOK_COSH,
    TOK_TAN,
    TOK_ATAN,
    TOK_RAD,
    TOK_DEG,
    TOK_LOG,
    TOK_LN,
    TOK_SQRT,//45
    TOK_ABS,
    TOK_ROUND,
    TOK_NROUND,
    TOK_JDAY,
    TOK_GDAY,
    TOK_COUNT,
    TOK_ANS_REF,

    TOK_LT, //<
    TOK_GT, //>
    TOK_EQ, //==
    TOK_NE, //!=
    TOK_LE, //<=
    TOK_GE, //>=

    TOK_AND_AND,//&&
    TOK_OR_OR,//||
    TOK_NOT,//!

    TOK_IF,
    TOK_ELSEIF,
    TOK_ELSE,
    TOK_FOR,

    TOK_CP,
    TOK_SEPCP,
    TOK_NUM2TEXT,

    TOK_VCLR,
    //TOK_VCLRALL,
    TOK_VDESC,
    TOK_VEXP,
    TOK_VCAT,
    TOK_VLIST,
    TOK_VSTR,
    TOK_VSTRCMP,
    TOK_VLOCK,
    TOK_VUNLOCK,
    TOK_VREN,
    TOK_VDUP,
    TOK_ARRAYDEF,
    TOK_AFILL,

    TOK_LOAD,
    TOK_SAVE,
    TOK_FLUSHF,
    TOK_VIEW,

    TOK_RECALC,
    TOK_XRECALC,
//    TOK_LOOP,
    TOK_LABEL,
    TOK_GOTO,
    TOK_RETURN,
    TOK_JUMP,
    TOK_AUTORUN,

    TOK_HISTORY,
    TOK_HLOAD,

    TOK_HIJRI,
    TOK_GREG,
    TOK_TODAY,
    TOK_JDATE,
    TOK_GDATE,

    TOK_MODE,

    TOK_FORK,
    TOK_ENDFORK,
    TOK_SWITCH,
    TOK_BREAK,
    TOK_SLEEP,
    TOK_WAIT,
    TOK_EXEC,

    TOK_PRINT,
    TOK_PLOT,
    TOK_AEDIT,

    TOK_STOP,
    TOK_RESUME,
    TOK_SHOW,
    TOK_REMOVE,
    TOK_UP,
    TOK_DOWN,
    TOK_TRIM,
    TOK_NORMAL,
    TOK_ACC,
    TOK_PROG,
    TOK_ROOT,
    TOK_PARENT,
    TOK_ALL,
    TOK_HEX,
    TOK_BIN,
    TOK_UI,
    TOK_FILE,
    TOK_BUF,
	TOK_CORE,

    TOK_SYS,
    TOK_VER,
    TOK_HELP,
    TOK_EXIT,
    TOK_TERM,
    TOK_SAVESTAT,	
	TOK_INVAL
} d2_tok_t;

struct d2_tok {
	size_t bufsize;
	char* buf;
	
	d2_tok_t tok_t;
    fnum_t fval;
	
	struct d2_tok* 	next, *prefix_next;
};

#ifdef __cplusplus
extern "C" {
#endif
char* __d2_tokenize(char* start, char delim[], char esc, char pack1, char pack2);
size_t __d2_count_token(char* start, char delim[], char esc, char pack1, char pack2);
#define d2_tok(exp) MACRO( __d2_tokenize(exp, d2_delimlist, d2_escape, d2_pack1, d2_pack2) )
#define d2_ntok(exp) MACRO( __d2_count_token(exp, d2_delimlist, d2_escape, d2_pack1, d2_pack2) )
#ifdef __cplusplus
}
#endif

#endif //D2_TOKEN_H