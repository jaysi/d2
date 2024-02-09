#include <string.h>
#include <ctype.h>
#include "d2.h"
#include "if.h"
#include "lock.h"

#undef TEST_TOKENIZE

#ifndef NDEBUG
#define D2_COMPILE_FLAGS "Debug"
#else
#define D2_COMPILE_FLAGS "Release"
#endif

#ifdef __cplusplus
extern "C" {
#endif

	e13_t d2_init_ctx(struct d2_handle *h, struct d2_ctx *ctx, char *buf,
			  size_t bufsize, char flags);
	e13_t d2_rm_ctx(struct d2_handle *h, char *name);
	e13_t d2_find_ctx(struct d2_handle *h, char *name);
	e13_t d2_new_ctx(struct d2_handle *h, char *name);
	e13_t d2_set_ctx_buf(struct d2_handle *h, char *name, char *buf,
			     size_t bufsize, char flags);
	e13_t d2_rst_ctx(struct d2_handle *h, char *name);
	e13_t d2_run_ctx(struct d2_handle *h, char *name);

	struct d2_tok *d2_blockize(struct d2_tok *first);
	e13_t d2_tokenize(char *buf, size_t bufsize,
			  struct d2_tok **toklist_first, size_t *ntok);
	e13_t d2_combine(struct d2_tok *toklist_first);
	e13_t d2_lex(struct d2_tok *tok);
	e13_t d2_expize(struct d2_ctx *ctx, struct d2_exp *parent);
	e13_t d2_infix2prefix(struct d2_exp *exp);
	e13_t d2_run_pre(struct d2_ctx *ctx, struct d2_exp *exp);

#ifdef __cplusplus
}
#endif
e13_t d2_init_if(struct d2_handle *h, FILE * fin, FILE * fout)
{
	h->fin = fin;
	h->fout = fout;

	return E13_OK;
}

void d2_destroy_if(struct d2_handle *h)
{
	fclose(h->fin);
	fclose(h->fout);
}

#define d2_print(h, fmt, ...) fprintf(h->fout, fmt, __VA_ARGS__)
#define d2_emsg(h, fmt, ...) fprintf(h->ferr, fmt, __VA_ARGS__)
#define d2_gets(buf, bufsize, h) {fgets(buf, bufsize, h->fin);buf[strlen(buf)-1]=0;}

void d2_print_console_help(struct d2_handle *h)
{

	d2_print(h,
		 "d2 interpreter, kernel %s - ui %s - compilation flags %s\n",
		 D2_KERNEL_VER, D2_UI_VER, D2_COMPILE_FLAGS);
	d2_print(h, "%s", "cui interface command list:\n");
	d2_print(h, "%s", "c <name> <buffer>\tdo the complete cycle!\n");
	d2_print(h, "%s", "n <name>\t\tnew context\n");
	d2_print(h, "%s", "l\t\t\tlist contexts\n");
	d2_print(h, "%s", "d <name>\t\tdelete context\n");
	d2_print(h, "%s", "s <name> <buffer>\tset context buffer\n");
	d2_print(h, "%s", "r <name>\t\treset context buffer\n");
	d2_print(h, "%s", "u <name>\t\trun context\n");
	d2_print(h, "%s", "p <name>\t\tprint context results\n");
	d2_print(h, "%s", "b <name>\t\tprint context buffer\n");
	d2_print(h, "%s", "q\t\t\tquit\n");

}

void d2_print_ctx_list(struct d2_handle *h)
{
	struct d2_ctx *ctx;
	d2_print(h, "%s", "ctx list {\n");
	d2_lock_ctx(h);
	ctx = h->ctxlist_first;
	while (ctx) {
		d2_print(h, "\t%s\n", ctx->name);
		ctx = ctx->next;
	}
	d2_unlock_ctx(h);
	d2_print(h, "%s", "} ctx list;\n");
}

