#include <ctype.h>
#include <stdio.h>
#include <errno.h>
#include "lib13.h"
#include "include/token.h"
#include "include/d2.h"
#include "include/limmit.h"
#include "include/dmsg.h"
#include "include/sdmem.h"
#include "include/var.h"

/*

#define COLOR_TABLE \
X(red, "red")       \
X(green, "green")   \
X(blue, "blue")

#define X(a, b) a,
enum COLOR {
  COLOR_TABLE
};
#undef X

#define X(a, b) b,
char *color_name[] = {
  COLOR_TABLE
};
#undef X

int main() {
  enum COLOR c = red;
  printf("c=%s\n", color_name[c]);
  return 0;
}

*/

//enable to debug tok sequence;
#undef ENABLE_DUMP_TOK

#define dm_loc() fprintf(stderr, "loc: %s.%s().%i", __FILE__, __FUNC__, __LINE__)
#define dm(fmt, ...)
#define dm_tok1(fmt, ...)
#define dm_tok2(fmt, ...)
#define dm_tok3(fmt, ...)
#define dm_tok4(fmt, ...)
#define _dm_comb(fmt, ...)
#define _dm_flatz(fmt, ...)
#define dm_sci(fmt, ...)
/*
	source code shape:
	block;
		exp;
		block;
			exp;
		label:
*/ struct d2_tok_form_s {

	char *form;
	int preced;

} d2_tok_form[] = {
	//EMPTY
	{"", 0},

	//100
	{"(", 100},
	{")", 100},
	{"[", 100},
	{"]", 100},

	//200
	{"++", 200},
	{"--", 200},
	{"!", 200},
	{"~", 200},

	//300
	{"*", 300},
	{"/", 300},
	{"%", 300},

	//400
	{"+", 400},
	{"-", 400},

	//500
	{"<<", 500},
	{">>", 500},

	//600
	{"<", 600},
	{"<=", 600},
	{">", 600},
	{">=", 600},

	//700
	{"==", 700},
	{"!=", 700},

	//800
	{"&", 800},

	//900
	{"^", 900},

	//1000
	{"|", 1000},

	//1100
	{"&&", 1100},

	//1200
	{"||", 1200},

	//1300
	{"?:", 1300},

	//1400
	{"=", 1400},
	{"+=", 1400},
	{"-=", 1400},
	{"*=", 1400},
	{"/=", 1400},
	{"%=", 1400},
	{"<<=", 1400},
	{">>=", 1400},
	{"&=", 1400},
	{"^=", 1400},
	{"|=", 1400},

	//1500
	{",", 1500},
	{";", 1500},

	//1600
	{"{", 1600},
	{"}", 1600},

	//access
	{"@", 1650},

	//1700 - flow control - c keywords
	{"if", 1700},
	{"else", 1700},
	{"else if", 1700},
	{"switch", 1700},
	{"case", 1700},
	{"default", 1700},
	{"do", 1700},
	{"while", 1700},
	{"break", 1700},
	{"continue", 1700},
	{"for", 1700},
	{"goto", 1700},
	{"return", 1700},

	//label
	{":", 1800},

	//scripting 
	{"thread", 1900},
	{"exit", 1900},
	{"sys", 1900},
	{"print", 1900},

	//operands
	{"{NUMBER}", 0},
	{"{STRING}", 0},
	{"{BLOB}", 0},

	//vars
	{"{VARIABLE}", 0},
	{"{STRING_VARIABLE}", 0},

	//fns
	{"{FUNCTION}", 0},

	//INVAL
	{NULL}
};

int __d2_tok_preced(d2_tok_enum code)
{
	return d2_tok_form[code].preced;
}

//struct d2_tok* __d2_get_tok_ptr(struct )

size_t __d2_estimate_ntokens(char *start,
				 char delim[], char esc, char pack1, char pack2)
{
	char *end, *d = delim;
	char pack = 0;
	size_t cnt = 1UL;
	end = start;
	while (*end) {
		dm_tok1("end is %c, cnt = %lu\n", *end, cnt);
		if (*end == esc) {
			dm_tok1("escape %c passed to %c\n", *(end + 1),
				*(end + 2));
			end += 2;
			continue;
		}
		if (!pack) {
			d = delim;
			while (*d) {
				if (*d == *end) {
					if (*end == pack1 || *end == pack2) {
						pack = *end;
						break;
					}
					cnt += 2;
				}
				if (pack)
					continue;
				d++;
			}
		} else {	//if(!pack)
			if (*end == pack)
				cnt += 2;
		}
		end++;
	}
	return cnt;
}

