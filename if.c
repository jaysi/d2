#include <string.h>
#include <ctype.h>
#include "d2.h"
#include "if.h"

#define TEST_TOKENIZE

extern e13_t d2_init_ctx(struct d2_handle* h, struct d2_ctx* ctx, char* buf, size_t bufsize, char flags);
e13_t d2_rm_ctx(struct d2_handle* h, char* name);
e13_t d2_find_ctx(struct d2_handle* h, char* name);
e13_t d2_new_ctx(struct d2_handle* h, char* name);
e13_t d2_set_ctx_buf(struct d2_handle* h, char* name, char* buf, size_t bufsize, char flags);
e13_t d2_rst_ctx(struct d2_handle* h, char* name);
e13_t d2_run_ctx(struct d2_handle* h, char* name);

extern struct d2_tok* d2_blockize(struct d2_tok* first);
extern e13_t d2_tokenize(char* buf, size_t bufsize, struct d2_tok** toklist_first, size_t* ntok);
extern e13_t d2_combine(struct d2_tok* toklist_first);
extern e13_t d2_lex(struct d2_tok* tok);
extern e13_t d2_expize(struct d2_exp* parent, struct d2_tok* toklist_first, struct d2_exp** exps, size_t* nexp);
extern e13_t d2_infix2prefix(struct d2_exp* exp);
extern e13_t d2_run_pre(struct d2_ctx* ctx, struct d2_exp* exp);


e13_t d2_init_if(struct d2_handle* h, FILE* fin, FILE* fout){
    h->fin = fin;
    h->fout = fout;

    return E13_OK;
}

void d2_destroy_if(struct d2_handle* h){
    fclose(h->fin);
    fclose(h->fout);
}

#define d2_print(h, fmt, ...) fprintf(h->fout, fmt, __VA_ARGS__)
#define d2_emsg(h, fmt, ...) fprintf(h->ferr, fmt, __VA_ARGS__)
#define d2_gets(buf, bufsize, h) fgets(buf, bufsize, h->fin)

void d2_print_console_help(struct d2_handle* h){

    d2_print(h, "%s", "d2 interpreter, kernel %s - ui %s\n", D2_KERNEL_VER, D2_UI_VER);
    d2_print(h, "%s", "cui interface command list:\n");
    d2_print(h, "%s", "new\t\tnew context\n");
    d2_print(h, "%s", "ls\t\tlist contexts\n");
    d2_print(h, "%s", "rm\t\tremove context\n");
    d2_print(h, "%s", "set\t\tset context buffer\n");
    d2_print(h, "%s", "rst\t\treset context buffer\n");
    d2_print(h, "%s", "run\t\trun context\n"); 

}

void d2_print_ctx_list(struct d2_handle* h){
    struct d2_ctx* ctx;
    d2_print(h, "%s", "ctx list {\n");
    d2_lock_ctx(h);
    ctx = h->ctxlist_first;    
    while(ctx){
        d2_print(h, "%s\n", ctx->name);
        ctx = ctx->next;
    }    
    d2_unlock_ctx(h);
    d2_print(h, "%s", "} ctx list;\n");
}

