#ifndef D2_H
#define D2_H

#include <assert.h>
#include <pthread.h>
#include <stdio.h>

#include "token.h"
#include "conf.h"

#define D2_KERNEL_VER "0.0.1-dev"
#define D2_UI_VER  "0.0.1-cui-dev"

#define MAIN_CTX_NAME   "main"

#ifndef MACRO
#define MACRO(A) do { A; } while(0)
#endif

struct d2_exp {

	char flags;
	char *infixbuf;
	size_t ntok;

	//views
	struct d2_tok *infix_tok_first, *infix_tok_last;
	struct d2_tok *prefix_tok_first, *prefix_tok_last;
	struct d2_tok *stack_top;
	//list
	struct d2_exp *next, *prev;
};

#define D2_VARTYPE_NUMBER   0x01
#define D2_VARTYPE_STRING   0x02

struct d2_ret {
	struct d2_tok tok;
	struct d2_ret *next;
};

struct d2_handle;

#define D2_CTXF_INIT        (0x00)
#define D2_CTXF_COPY_BUF    (0x01<<0)
#define D2_CTXF_LOCKED      (0x01<<1)
#define D2_CTXF_BUSY        (0x01<<2)

struct d2_ctx {

	char flags;
	size_t bufsize;
	char *buf;

	char *name;

	size_t tok_databuf_poolsize;	//size of the buffer below
	char *tok_databuf_pool;	//shared buffer of token data
	size_t ntoks;
	struct d2_tok *tok_list_first, *tok_list_last, *tok_list_cur;

	//return results    
	struct d2_ret *ret_list_first, *ret_list_last;

	size_t nexps;
	struct d2_exp *exps;	//dynamic array

	struct d2_var *var_list_first, *var_list_last;

	struct d2_ctx *next, *prev;

	struct d2_handle *h;

};

struct d2_handle {

	struct d2_conf conf;

	pthread_mutex_t ctxlist_mx;
	struct d2_ctx *ctxlist_first, *ctxlist_last, *ctx;

	pthread_mutex_t stream_mx;
	//io streams
	FILE *fin, *fout, *ferr;

};

#ifdef __cplusplus
extern "C" {
#endif
    /* ctx struct is defined here, we put export list here too! */
	e13_t d2_init_ctx(struct d2_handle *h, struct d2_ctx *ctx, char *buf,
			  size_t bufsize, char flags);
	e13_t d2_rm_ctx(struct d2_handle *h, char *name);
	e13_t d2_find_ctx(struct d2_handle *h, char *name);
	e13_t d2_new_ctx(struct d2_handle *h, char *name);
	e13_t d2_set_ctx_buf(struct d2_handle *h, char *name, char *buf,
			     size_t bufsize, char flags);
	e13_t d2_rst_ctx(struct d2_handle *h, char *name);
	e13_t d2_run_ctx(struct d2_handle *h, char *name);

    /* we put parser.c functions here too! */

	struct d2_tok *d2_blockize(struct d2_tok *first);
	e13_t d2_tokenize(struct d2_ctx* ctx);
	e13_t d2_combine(struct d2_ctx* ctx);
	e13_t d2_lex(struct d2_ctx* ctx, struct d2_tok *tok);
	e13_t d2_expize(struct d2_ctx *ctx, struct d2_exp *parent);
	e13_t d2_infix2prefix(struct d2_exp *exp);

    /* put this calc.c function here too! */    
    e13_t d2_run_pre(struct d2_ctx *ctx, struct d2_exp *exp);
    e13_t __d2_strtold(char* data, fnum_t* val);

#ifdef __cplusplus
}
#endif


#endif				//D2_H
