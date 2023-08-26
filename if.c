#include "d2.h"
#include "if.h"

e13_t d2_init_if(struct d2_handle* h, FILE* fin, FILE* fout){
    h->fin = fin;
    h->fout = fout;

    return E13_OK;
}

void d2_destroy_if(struct d2_handle* h){
    fclose(h->fin);
    fclose(h->fout);
}

e13_t d2_print(struct d2_handle* h, char* fmt, ...){

    return E13_OK;
}

#ifdef TEST_TOKENIZE

extern e13_t d2_init_ctx(struct d2_ctx* ctx);
extern struct d2_tok* d2_blockize(struct d2_tok* first);
extern e13_t d2_tokenize(char* buf, size_t bufsize, struct d2_tok** toklist_first, size_t* ntok);
extern e13_t d2_combine(struct d2_tok* toklist_first);
extern e13_t d2_lex(struct d2_tok* tok);
extern e13_t d2_expize(struct d2_exp* parent, struct d2_tok* toklist_first, struct d2_exp** exps, size_t* nexp);
extern e13_t d2_infix2prefix(struct d2_exp* exp);
extern e13_t d2_run_exp(struct d2_ctx* ctx, struct d2_exp* exp);

int test_tokenize(){
	char exp[100];
	struct d2_tok* toklist_first, *tok;//, *toklist_last;
	e13_t err;
	size_t ntok, nexp;
	struct d2_exp* exps;
	printf("exp: ");
	fgets(exp, 100, stdin);
    struct d2_ctx ctx;    

    d2_init_ctx(&ctx);

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
           ctx.nexps = nexps;
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
                if(d2_run_exp(exps+ntok) == E13_OK){
                    printf("result of #%lu is %Lf\n", ntok, exps[ntok].stack_top->dval);
                }

			} else {
				printf("infix2prefix fails here < %s >\n", exps[ntok].infix_tok_first->rec.data);
			}            
		}        

		free(exps);
		
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