void __d2_print_ctx(struct d2_handle *h, char *arg1)
{
	struct d2_var *var;
	struct d2_ret *ret;
	d2_lock_ctx(h);
	struct d2_ctx *ctx = h->ctxlist_first;
	while (ctx) {
		if (!strcmp(ctx->name, arg1)) {
			assert(ctx);
			assert(ctx->ret_list_first);

			ret = ctx->ret_list_first;
			while (ret) {
				switch (ctx->ret_list_first->tok.rec.code) {
				case TOK_NUMBER:
					d2_print(h, "return number: %Lf\n",
						 ret->tok.dval);
					break;
				case TOK_STRING:
					d2_print(h, "return string: %s\n",
						 ret->tok.rec.data);
					break;
				case TOK_VAR:
					d2_print(h, "return var: %s = %Lf\n",
						 ret->tok.rec.data,
						 ret->tok.dval);
					break;
				default:
					d2_print(h,
						 "return unknown type: %s, %Lf\n",
						 ret->tok.rec.data,
						 ret->tok.dval);
					break;
				}
				ret = ret->next;
			}	//while(ret)

			var = ctx->var_list_first;
			d2_print(h, "%s\n", "var{");
			while (var) {
				d2_print(h, "\t%s -> %Lf\n", var->name,
					 var->val);
				var = var->next;
			}
			d2_print(h, "%s\n", "}var;");

			break;
		}
		ctx = ctx->next;
	}
	if (!ctx)
		d2_print(h, "not found '%s', try 'ls'.\n", arg1);
	d2_unlock_ctx(h);

}

void d2_console_if(struct d2_handle *h)
{

	char *buf;
	struct d2_ctx *ctx;
	char *arg1, *arg2, *argtmp;
	e13_t er;

	//init input buffer for interface
	buf = (char *)malloc(h->conf.in_bufsize + 1);
	if (!buf) {
		d2_emsg(h, "%s", "out of memmory\n");
		return;
	}

	d2_print(h, "%s",
		 "d2 console interactive interface, try '?' for help\n");

	while (1) {
		d2_print(h, "%s", "> ");
		d2_gets(buf, h->conf.in_bufsize, h);

		if (!isspace(buf[1]) && strlen(buf) > 1) {
			d2_print(h, "%s\n", "bad format");
			continue;
		}

		arg1 = buf + 1;
		while (isspace(*(arg1)))
			arg1++;
		argtmp = arg1;
		while (*argtmp && !isspace(*(argtmp)))
			argtmp++;
		*argtmp = 0;
		arg2 = argtmp + 1;
		while (isspace(*(arg2)))
			arg2++;
		//no need to terminate arg2

		//d2_print(h, "arg1:%s, arg2:%s\n", arg1, arg2);

		switch (buf[0]) {

		case '?':
			d2_print_console_help(h);
			break;

		case 'c':
			//arg1 & arg2 are ok, checked that
			er = d2_new_ctx(h, arg1);
			if (er != E13_OK) {
				d2_print(h,
					 "failed to create new context '%s' (code=%i)\n",
					 arg1, er);
				break;
			}
			er = d2_rst_ctx(h, arg1);
			if (er != E13_OK) {
				d2_print(h,
					 "failed to reset context '%s' (code=%i)\n",
					 arg1, er);
				break;
			}
			er = d2_set_ctx_buf(h, arg1, arg2, strlen(arg2) + 1,
					    D2_CTXF_COPY_BUF);
			if (er != E13_OK) {
				d2_print(h,
					 "failed to set context buffer '%s' to '%s' (code=%i)\n",
					 arg1, arg2, er);
				break;
			}
			er = d2_run_ctx(h, arg1);
			if (er != E13_OK) {
				d2_print(h,
					 "failed to run context '%s' (code=%i)\n",
					 arg1, er);
				break;
			}
			__d2_print_ctx(h, arg1);
			break;

		case 'n':
			switch (d2_new_ctx(h, arg1)) {
			case E13_OK:
				d2_print(h, "%s", "ok\n");
				break;
			case e13_error(E13_EXISTS):
				d2_emsg(h, "%s", "already exists\n");
				break;
			default:
				d2_emsg(h, "%s", "system error\n");
				break;
			}
			break;

		case 'l':
			d2_print_ctx_list(h);
			break;

		case 'd':
			switch (d2_rm_ctx(h, arg1)) {
			case E13_OK:
				d2_print(h, "%s", "ok\n");
				break;
			case e13_error(E13_NOTFOUND):
				d2_emsg(h, "%s", "not found\n");
				break;
			default:
				d2_emsg(h, "%s", "system error\n");
				break;
			}
			break;

		case 's':
			switch (d2_set_ctx_buf
				(h, arg1, arg2, strlen(arg2) + 1,
				 D2_CTXF_COPY_BUF)) {
			case E13_OK:
				d2_print(h, "%s", "ok\n");
				break;
			case e13_error(E13_NOTFOUND):
				d2_emsg(h, "%s", "not found\n");
				break;
			case e13_error(E13_EXISTS):
				d2_emsg(h, "%s",
					"already exists, 'rst' first\n");
				break;
			default:
				d2_emsg(h, "%s", "system error\n");
				break;
			}
			break;

		case 'r':
			switch (d2_rst_ctx(h, arg1)) {
			case E13_OK:
				d2_print(h, "%s", "ok\n");
				break;
			case e13_error(E13_NOTFOUND):
				d2_emsg(h, "%s", "not found\n");
				break;
			default:
				d2_emsg(h, "%s", "system error\n");
				break;
			}
			break;

		case 'u':
			switch (er = d2_run_ctx(h, arg1)) {
			case E13_OK:
				d2_print(h, "%s",
					 "ok, next try 'p' to print results\n");
				break;
			case e13_error(E13_NOTFOUND):
				d2_emsg(h, "%s", "not found\n");
				break;
			default:
				d2_emsg(h, "system error (code = %i)\n", er);
				break;
			}
			break;

		case 'p':
			__d2_print_ctx(h, arg1);
			break;

		case 'b':
			d2_lock_ctx(h);
			ctx = h->ctxlist_first;
			while (ctx) {
				if (!strcmp(ctx->name, arg1)) {
					d2_print(h, "buffer:\n%s\n",
						 ctx->buf ? ctx->buf : "EMPTY");
					break;
				}
				ctx = ctx->next;
			}
			if (!ctx)
				d2_print(h, "not found '%s', try 'ls'\n", arg1);
			d2_unlock_ctx(h);
			break;

		case 'q':
			return;
			break;

		default:
			d2_print(h, "bad command '%c', try '?'\n", buf[0]);
			break;

		}
	}

}

