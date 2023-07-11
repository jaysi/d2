#ifndef D2_TOKEN_H
#define D2_TOKEN_H

#include "type13.h"
#include "types.h"

#define WHITESPACES " \t\"\'\n\r\f"
#define d2_blocklist "{}"
#define d2_brlist "()"
#define d2_logiclist "<>!=\"\'{}"
#define d2_delimlist ",~$[](){}<>!=&|?:+-*/^%;"WHITESPACES//with whitespaces
//#define d2_delimlist ",~$[](){}<>!=&|?:+-*/^%\"\';"//without whitespaces
#define d2_packlist "\"\'"
#define d2_escape '\\'
#define d2_lf	";"
#define d2_array "[]"
#define d2_chain '@'
#define d2_chain_str "@"
#define d2_pack1 '"'
#define d2_pack2 '\''

// struct d2_tok {
// 	size_t bufsize;
// 	char* buf;
	
// 	d2_tok_t tok_t;
//     fnum_t fval;
	
// 	struct d2_tok* 	next, *prefix_next;
// };

/*
Precedence	Type	Operators	                        Associativity
1	Postfix         () [] -> . ++ —	                    Left to Right
2	Unary	        + – ! ~ ++ — (type)* & sizeof	    Right to Left
3	Multiplicative	* / %	                            Left to Right
4	Additive	    + –	                                Left to Right
5	Shift	        << >>	                            Left to Right
6	Relational	    < <= > >=	                        Left to Right
7	Equality	    == !=	                            Left to Right
8	Bitwise AND	    &	                                Left to Right
9	Bitwise XOR 	^	                                Left to Right
10	Bitwise OR	    |	                                Left to Right
11	Logical AND	    &&	                                Left to Right
12	Logical OR	    ||	                                Left to Right
13	Conditional	    ?:	                                Right to Left
14	Assignment	    = += -+ *= /= %= >>= <<= &= ^= |=	Right to Left
15	Comma	        ,	                                Left to Right
*/

/*
C Operator Precedence
 
The following table lists the precedence and associativity of C operators. Operators are listed top to bottom, in descending precedence.
Precedence 	Operator 	Description 	Associativity
1 	++ -- 		Suffix/postfix increment and decrement 	Left-to-right
() 				Function call
[] 				Array subscripting
. 				Structure and union member access
-> 				Structure and union member access through pointer
(type){list} 	Compound literal(C99)
2 	++ -- 		Prefix increment and decrement[note 1] 	Right-to-left
+ - 			Unary plus and minus
! ~ 			Logical NOT and bitwise NOT
(type) 			Cast
* 				Indirection (dereference)
& 				Address-of
sizeof 			Size-of[note 2]
_Alignof 		Alignment requirement(C11)
3 	* / % 		Multiplication, division, and remainder 	Left-to-right
4 	+ - 		Addition and subtraction
5 	<< >> 		Bitwise left shift and right shift
6 	< <= 		For relational operators < and ≤ respectively
> >= 	        For relational operators > and ≥ respectively
7 	== != 	    For relational = and ≠ respectively
8 	& 	        Bitwise AND
9 	^ 	        Bitwise XOR (exclusive or)
10 	| 	        Bitwise OR (inclusive or)
11 	&& 	        Logical AND
12 	|| 	        Logical OR
13 	?: 	        Ternary conditional[note 3] 	Right-to-left
14[note 4] 	= 	Simple assignment
+= -= 	        Assignment by sum and difference
*= /= %= 	    Assignment by product, quotient, and remainder
<<= >>= 	    Assignment by bitwise left shift and right shift
&= ^= |= 	    Assignment by bitwise AND, XOR, and OR
15 	, 	        Comma 	Left-to-right

	↑ The operand of prefix ++ and -- can't be a type cast. This rule grammatically forbids some expressions that would be semantically invalid anyway. Some compilers ignore this rule and detect the invalidity semantically.
	↑ The operand of sizeof can't be a type cast: the expression sizeof (int) * p is unambiguously interpreted as (sizeof(int)) * p, but not sizeof((int)*p).
	↑ The expression in the middle of the conditional operator (between ? and :) is parsed as if parenthesized: its precedence relative to ?: is ignored.
	↑ Assignment operators' left operands must be unary (level-2 non-cast) expressions. This rule grammatically forbids some expressions that would be semantically invalid anyway. Many compilers ignore this rule and detect the invalidity semantically. For example, e = a < d ? a++ : a = d is an expression that cannot be parsed because of this rule. However, many compilers ignore this rule and parse it as e = ( ((a < d) ? (a++) : a) = d ), and then give an error because it is semantically invalid.

When parsing an expression, an operator which is listed on some row will be bound tighter (as if by parentheses) to its arguments than any operator that is listed on a row further below it. For example, the expression *p++ is parsed as *(p++), and not as (*p)++.

Operators that are in the same cell (there may be several rows of operators listed in a cell) are evaluated with the same precedence, in the given direction. For example, the expression a=b=c is parsed as a=(b=c), and not as (a=b)=c because of right-to-left associativity.
Notes

Precedence and associativity are independent from order of evaluation.

The standard itself doesn't specify precedence levels. They are derived from the grammar.

In C++, the conditional operator has the same precedence as assignment operators, and prefix ++ and -- and assignment operators don't have the restrictions about their operands.

Associativity specification is redundant for unary operators and is only shown for completeness: unary prefix operators always associate right-to-left (sizeof ++*p is sizeof(++(*p))) and unary postfix operators always associate left-to-right (a[1][2]++ is ((a[1])[2])++). Note that the associativity is meaningful for member access operators, even though they are grouped with unary postfix operators: a.b++ is parsed (a.b)++ and not a.(b++). 
*/

