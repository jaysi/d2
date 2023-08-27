#include "error13.h"
#include "d2.h"

extern e13_t d2_tokenize(char* buf, size_t bufsize, struct d2_tok** toklist_first, size_t* ntok);
extern e13_t d2_combine(struct d2_tok* toklist_first);
extern e13_t d2_lex(struct d2_tok* tok);
extern struct d2_tok* d2_blockize(struct d2_tok* first);
extern e13_t d2_expize(struct d2_exp* parent, struct d2_tok* toklist_first, struct d2_exp** exps, size_t* nexp);
extern e13_t d2_run_pre(struct d2_ctx* ctx, struct d2_exp* exp);
extern e13_t d2_infix2prefix(struct d2_exp* exp);

e13_t d2_init_ctx(struct d2_handle* h, struct d2_ctx* ctx, char* buf, size_t bufsize, char flags){
    ctx->var_list_first = NULL;
    ctx->exps = NULL;
    ctx->toks = NULL;
    ctx->h = h;    

    ctx->bufsize = bufsize;
    ctx->flags = flags;

    if(flags & D2_CTXF_COPY_BUF){        
        ctx->buf = (char*)malloc(bufsize);        
    } else {
        ctx->buf = buf;        
    }

    return E13_OK;
}

void d2_rst_ctx(struct d2_ctx* ctx){    
    if(ctx->flags & D2_CTXF_COPY_BUF) free(ctx->buf);
    if(ctx->exps) free(ctx->exps);
    if(ctx->toks) free(ctx->toks);
    //this must be enough to start a new
    ctx->exps = NULL;    
}

void d2_rm_ctx(struct d2_ctx* ctx){
    struct d2_var* var;
    if(ctx->flags & D2_CTXF_COPY_BUF) free(ctx->buf);
    if(ctx->exps) free(ctx->exps);
    if(ctx->toks) free(ctx->toks);
    while(ctx->var_list_first){
        var = ctx->var_list_first;
        ctx->var_list_first = ctx->var_list_first->next;
        free(var->name);
        free(var);
    }
    //this must be enough to start a new
    ctx->exps = NULL;
}

e13_t d2_run_ctx(struct d2_ctx* ctx){

    size_t nexp, ntok;
    struct d2_tok* toks;
    struct d2_exp* exps;
    e13_t err;

    if(!ctx->exps){//not compiled

        if((err = d2_tokenize(ctx->buf, ctx->bufsize, &toks, &ntok)) == E13_OK){

            ctx->toks = toks;
            ctx->ntoks = ntok;
            
            //TODO: for now these two always return OK
            d2_combine(toks);

            d2_blockize(toks);

            if((err=d2_expize(NULL, toks, &exps, &nexp)) == E13_OK){
                ctx->nexps = nexp;
                ctx->exps = exps;
            } else return err;
        } else return err;

		for(ntok = 0; ntok < nexp; ntok++){
			if((err=d2_infix2prefix(exps+ntok)) != E13_OK) return err;
		}        

    }
    //now compiled, run!
    for(ntok = 0; ntok < nexp; ntok++){
            if((err=d2_run_pre(ctx, exps+ntok)) != E13_OK) return err;
    }

    return E13_OK;

}