#ifdef TEST_TOKENIZE

int test_tokenize()
{
	char exp[100];
	struct d2_tok *toklist_first;	//, *tok, *toklist_last;
	e13_t err;
	struct d2_ctx ctx;

	d2_init_ctx(NULL, &ctx, exp, strlen(exp) + 1, D2_CTXF_COPY_BUF);

	while (1) {

		printf("exp: ");
		fgets(exp, 100, stdin);

		//phase a and b
		err = d2_tokenize(exp, strlen(exp), &toklist_first, &ntok);

		if (err == E13_OK) {

			//phase c and b1
			d2_combine(toklist_first);

			d2_blockize(toklist_first);

			/*
			   for(tok = toklist_first; tok; tok = tok->next){
			   printf("%s -- %i", tok->rec.data, tok->rec.code);
			   tok->blockend?printf("{%s..%s}\n", tok->rec.data, tok->blockend->rec.data):printf("\n");
			   //if(!tok->next) toklist_last = tok;
			   }
			 */

			if ((err = d2_expize(&ctx, NULL) == E13_OK) {
			    printf("nexp=%lu\n", ctx.nexp);
			    /*
			       for(ntok = 0; ntok < nexp; ntok++){
			       printf("#%lu: exp->begin: %s, exp->end: %s, exp->ntok = %lu\n",
			       ntok, exps[ntok].infix_tok_first->rec.data,
			       exps[ntok].infix_tok_last->rec.data, exps[ntok].ntok);
			       }
			     */
			    }
			    else {
			    printf("expize! failed code: %i\n", err); return -1;}

			    for (ntok = 0; ntok < nexp; ntok++) {

			    if (d2_infix2prefix(exps + ntok) == E13_OK) {

			    for (tok = exps[ntok].prefix_tok_first;
				 tok; tok = tok->prefix_next) {
			    printf("%s ", tok->rec.data);}
			    printf("\n");
			    if (d2_run_pre(&ctx, exps + ntok) == E13_OK) {
			    printf
			    ("result of #%lu is %Lf\n",
			     ntok, exps[ntok].stack_top->dval);}

			    }
			    else {
			    printf
			    ("infix2prefix fails here < %s >\n",
			     exps[ntok].infix_tok_first->rec.data);}
			    }

			    }

			    d2_rst_ctx(&ctx);}

			    return 0;}

			    int test_tokenize2() {
			    char exp[100], tk[20], *s, *e;
			    printf("exp: ");
			    fgets(exp, 100, stdin); s = exp; e = s; while (*e) {
			    e =
			    __d2_token(s, d2_delimlist, d2_escape, d2_pack1,
				       d2_pack2); if (!isspace(*e) && *e) {
			    memcpy(tk, s, e - s + 1); tk[e - s + 1] = '\0'; printf("-> (%i) %s\n", (int)tk[0], tk);}	//if(*e)
			    s = e + 1;	//out of bounds
			    }
			    //printf("exp: %p, s: %p, e: %p, diff: %tp, sizeof(mem): %u\n", exp, s, e, (ptrdiff_t)(e-exp), __d2_estimate_ntokens(exp, d2_delimlist, d2_escape, d2_pack1, d2_pack2));
			    return 0;}

#endif				//TEST_TOKENIZE