char *__d2_token(char *start, char delim[], char esc, char pack1, char pack2)
{
	char *end, *d = delim;
	char pack = 0;
	end = start;
	while (*end) {
		if (*end == esc) {
			end += 2;
			continue;
		}
		if (!pack) {
			d = delim;
			while (*d) {
				if (*d == *end) {
					if (*end == pack1 || *end == pack2) {
						pack = *end;
						break;
					}
					//will take care of isspace() in the calling function
					//while(isspace(*end)) end++;//skip whitespaces
					return start == end ? end : end - 1;
				}
				if (pack)
					break;
				d++;
			}
		} else {
			if (*end == pack) {
				return end;
			}
		}
		end++;
	}
	return end;
}

/*
	this lex was not used actually! it is done inside the tokenize()
*/

e13_t d2_lex(struct d2_ctx* ctx, struct d2_tok *tok)
{
	struct d2_tok_form_s *form;

	tok->rec.code = TOK_EMPTY;

	form = d2_tok_form;
	for (d2_tok_enum tokenum = TOK_EMPTY; form->form;
			form++, tokenum++) {

		dm_tok3("%s->%s\n", form->form, tok->rec.data);

		if (!strcmp(form->form, tok->rec.data)) {
			tok->rec.code = tokenum;
			break;
		}
	}

	if (!form->form) {
		errno = 0;
		if (__d2_strtold(tok->rec.data, &tok->dval) == E13_OK)
			tok->rec.code = TOK_NUMBER;
		else if(d2_var_val(ctx, tok->rec.data, &tok->dval) == E13_OK)//TODO: unnecessary??
			tok->rec.code = TOK_VAR;
		else           
			tok->rec.code = TOK_STRING;
	}

	dm_tok2("ntok = %li, data = %s, code = %i\n",
		ctx->ntoks, tok->rec.data, tok->rec.code);
	return E13_OK;
}

e13_t __d2_is_combo_tok(d2_tok_enum tok_id)
{
	switch (tok_id) {
	case TOK_BIT_NOT:
	case TOK_MULT:
	case TOK_DIV:
	case TOK_REMAIN:
	case TOK_ADD:
	case TOK_SUB:
	case TOK_LT:
	case TOK_GT:
	case TOK_BIT_AND:
	case TOK_BIT_OR:
	case TOK_BIT_XOR:
	case TOK_ASSIGN:
		return E13_OK;
		break;
	default:
		break;
	}

	return e13_error(E13_FORMAT);
}

e13_t __d2_cmp_tok_form(struct d2_tok *tok, d2_tok_enum tok_id)
{

	assert(tok);

	struct d2_tok *tk = tok;
	int tok_form_len = strlen(d2_tok_form[tok_id].form);

	for (int i = 0; i < tok_form_len; i++) {
		_dm_comb("tk->code = %i\n", tk->rec.code);
		if (!tk ||
			d2_tok_form[tk->rec.code].form[0] !=
			d2_tok_form[tok_id].form[i])
			return e13_error(E13_FORMAT);
		tk = tk->next;
	}

	_dm_comb("found one!, id = %i\n", tok_id);
	return E13_OK;
}

#ifndef ENABLE_DUMP_TOK
e13_t __d2_dump_tok_list(struct d2_tok* tok){
	assert(tok);
	struct d2_tok* tk = tok;
	_dm_flatz("%s", "*** token list dump { ");
	while(tk){
		_dm_flatz("%s(%p) ", tk->rec.data, (void*)tk);
		tk = tk->next;
	}
	_dm_flatz("%s", " } token list dump ***\n");
	return E13_OK;
}

e13_t __d2_dump_tok_list_reverse(struct d2_tok* tok){
	assert(tok);
	struct d2_tok* tk = tok;
	_dm_flatz("%s", "*** reverse token list dump { ");
	while(tk){
		_dm_flatz("(%p)%s ", (void*)tk, tk->rec.data);
		tk = tk->prev;
	}
	_dm_flatz("%s", " } reverse token list dump ***\n");
	return E13_OK;
}

