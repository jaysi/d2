#include <string.h>
#include <ctype.h>
#include "error13.h"
#include "d2.h"
#include "lock.h"
#include "dmsg.h"

#define __dm_run_ctx _dm

#ifdef __cplusplus
extern "C" {
#endif

	e13_t d2_tokenize(struct d2_ctx *ctx);
	e13_t d2_combine(struct d2_ctx *ctx);
	e13_t d2_lex(struct d2_tok *tok);
	struct d2_tok *d2_blockize(struct d2_tok *first);
	e13_t d2_expize(struct d2_ctx *ctx, struct d2_exp *parent);
	e13_t d2_run_pre(struct d2_ctx *ctx, struct d2_exp *exp);
	e13_t d2_infix2prefix(struct d2_exp *exp);

	e13_t __d2_delete_tok_list(struct d2_ctx *ctx, int free_databuf);
#ifdef __cplusplus
}
#endif
//do not forget unlock on successful return
    e13_t d2_get_ctx(struct d2_handle *h, char *name, struct d2_ctx **ctx)
{

	d2_lock_ctx(h);
	*ctx = h->ctxlist_first;
	while (*ctx) {
		if (!strcmp((*ctx)->name, name))
			break;
		*ctx = (*ctx)->next;
	}
	if (*ctx) {
		if ((*ctx)->flags & D2_CTXF_BUSY) {
			d2_unlock_ctx(h);
			return e13_error(E13_BUSY);
		}
		(*ctx)->flags |= D2_CTXF_BUSY;
		d2_unlock_ctx(h);
		return E13_OK;
	}
	d2_unlock_ctx(h);
	return e13_error(E13_NOTFOUND);
}

e13_t d2_unget_ctx(struct d2_handle *h, struct d2_ctx *ctx)
{
	d2_lock_ctx(h);
	if (!(ctx->flags & D2_CTXF_BUSY))
		return e13_error(E13_MISUSE);
	ctx->flags &= ~D2_CTXF_BUSY;
	d2_unlock_ctx(h);
	return E13_OK;
}

e13_t d2_rm_ctx(struct d2_handle *h, char *name)
{

	struct d2_ctx *ctx;
	struct d2_var *var;
	struct d2_ret *ret;

	d2_lock_ctx(h);
	ctx = h->ctxlist_first;
	while (ctx) {
		if (!strcmp(ctx->name, name))
			break;
		ctx = ctx->next;
	}

	if (ctx) {

		if (ctx->flags & D2_CTXF_BUSY) {
			d2_unlock_ctx(h);
			return e13_error(E13_BUSY);
		}

		if (ctx == h->ctxlist_first) {
			h->ctxlist_first = h->ctxlist_first->next;
			if (h->ctxlist_first)
				h->ctxlist_first->prev = NULL;
		} else {
			ctx->prev->next = ctx->next;
		}

		d2_unlock_ctx(h);

		free(ctx->name);

		while (ctx->ret_list_first) {
			ret = ctx->ret_list_first;
			ctx->ret_list_first = ctx->ret_list_first->next;
			free(ret);
		}

		if (ctx->flags & D2_CTXF_COPY_BUF)
			free(ctx->buf);
		if (ctx->exps)
			free(ctx->exps);
		__d2_delete_tok_list(ctx, 1);
		while (ctx->var_list_first) {
			var = ctx->var_list_first;
			ctx->var_list_first = ctx->var_list_first->next;
			free(var->name);
			free(var);
		}
		free(ctx);

		return E13_OK;

	} else
		d2_unlock_ctx(h);

	return e13_error(E13_NOTFOUND);

}

e13_t d2_find_ctx(struct d2_handle *h, char *name)
{

	struct d2_ctx *ctx;

	d2_lock_ctx(h);
	ctx = h->ctxlist_first;
	while (ctx) {
		if (!strcmp(ctx->name, name))
			break;
		ctx = ctx->next;
	}
	d2_unlock_ctx(h);

	return ctx ? E13_OK : e13_error(E13_NOTFOUND);

}

e13_t d2_new_ctx(struct d2_handle *h, char *name)
{
	int i;
	//avoid white space names
	for (i = 0; i < strlen(name); i++) {
		if (!isspace(name[i]))
			break;
	}
	if (i == strlen(name))
		return e13_error(E13_FORMAT);

	if (d2_find_ctx(h, name) == E13_OK)
		return e13_error(E13_EXISTS);
	struct d2_ctx *ctx = (struct d2_ctx *)malloc(sizeof(struct d2_ctx));
	if (!ctx)
		return e13_error(E13_NOMEM);

	ctx->name = (char *)malloc(strlen(name) + 1);
	if (!ctx->name) {
		free(ctx);
		return e13_error(E13_NOMEM);
	}

	ctx->flags = D2_CTXF_INIT;
	ctx->ret_list_first = NULL;
	ctx->var_list_first = NULL;
	ctx->buf = NULL;
	ctx->exps = NULL;
	ctx->tok_list_first = NULL;
	ctx->next = NULL;
	ctx->h = h;
	strcpy(ctx->name, name);

	d2_lock_ctx(h);
	if (!h->ctxlist_first) {
		ctx->prev = NULL;
		h->ctxlist_first = ctx;
		h->ctxlist_last = ctx;
	} else {
		ctx->prev = h->ctxlist_last;
		h->ctxlist_last->next = ctx;
		h->ctxlist_last = ctx;
	}
	d2_unlock_ctx(h);

	return E13_OK;

}

