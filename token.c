#include <ctype.h>
#include <stdio.h>
#include <errno.h>
#include "lib13.h"
#include "token.h"
#include "d2.h"
#include "limmit.h"
#include "dmsg.h"

#define dm_loc() fprintf(stderr, "loc: %s.%s().%i", __FILE__, __FUNC__, __LINE__)
#define dm _dm
#define dm_tok1(fmt, ...)	//fprintf(stderr, fmt, __VA_ARGS__)
#define dm_tok2(fmt, ...)	//fprintf(stderr, fmt, __VA_ARGS__)
#define dm_tok3(fmt, ...)	//fprintf(stderr, fmt, __VA_ARGS__)

#ifdef __cplusplus
extern "C" {
#endif
//define mem/alltok_list_first = NULL;e
	e13_t __d2_skip_tok(struct d2_ctx *ctx, struct d2_tok *tok);
	e13_t __d2_realloc_tok_buf(struct d2_tok *tok, char *data,
				   size_t datasize, int free_old_data);
	struct d2_tok *__d2_enumset_tok_buf(struct d2_ctx *ctx, char *data,
					    size_t datasize);
	e13_t __d2_alloc_tok_databuf_pool(struct d2_ctx *ctx, size_t bufsize);
	e13_t __d2_alloc_tok_list(struct d2_ctx *ctx, size_t ntok);
	e13_t __d2_delete_tok_list(struct d2_ctx *ctx, int free_databuf);
	size_t __d2_get_tok_databuf_poolsize(char *buf, size_t ntok);
#ifdef __cplusplus
}
#endif
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

	//vars

	//fns

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
	size_t cnt = 0UL;
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

e13_t d2_lex(struct d2_tok *tok)
{
	d2_tok_enum tokenum;
	struct d2_tok_form_s *form;
	//struct d2_tok* tok;
	char *endptr;

	for (tokenum = TOK_EMPTY, form = d2_tok_form; form->form;
	     form++, tokenum++) {

		if (!strcmp(form->form, tok->rec.data)) {
			tok->rec.code = tokenum;
			break;
		}

	}

	if (!form->form) {

		//is digit? base10, hexa, scientific*
		tok->dval = strtold(tok->rec.data, &endptr);
		if (tok->dval == (long double)0 && endptr == tok->rec.data) {	//no conversion happened
			tok->rec.code = TOK_INVAL;
		} else
			tok->rec.code = TOK_NUMBER;
	}

	return E13_OK;
}

e13_t d2_combine(struct d2_ctx *ctx)
{
	struct d2_tok *tok, *toktmp, *toklist_first = ctx->tok_list_first;
	int j;
	char sci_num[D2_MAX_SCI_NUM];

	//1. resolve combo operators
	for (int i = 3; i > 1; i--) {
		for (tok = toklist_first; tok; tok = tok->next) {
			for (d2_tok_enum tok_enum = TOK_EMPTY;
			     d2_tok_form[tok_enum].form; tok_enum++) {
				if (strlen(d2_tok_form[tok_enum].form) == i) {
					toktmp = tok;

					for (j = 0; j < i; j++) {
						if (toktmp && toktmp->rec.data
						    && toktmp->rec.data[0] ==
						    d2_tok_form[tok_enum].form
						    [j])
							toktmp = toktmp->next;
						else
							break;
					}

					if (j == i) {	//found a combination
						assert(tok);
						assert(toktmp);
						tok->rec.code = tok_enum;
						__d2_realloc_tok_buf(tok,
								     d2_tok_form
								     [tok_enum].form,
								     strlen
								     (d2_tok_form
								      [tok_enum].form),
								     0);
						__d2_skip_tok(ctx, toktmp);
					}

				}
			}
		}
	}			//resolve combo operators ends

	//resolve else if
	for (tok = toklist_first; tok; tok = tok->next) {
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
	if (toklist_first &&
	    toklist_first->rec.code == TOK_STRING &&
	    toklist_first->next &&
	    toklist_first->next->rec.code == TOK_LABEL &&
	    !toklist_first->next->next) {
		toklist_first->rec.code = TOK_LABEL;

		__d2_skip_tok(ctx, toklist_first->next);
	}

	//2. resolve scientific numbers
	for (tok = toklist_first; tok; tok = tok->next) {
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
				errno = 0;
				tok->dval = strtold(sci_num, NULL);
				if (!errno) {
					tok->rec.code = TOK_NUMBER;
					__d2_skip_tok(ctx, tok->next->next);
					__d2_skip_tok(ctx, tok->next);
				}
			}	//else if
		}

	}
	return E13_OK;
}

e13_t d2_tokenize(struct d2_ctx *ctx)
{

	struct d2_tok *tok;
	char *start, *end;
	struct d2_tok_form_s *form;

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

			form = d2_tok_form;
			for (d2_tok_enum tokenum = TOK_EMPTY; form->form;
			     form++, tokenum++) {

				dm_tok3("%s->%s\n", form->form, tok->rec.data);

				if (!strcmp(form->form, tok->rec.data)) {
					tok->rec.code = tokenum;
					break;
				} else
				    if ((tok->dval =
					 strtold(tok->rec.data, NULL))) {
					tok->rec.code = TOK_NUMBER;
					break;
				}
			}

			if (!form->form) {
				tok->rec.code = TOK_STRING;
			}

			dm_tok2("ntok = %li, data = %s, code = %i\n",
				ctx->ntoks, tok->rec.data, tok->rec.code);

		}		//if(*end && !isspace(*end))

		start = end + 1;	//out of bounds

	}

#undef len

	return E13_OK;
}