#endif

e13_t d2_combine(struct d2_ctx *ctx)
{
	struct d2_tok *tok;
	char sci_num[D2_MAX_SCI_NUM];

	d2_tok_enum math_combo_list[] =
		{ TOK_PLUS1, TOK_MINUS1, TOK_BIT_SHIFT_LEFT, TOK_BIT_SHIFT_RIGHT,
			TOK_LE, TOK_GE, TOK_EQ, TOK_NE, TOK_LOGIC_AND, TOK_LOGIC_OR, TOK_ASSIGN_ADD,
			TOK_ASSIGN_SUB, TOK_ASSIGN_DIV, TOK_ASSIGN_REMAIN, TOK_ASSIGN_BIT_AND,
			TOK_ASSIGN_BIT_XOR, TOK_ASSIGN_BIT_OR, TOK_ASSIGN_BIT_SHIFT_LEFT,
			TOK_ASSIGN_BIT_SHIFT_RIGHT };

	//1. resolve combo operators
	tok = ctx->tok_list_first;

#ifndef NDEBUG
	__d2_dump_tok_list(tok);  
	__d2_dump_tok_list_reverse(ctx->tok_list_last);
#endif

	while (tok) {
		_dm_comb("while(tok): tok->rec.code = %i, data = %s\n", tok->rec.code, tok->rec.data);
		for (int i = 0;
			 i < sizeof(math_combo_list) / sizeof(math_combo_list[0]);
			 i++) {
			if (__d2_cmp_tok_form(tok, math_combo_list[i]) == E13_OK) {	//for loop ended normally
				tok->rec.code = math_combo_list[i];
				__d2_realloc_tok_buf(tok, d2_tok_form[math_combo_list[i]].form, strlen(d2_tok_form[math_combo_list[i]].form), 0);	//free old data?

				if (math_combo_list[i] == TOK_ASSIGN_BIT_SHIFT_LEFT || math_combo_list[i] == TOK_ASSIGN_BIT_SHIFT_RIGHT){	//3 letter combos
					_dm_comb("3 letter comb, skip token %s\n", tok->next->next->rec.data);
					__d2_skip_tok(ctx, tok->next->next);
				}
				_dm_comb("skip token %s\n", tok->next->rec.data);
				__d2_skip_tok(ctx, tok->next);
			}	//if same == combolen
#ifndef NDEBUG            
			__d2_dump_tok_list_reverse(ctx->tok_list_last);
#endif            
		}

		tok = tok->next;

	}			//while(tok)

	//resolve else if
	for (tok = ctx->tok_list_first; tok; tok = tok->next) {
		if (tok->rec.code == TOK_ELSE && tok->next
			&& tok->next->rec.code == TOK_IF) {
			tok->rec.code = TOK_ELSE_IF;
			__d2_realloc_tok_buf(tok, d2_tok_form[TOK_ELSE_IF].form,
						 strlen(d2_tok_form
							[TOK_ELSE_IF].form), 0);
			__d2_skip_tok(ctx, tok->next);
		}
	}

	//resolve label
	if (ctx->tok_list_first &&
		ctx->tok_list_first->rec.code == TOK_STRING &&
		ctx->tok_list_first->next &&
		ctx->tok_list_first->next->rec.code == TOK_LABEL &&
		!ctx->tok_list_first->next->next) {
		ctx->tok_list_first->rec.code = TOK_LABEL;

		__d2_skip_tok(ctx, ctx->tok_list_first->next);
	}

	//2. resolve scientific numbers
	for (tok = ctx->tok_list_first; tok; tok = tok->next) {
		if (tok->rec.code == TOK_STRING) {
			//these 3 lines already done in tokenize
			//errno = 0;
			//tok->dval = strtold(tok->rec.data, NULL);//try format xxxEyyy
			//if(!errno) tok->rec.code = TOK_NUMBER;
			if (tok->next && tok->next->next &&	//try format xxxE+-yyy
				(tok->next->rec.code == TOK_ADD
				 || tok->next->rec.code == TOK_SUB)
				&& tok->next->next->rec.code == TOK_NUMBER) {
				snprintf(sci_num, D2_MAX_SCI_NUM, "%s%s%s",
					 tok->rec.data, tok->next->rec.data,
					 tok->next->next->rec.data);
				dm_sci("sci num: %s\n", sci_num);
				if (__d2_strtold(sci_num, &tok->dval) == E13_OK) {
					tok->rec.code = TOK_NUMBER;
					__d2_realloc_tok_buf(tok, sci_num, strlen(sci_num)+1, 1);
					__d2_skip_tok(ctx, tok->next->next);
					__d2_skip_tok(ctx, tok->next);
				}
			}	//else if
		}

		//3a. resolve sign (+-?)
		if(tok->rec.code == TOK_PAREN_OPEN){
			if(tok->next && tok->next->next &&
				(tok->next->rec.code == TOK_ADD || tok->next->rec.code == TOK_SUB) &&
				(tok->next->next->rec.code == TOK_NUMBER || tok->next->next->rec.code == TOK_VAR)){
			  	if(tok->next->rec.code == TOK_SUB){
                    if(tok->next->next->rec.code == TOK_NUMBER) tok->next->next->dval *= -1;
                    else tok->next->next->flags |= D2_TOKF_NEGATIVE;//for VARIABLE
                }
                __d2_skip_tok(ctx, tok->next);//skip sign anyways!
			}
		}

		//3b. resolve sign ?-+?, already removed -- and ++ above!
		if(tok->rec.code == TOK_NUMBER){
			if(tok->next && tok->next->next && tok->next->next->next &&
				(tok->next->rec.code == TOK_ADD || tok->next->rec.code == TOK_SUB) &&
				(tok->next->next->rec.code == TOK_ADD || tok->next->next->rec.code == TOK_SUB) &&
				(tok->next->next->next->rec.code == TOK_NUMBER || tok->next->next->next->rec.code == TOK_VAR)){
                if(tok->next->next->rec.code == TOK_SUB){
                    if (tok->next->next->next->rec.code == TOK_NUMBER) tok->next->next->next->dval *= -1;
                    else tok->next->next->next->flags |= D2_TOKF_NEGATIVE;//for VARIABLE
                }
                __d2_skip_tok(ctx, tok->next->next);
			}
		}
	}

	return E13_OK;
}