void d2_console_if(struct d2_handle* h){

    char *buf;
    char *name;
    struct d2_ctx* ctx;

    buf = (char*)malloc(h->conf.max_ctx_buf);
    if(!buf){
        d2_emsg(h, "%s", "out of memmory\n");
        return;
    }
    name = (char*)malloc(h->conf.max_ctx_name);    
    if(!name){
        free(buf);
        d2_emsg(h, "%s", "out of memmory\n");
        return;
    }


    d2_print(h, "%s", "d2 console interactive interface, type ? for help\n");

    while(1){
        d2_print(h, "%s", "> ");
        d2_gets(buf, MAX_IN_BUF, h);

        if(!strcmp(buf, "?")){
            d2_print_console_help(h);
        }

        if(!strcmp(buf, "new")){
            d2_print(h, "%s", "name> ");
            d2_gets(name, h->conf.max_ctx_name, h);            
            switch(d2_new_ctx(h, name)){
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
        }

        if(!strcmp(buf, "ls")){
            d2_print_ctx_list(h);
        }

        if(!strcmp(buf, "rm")){

            d2_print(h, "%s", "name> ");
            d2_gets(name, h->conf.max_ctx_name, h);            
            switch(d2_rm_ctx(h, name)){
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
        }

        if(!strcmp(buf, "set")){

            d2_print(h, "%s", "name> ");
            d2_gets(name, h->conf.max_ctx_name, h);
            d2_print(h, "%s", "src> ");
            d2_gets(buf, h->conf.max_ctx_buf, h);
            switch(d2_set_ctx_buf(h, name, buf, strlen(buf)+1, D2_CTXF_COPY_BUF)){
                case E13_OK:
                d2_print(h, "%s", "ok\n");
                break;
                case e13_error(E13_NOTFOUND):
                d2_emsg(h, "%s", "not found\n");
                break;
                case e13_error(E13_EXISTS):
                d2_emsg(h, "%s", "already exists, 'rst' first\n");
                break;
                default:
                d2_emsg(h, "%s", "system error\n");
                break;
            }
        }

        if(!strcmp(buf, "rst")){

            d2_print(h, "%s", "name> ");
            d2_gets(name, h->conf.max_ctx_name, h);            
            switch(d2_rst_ctx(h, name)){
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
        }

        if(!strcmp(buf, "run")){

            d2_print(h, "%s", "name> ");
            d2_gets(name, h->conf.max_ctx_name, h);
            switch(d2_run_ctx(h, name)){
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
        }


    }

}

#ifdef TEST_TOKENIZE

int test_tokenize(){
	char exp[100];
	struct d2_tok* toklist_first;//, *tok, *toklist_last;
	e13_t err;
	size_t ntok, nexp;
	struct d2_exp* exps;
    struct d2_ctx ctx;    

    d2_init_ctx(NULL, &ctx, exp, strlen(exp)+1, D2_CTXF_COPY_BUF);

    while(1) {

        printf("exp: ");
        fgets(exp, 100, stdin);

        //phase a and b
        err = d2_tokenize(exp, strlen(exp), &toklist_first, &ntok);

        if(err == E13_OK){
            
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

            if((err=d2_expize(NULL, toklist_first, &exps, &nexp)) == E13_OK){
                printf("nexp=%lu\n", nexp);
                /*
                for(ntok = 0; ntok < nexp; ntok++){
                    printf("#%lu: exp->begin: %s, exp->end: %s, exp->ntok = %lu\n",
                            ntok, exps[ntok].infix_tok_first->rec.data,
                            exps[ntok].infix_tok_last->rec.data, exps[ntok].ntok);
                }
                */		
            ctx.nexps = nexp;
            ctx.exps = exps;		
            } else {
                printf("expize! failed code: %i\n", err);
                return -1;
            }

            
            for(ntok = 0; ntok < nexp; ntok++){

                if(d2_infix2prefix(exps+ntok) == E13_OK){
                    /*
                    for(tok = exps[ntok].prefix_tok_first; tok; tok = tok->prefix_next){
                        printf("%s ", tok->rec.data);
                    }
                    printf("\n");
                    */
                    if(d2_run_pre(&ctx, exps+ntok) == E13_OK){
                        printf("result of #%lu is %Lf\n", ntok, exps[ntok].stack_top->dval);
                    }

                } else {
                    printf("infix2prefix fails here < %s >\n", exps[ntok].infix_tok_first->rec.data);
                }            
            }                    
            
        }

        d2_rst_ctx(&ctx);

    }

	return 0;
}

int test_tokenize2(){
	char exp[100], tk[20], *s, *e;
	printf("exp: ");
	fgets(exp, 100, stdin);
	s = exp;
	e = s;
	while(*e){
		e = __d2_token(s, d2_delimlist, d2_escape, d2_pack1, d2_pack2);
		if(!isspace(*e) && *e){
			memcpy(tk, s, e-s+1);
			tk[e-s+1] = '\0';
			printf("-> (%i) %s\n", (int)tk[0],tk);			
		}//if(*e)
		s = e+1;//out of bounds
	}
	//printf("exp: %p, s: %p, e: %p, diff: %tp, sizeof(mem): %u\n", exp, s, e, (ptrdiff_t)(e-exp), __d2_estimate_ntokens(exp, d2_delimlist, d2_escape, d2_pack1, d2_pack2));
	return 0;
}


#endif//TEST_TOKENIZE