typedef enum {
	TOK_EMPTY,
	
	//100
	TOK_PAREN_OPEN,
	TOK_PAREN_CLOSE,
	TOK_ARRAY_OPEN,
	TOK_ARRAY_CLOSE,  

	//200
	TOK_PLUS1,
	TOK_MINUS1,
	//TOK_PLUS,
	//TOK_MINUS,
	TOK_LOGIC_NOT,
	TOK_BIT_NOT,

	//300
	TOK_MULT,
	TOK_DIV,
	TOK_REMAIN,
	
	//400
	TOK_ADD,
	TOK_SUB,

	//500
	TOK_BIT_SHIFT_LEFT,
	TOK_BIT_SHIFT_RIGHT,

	//600
	TOK_LT,
	TOK_LE,
	TOK_GT,
	TOK_GE,

	//700
	TOK_EQ,
	TOK_NE,

	//800
	TOK_BIT_AND,

	//900
	TOK_BIT_XOR,

	//1000
	TOK_BIT_OR,

	//1100
	TOK_LOGIC_AND,

	//1200
	TOK_LOGIC_OR,

	//1300
	TOK_TERN_COND,//?:

	//1400
	TOK_ASSIGN,
	TOK_ASSIGN_ADD,
	TOK_ASSIGN_SUB,
	TOK_ASSIGN_MULT,
	TOK_ASSIGN_DIV,
	TOK_ASSIGN_REMAIN,
	TOK_ASSIGN_BIT_SHIFT_LEFT,
	TOK_ASSIGN_BIT_SHIFT_RIGHT,
	TOK_ASSIGN_BIT_AND,
	TOK_ASSIGN_BIT_XOR,
	TOK_ASSIGN_BIT_OR,

	//1500
	TOK_COMMA,
    TOK_SEMICOLON,

	//1600 -- block
	TOK_BLOCK_OPEN,
	TOK_BLOCK_CLOSE,

    // -- access
    TOK_ACCESS,

    // -- math functions will implement as mods

    //1700 -- flow control c keywords
    TOK_IF,
    TOK_ELSE,
    TOK_ELSE_IF,
    TOK_SWITCH,
    TOK_CASE,
    TOK_DEFAULT,
    TOK_DO,
    TOK_WHILE,
    TOK_BREAK,
    TOK_CONTINUE,
    TOK_FOR,
    TOK_GOTO,
    TOK_RETURN,

    TOK_LABEL,   

    //1900 -- scripting
    TOK_THREAD,
    TOK_EXIT,    

    TOK_SYS,

    TOK_PRINT,

	//OPERANDS
    TOK_NUMBER,
	TOK_STRING,
	TOK_BLOB,

	//VAR
	TOK_VAR,

	//FUNCTIONS
	TOK_FN,
	
	TOK_INVAL
} d2_tok_enum;

struct d2_rec {
	uint16_t code;//written to disk in network byte order
	char* data;//also written to disk! nbo in short, long, long long and float
};

struct d2_tok {	
    //long long ival;
    long double dval;
	struct d2_rec rec;
	struct d2_tok* next, *prev;
};

#ifdef __cplusplus
extern "C" {
#endif
char* __d2_token(char* start, char delim[], char esc, char pack1, char pack2);
size_t __d2_estimate_ntokens(char* start, char delim[], char esc, char pack1, char pack2);
/*
e13_t d2_tokenize(struct d2_handle* h);
e13_t d2_lexer;
e13_t d2_parse;
*/
#define d2_tok(exp) MACRO( __d2_token(exp, d2_delimlist, d2_escape, d2_pack1, d2_pack2) )
#define d2_ntok(exp) MACRO( __d2_count_token(exp, d2_delimlist, d2_escape, d2_pack1, d2_pack2) )
#ifdef __cplusplus
}
#endif

#endif //D2_TOKEN_H