e13_t d2_tokenize(struct d2_ctx *ctx)
{

	struct d2_tok *tok;
	char *start, *end;	

	//TODO: this is so dirty! do not terminate the buffer and the entire pricess terminates! (SEGV)
	dm_tok4("strlen ctx->buf = %li - (%c)\n", strlen(ctx->buf), ctx->buf[strlen(ctx->buf)-1]);
	if(ctx->buf[strlen(ctx->buf)-1] != ';') return e13_error(E13_FORMAT);

	ctx->ntoks =
		__d2_estimate_ntokens(ctx->buf, d2_delimlist, d2_escape, d2_pack1,
				  d2_pack2);

	dm_tok2("esttok = %lu\n", ctx->ntoks);

	ctx->tok_databuf_poolsize =
		__d2_get_tok_databuf_poolsize(ctx->buf, ctx->ntoks);

	if (__d2_alloc_tok_list(ctx, ctx->ntoks) != E13_OK)
		return e13_error(E13_NOMEM);

	if (__d2_alloc_tok_databuf_pool(ctx, ctx->tok_databuf_poolsize) !=
		E13_OK)
		return e13_error(E13_NOMEM);

	//some init before the loop     
	start = ctx->buf;
	end = start;

//defines to speed up things
#define len (end - start + 1)

	while (*end) {
		end =
			__d2_token(start, d2_delimlist, d2_escape, d2_pack1,
				   d2_pack2);
		dm_tok2("start = %s, end = %s(%c)\n", start, end, end[0]);
		if (*end && !isspace(*end)) {

			//init tok ptr
			//tok = &toks[(ctx->ntok)++];                   

			tok = __d2_enumset_tok_buf(ctx, start, len);            

			//phase b, lexical analysis 1 (2 is done via combine())
			d2_lex(ctx, tok);

		}		//if(*end && !isspace(*end))

		start = end + 1;	//out of bounds

	}

	__d2_free_unused_tok_list(ctx, tok); 

#undef len

	return E13_OK;
}
