#include "error13.h"
#include "d2.h"

e13_t d2_init_ctx(struct d2_ctx* ctx, char* buf, size_t bufsize, FILE* fin, FILE* fout, char flags){
    ctx->var_list_first = NULL;
    ctx->exps = NULL;
    ctx->toks = NULL;

    ctx->bufsize = bufsize;
    ctx->flags = flags;

    ctx->fin = fin;
    ctx->fout = fout;

    if(flags & D2_CTXF_COPY_BUF){        
        ctx->buf = (char*)malloc(bufsize);        
    } else {
        ctx->buf = buf;        
    }

    return E13_OK;
}

void d2_destroy_ctx(struct d2_ctx* ctx){
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
}

e13_t d2_run_ctx(struct d2_ctx* ctx){

    size_t nexp, ntok;
    struct d2_tok* toks;
    struct d2_exp exp, *exps;

    if(!ctx->exps){//not compiled

        if((err = d2_tokenize(ctx->buf, ctx->bufsize, &toks, &ntok)) == E13_OK){

            ctx->toks = toks;
            ctx->ntok = ntok;
            
            //TODO: for now these two always return OK
            d2_combine(toks);

            d2_blockize(toks);

            if((err=d2_expize(NULL, toks, &exps, &nexp)) == E13_OK){
                ctx->nexps = nexps;
                ctx->exps = exps;
            } else return err;
        } else return ret;

		for(ntok = 0; ntok < nexp; ntok++){
			if((ret=d2_infix2prefix(exps+ntok)) != E13_OK) return ret;
		}        

    }
    //now compiled, run!
    for(ntok = 0; ntok < nexp; ntok++){
            if((ret=d2_run_exp(exps+ntok)) != E13_OK) return ret;
    }

}