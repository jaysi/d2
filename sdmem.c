//static - dynamic memmory manager for allocating many small objects in a single buffer
#include "error13.h"
#include "d2.h"
#include "token.h"

struct sdmem_node {
	size_t size;
	char *buf;
	struct sdmem_node *next;
};

struct sdmem_s {
	size_t n;
	size_t size;
	char *buf;

};

int sdmem_init(struct sdmem_s *ctl, char *buf, size_t size, size_t n)
{
	return e13_error(E13_IMPLEMENT);
}

/* *
 *
 * TOKEN DATA ALLOCATION
 * NEEDS:
 *  1. token count estimation
 *  2. token allocation pool
 *  3. token data allocation pool
 * 
 * */

size_t __d2_get_tok_databuf_poolsize(char *buf, size_t ntok)
{
	return strlen(buf) + (ntok) + 1;
}

e13_t __d2_delete_tok_list(struct d2_ctx *ctx, int free_databuf)
{

	struct d2_tok *tok;

	tok = ctx->tok_list_first;
	while (tok) {

		if (free_databuf) {
			free(tok->rec.data);
		}
		ctx->tok_list_first = ctx->tok_list_first->next;
		free(tok);
		tok = ctx->tok_list_first;

	}
	return E13_OK;
}

e13_t __d2_alloc_tok_list(struct d2_ctx *ctx, size_t ntok)
{

	struct d2_tok *tok;

	ctx->tok_list_first = NULL;

	for (size_t i = 0; i < ntok; i++) {
		tok = (struct d2_tok *)malloc(sizeof(struct d2_tok));
		if (!tok) {
			__d2_delete_tok_list(ctx, 0);
			return e13_error(E13_NOMEM);
		}
		tok->next = NULL;

		if (!ctx->tok_list_first) {
			tok->prev = NULL;
			ctx->tok_list_first = tok;
			ctx->tok_list_last = tok;
		} else {
			tok->prev = ctx->tok_list_last;
			ctx->tok_list_last->next = tok;
			ctx->tok_list_last = tok;
		}

	}

	ctx->tok_list_cur = NULL;

	return E13_OK;
}

e13_t __d2_alloc_tok_databuf_pool(struct d2_ctx *ctx, size_t bufsize)
{
	ctx->tok_databuf_pool = (char *)malloc(bufsize);
	if (!ctx->tok_databuf_pool)
		return e13_error(E13_NOMEM);
	return E13_OK;
}

struct d2_tok *__d2_enumset_tok_buf(struct d2_ctx *ctx, char *data,
				    size_t datasize)
{

	if (!ctx->tok_list_cur) {
		ctx->tok_list_cur = ctx->tok_list_first;
	} else
		ctx->tok_list_cur = ctx->tok_list_cur->next;

	assert(ctx->tok_list_cur);

	ctx->tok_list_cur->rec.data = (char *)malloc(datasize + 1);
	if (!ctx->tok_list_cur->rec.data)
		return NULL;

	memcpy(ctx->tok_list_cur->rec.data, data, datasize);
	*(ctx->tok_list_cur->rec.data + datasize) = 0;	//terminate buffer

	return ctx->tok_list_cur;
}

e13_t __d2_realloc_tok_buf(struct d2_tok *tok, char *data, size_t datasize,
			   int free_old_data)
{
	if (free_old_data) {
		free(tok->rec.data);
		tok->rec.data = (char *)malloc(datasize + 1);
	} else {
		tok->rec.data = (char *)realloc(tok->rec.data, datasize + 1);
	}
	if (tok->rec.data)
		strcpy(tok->rec.data, data);
	else
		return e13_error(E13_NOMEM);

	return E13_OK;

}

e13_t __d2_skip_tok(struct d2_ctx *ctx, struct d2_tok *tok)
{
	if (tok == ctx->tok_list_first) {
		ctx->tok_list_first = ctx->tok_list_first->next;
		if (ctx->tok_list_first)
			ctx->tok_list_first->prev = NULL;
	} else if (tok == ctx->tok_list_last) {
		ctx->tok_list_last = ctx->tok_list_last->prev;
		if (ctx->tok_list_last)
			ctx->tok_list_last->next = NULL;
	} else {
		tok->prev = tok->next;
	}
	free(tok->rec.data);
	free(tok);
	return E13_OK;
}

e13_t __d2_dup_tok(struct d2_tok** dst, struct d2_tok* src){
    *dst = (struct d2_tok*)malloc(sizeof(struct d2_tok));
    if(!(*dst)) return e13_error(E13_NOMEM);
    if(src->rec.data && strlen(src->rec.data)){
        (*dst)->rec.data = (char*)malloc(strlen(src->rec.data)+1);    
        if(!(*dst)->rec.data){ free(*dst); return e13_error(E13_NOMEM); }
        strcpy((*dst)->rec.data, src->rec.data);        
    } else (*dst)->rec.data = NULL;
    (*dst)->rec.code = src->rec.code;
    (*dst)->dval = src->dval;
    
    return E13_OK;
}

e13_t __d2_copy_tok(struct d2_tok* dst, struct d2_tok* src){
    
    if(src->rec.data && strlen(src->rec.data)){
        dst->rec.data = (char*)malloc(strlen(src->rec.data)+1);    
        if(!dst->rec.data){ return e13_error(E13_NOMEM); }
        strcpy(dst->rec.data, src->rec.data);
    } else dst->rec.data = NULL;
    dst->rec.code = src->rec.code;
    dst->dval = src->dval;
    
    return E13_OK;
}

e13_t __d2_add_ret(struct d2_ctx *ctx, struct d2_exp *exp)
{
	struct d2_ret *ret = (struct d2_ret *)malloc(sizeof(struct d2_ret));
	if (!ret)
		return e13_error(E13_NOMEM);
	ret->next = NULL;
	if (!ctx->ret_list_first) {
		ctx->ret_list_first = ret;
		ctx->ret_list_last = ret;
	} else {
		ctx->ret_list_last->next = ret;
		ctx->ret_list_last = ret;
	}

	//free(ret->tok.rec.data);//TODO:?
	assert(exp->stack_top->rec.data);    

    /*
	ret->tok.rec.data =
	    (char *)malloc(strlen(exp->stack_top->rec.data) + 1);
	strcpy(ret->tok.rec.data, exp->stack_top->rec.data);
	ret->tok.rec.code = exp->stack_top->rec.code;
	ret->tok.dval = exp->stack_top->dval;
    */

	return __d2_copy_tok(&(ret->tok), exp->stack_top);
}