e13_t d2_set_ctx_buf(struct d2_handle *h, char *name, char *buf, size_t bufsize,
		     char flags)
{

	struct d2_ctx *ctx;
	e13_t er;

	if ((er = d2_get_ctx(h, name, &ctx) != E13_OK))
		return er;

	if (ctx->buf) {
		d2_unget_ctx(h, ctx);
		return e13_error(E13_EXISTS);
	}

	ctx->bufsize = bufsize;

	if (flags & D2_CTXF_COPY_BUF) {
		ctx->buf = (char *)malloc(bufsize);
		if (!ctx->buf) {
			d2_unget_ctx(h, ctx);
			return e13_error(E13_NOMEM);
		}
		ctx->flags |= D2_CTXF_COPY_BUF;
		memcpy(ctx->buf, buf, bufsize);
	} else {
		ctx->flags &= ~D2_CTXF_COPY_BUF;
		ctx->buf = buf;
	}
	d2_unget_ctx(h, ctx);

	return E13_OK;
}

e13_t d2_rst_ctx(struct d2_handle *h, char *name)
{
	struct d2_ctx *ctx;
	struct d2_ret *ret;
	e13_t er;

	if ((er = d2_get_ctx(h, name, &ctx)) != E13_OK)
		return er;

	if (ctx->flags & D2_CTXF_COPY_BUF && ctx->buf)
		free(ctx->buf);
	if (ctx->exps)
		free(ctx->exps);
	__d2_delete_tok_list(ctx, 1);
	//this must be enough to start a new
	ctx->exps = NULL;
	ctx->tok_list_first = NULL;
	ctx->buf = NULL;
	while (ctx->ret_list_first) {
		ret = ctx->ret_list_first;
		ctx->ret_list_first = ctx->ret_list_first->next;
		free(ret);
	}

	//since i gonna touch ctx flag, i will not use unget_ctx func
	d2_lock_ctx(h);
	ctx->flags = D2_CTXF_INIT;
	d2_unlock_ctx(h);

	return E13_OK;
}

e13_t d2_add_ctx_ret(struct d2_handle *h, char *name, struct d2_tok *tok)
{
	struct d2_ctx *ctx;
	struct d2_ret *ret_list_last, *ret;
	e13_t er;
	ret = (struct d2_ret *)malloc(sizeof(struct d2_ret));
	if (!ret)
		return e13_error(E13_NOMEM);
	memcpy(&ret->tok, tok, sizeof(struct d2_tok));
	ret->next = NULL;

	if ((er = d2_get_ctx(h, name, &ctx)) != E13_OK)
		return er;
	if (!ctx->ret_list_first) {
		ctx->ret_list_first = ret;
	} else {
		ret_list_last = ctx->ret_list_first;
		while (ret_list_last->next) {
			ret_list_last = ret_list_last->next;
		}
		ret_list_last->next = ret;
	}
	return E13_OK;
}

e13_t d2_run_ctx(struct d2_handle *h, char *name)
{

	struct d2_ctx *ctx;
	size_t ntok;
	e13_t err;

	if ((err = d2_get_ctx(h, name, &ctx)) != E13_OK)
		return err;

	if (!ctx->exps) {	//not compiled

		if ((err = d2_tokenize(ctx)) == E13_OK) {	//assume ctx->buf is already set

			//TODO: for now these two always return OK
			d2_combine(ctx);

			d2_blockize(ctx->tok_list_first);

			if ((err = d2_expize(ctx, NULL)) == E13_OK) {
				__dm_run_ctx("%s %s\n", "run ctx: ", ctx->name);
			} else {
				d2_unget_ctx(h, ctx);
				return err;
			}
		} else {	//did not tokenize well!
			d2_unget_ctx(h, ctx);
			return err;
		}

		for (ntok = 0; ntok < ctx->nexps; ntok++) {
			if ((err = d2_infix2prefix(ctx->exps + ntok)) != E13_OK) {
				d2_unget_ctx(h, ctx);
				return err;
			}
		}

	}			//end of compile block

	//now compiled, run!
	for (ntok = 0; ntok < ctx->nexps; ntok++) {
		if ((err = d2_run_pre(ctx, ctx->exps + ntok) != E13_OK)) {
			d2_unget_ctx(h, ctx);
			return err;
		}
	}

	d2_unget_ctx(h, ctx);

	__dm_run_ctx("%s, exp: %li\n", "run ctx", ctx->nexps);

	return ctx->nexps ? E13_OK : e13_error(E13_EMPTY);

}
