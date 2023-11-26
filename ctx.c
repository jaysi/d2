#include <string.h>
#include <ctype.h>
#include "error13.h"
#include "d2.h"
#include "lock.h"

#ifdef __cplusplus
extern "C" {
#endif

	e13_t d2_tokenize(char *buf, size_t bufsize,
			  struct d2_tok **toklist_first, size_t *ntok);
	e13_t d2_combine(struct d2_tok *toklist_first);
	e13_t d2_lex(struct d2_tok *tok);
	struct d2_tok *d2_blockize(struct d2_tok *first);
	e13_t d2_expize(struct d2_exp *parent, struct d2_tok *toklist_first,
			struct d2_exp **exps, size_t *nexp);
	e13_t d2_run_pre(struct d2_ctx *ctx, struct d2_exp *exp);
	e13_t d2_infix2prefix(struct d2_exp *exp);

#ifdef __cplusplus
}
#endif

e13_t d2_rm_ctx(struct d2_handle *h, char *name)
{

	struct d2_ctx *ctx;
	struct d2_var *var;

	d2_lock_ctx(h);
	ctx = h->ctxlist_first;
	while (ctx) {
		if (!strcmp(ctx->name, name))
			break;
		ctx = ctx->next;
	}

	if (ctx) {

    if(ctx->flags & D2_CTXF_RUNNING){
      d2_unlock_ctx(h);
      return e13_error(E13_BUSY);
    }

		if (ctx == h->ctxlist_first) {
			h->ctxlist_first = h->ctxlist_first->next;
      if(h->ctxlist_first) h->ctxlist_first->prev = NULL;
		} else {
			ctx->prev->next = ctx->next;
		}

		d2_unlock_ctx(h);

		free(ctx->name);
		if (ctx->flags & D2_CTXF_COPY_BUF)
			free(ctx->buf);
		if (ctx->exps)
			free(ctx->exps);
		if (ctx->toks)
			free(ctx->toks);
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
  for(i = 0; i < strlen(name); i++){
    if(!isspace(name[i])) break;
  }
  if(i == strlen(name)) return e13_error(E13_FORMAT);

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
	ctx->var_list_first = NULL;
  ctx->buf = NULL;
	ctx->exps = NULL;
	ctx->toks = NULL;
	ctx->next = NULL;
  ctx->h = h;
	ctx->retlist_first.tok.rec.code = TOK_EMPTY;
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

	d2_lock_ctx(h);
	ctx = h->ctxlist_first;
	while (ctx) {
		if (!strcmp(ctx->name, name))
			break;
		ctx = ctx->next;
	}

	if (!ctx) {
		d2_unlock_ctx(h);
		return e13_error(E13_NOTFOUND);
	}

  if(ctx->flags & D2_CTXF_RUNNING){
    d2_unlock_ctx(h);
    return e13_error(E13_BUSY);
  }
	if (ctx->buf) {
		d2_unlock_ctx(h);
		return e13_error(E13_EXISTS);
	}

	ctx->bufsize = bufsize;

	if (flags & D2_CTXF_COPY_BUF) {
		ctx->buf = (char *)malloc(bufsize);
    if(!ctx->buf){
      d2_unlock_ctx(h);
      return e13_error(E13_NOMEM);
    }
    ctx->flags |= D2_CTXF_COPY_BUF;
		memcpy(ctx->buf, buf, bufsize);
	} else {
    ctx->flags &= ~D2_CTXF_COPY_BUF;
		ctx->buf = buf;
	}
	d2_unlock_ctx(h);

	return E13_OK;
}

e13_t d2_rst_ctx(struct d2_handle *h, char *name)
{
	struct d2_ctx *ctx;

	d2_lock_ctx(h);

	ctx = h->ctxlist_first;
	while (ctx) {
		if (!strcmp(ctx->name, name))
			break;
		ctx = ctx->next;
	}

	if (!ctx) {
		d2_unlock_ctx(h);
		return e13_error(E13_NOTFOUND);
	}

  if(ctx->flags & D2_CTXF_RUNNING){
    d2_unlock_ctx(h);
    return e13_error(E13_BUSY);
  }

	if (ctx->flags & D2_CTXF_COPY_BUF && ctx->buf)
		free(ctx->buf);
	if (ctx->exps)
		free(ctx->exps);
	if (ctx->toks)
		free(ctx->toks);
	//this must be enough to start a new
	ctx->exps = NULL;
	ctx->toks = NULL;
	ctx->buf = NULL;
  ctx->flags = D2_CTXF_INIT;
	ctx->retlist_first.tok.rec.code = TOK_EMPTY;

	d2_unlock_ctx(h);

	return E13_OK;
}

e13_t d2_run_ctx(struct d2_handle *h, char *name)
{

	struct d2_ctx *ctx;
	size_t nexp, ntok;
	struct d2_tok *toks;
	struct d2_exp *exps;
	e13_t err;

	d2_lock_ctx(h);
	ctx = h->ctxlist_first;
	while (ctx) {
		if (!strcmp(ctx->name, name))
			break;
		ctx = ctx->next;
	}
	if (!ctx) {
		d2_unlock_ctx(h);
		return e13_error(E13_NOTFOUND);
	}
  //check for running flag
  if(ctx->flags & D2_CTXF_RUNNING){
    d2_unlock_ctx(h);
    return e13_error(E13_BUSY);
  }
  //set running flag
  ctx->flags |= D2_CTXF_RUNNING;
  d2_unlock_ctx(h);

	if (!ctx->exps) {	//not compiled

		if ((err =
		     d2_tokenize(ctx->buf, ctx->bufsize, &toks,
				 &ntok)) == E13_OK) {

			ctx->toks = toks;
			ctx->ntoks = ntok;

			//TODO: for now these two always return OK
			d2_combine(toks);

			d2_blockize(toks);

			if ((err =
			     d2_expize(NULL, toks, &exps, &nexp)) == E13_OK) {
				ctx->nexps = nexp;
				ctx->exps = exps;
			} else
        d2_lock_ctx(h);
        ctx->flags &= ~D2_CTXF_RUNNING;
        d2_unlock_ctx(h);
				return err;
		} else {//did not tokenize well!
      d2_lock_ctx(h);
      ctx->flags &= ~D2_CTXF_RUNNING;
    	d2_unlock_ctx(h);
			return err;
		}

		for (ntok = 0; ntok < nexp; ntok++) {
			if ((err = d2_infix2prefix(exps + ntok)) != E13_OK) {
        d2_lock_ctx(h);
        ctx->flags &= ~D2_CTXF_RUNNING;
      	d2_unlock_ctx(h);
				return err;
			}
		}

	}//end of compile block

	//now compiled, run!
	for (ntok = 0; ntok < nexp; ntok++) {
		if ((err = d2_run_pre(ctx, exps + ntok)) != E13_OK) {
      d2_lock_ctx(h);
      ctx->flags &= ~D2_CTXF_RUNNING;
	    d2_unlock_ctx(h);
			return err;
		}
	}

	//TODO: ugliest thing!
	ctx->retlist_first.tok.rec.code = TOK_NUMBER;
	ctx->retlist_first.tok.dval = ctx->exps[nexp - 1].stack_top->dval;
	ctx->retlist_first.tok.rec.data =
	    ctx->exps[nexp - 1].stack_top->rec.data;

  d2_lock_ctx(h);
  ctx->flags &= ~D2_CTXF_RUNNING;
	d2_unlock_ctx(h);

	return E13_OK;

}
