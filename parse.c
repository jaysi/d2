#include "d2.h"
#include "error13.h"

size_t d2_nexp(struct d2_tok* toklist_first){
	struct d2_tok* tok;    
	size_t nexp = toklist_first?1UL:0UL;
	tok = toklist_first;
	while(tok){
		switch(tok->rec.code){
			case TOK_SEMICOLON:
			case TOK_LABEL:			
			nexp++;
			default:
			if(tok->blockend) {tok = tok->blockend; nexp++;}
			break;
		}
		tok = tok->next;
	}
	return nexp;
}

e13_t d2_expize(struct d2_tok* toklist_first, struct d2_exp** exps, size_t* nexp){
	struct d2_tok* tok;
	size_t nexp_est = d2_nexp(toklist_first);
	if(!nexp_est) return e13_error(E13_EMPTY);

	*exps = (struct d2_exp*)malloc(nexp_est*sizeof(struct d2_exp));
	if(!(*exps)) return e13_error(E13_NOMEM);
	
	tok = toklist_first;
	*nexp = 0UL;
	(*exps)[*nexp].infix_tok_first = tok;    
	while(tok){
		switch(tok->rec.code){
			case TOK_SEMICOLON:
			case TOK_LABEL:			
			(*exps)[*nexp].infix_tok_last = tok;
			assert(*nexp < nexp_est);
			(*nexp)++;
			(*exps)[*nexp].infix_tok_first = tok->next;
			break;
			default:
			if(tok->blockend) {
				(*exps)[*nexp].infix_tok_last = tok->blockend;
				assert(*nexp < nexp_est);
				(*nexp)++;
				tok = tok->blockend;
				(*exps)[*nexp].infix_tok_first = tok->next;
			}
			break;
		}
		if(!tok->next) (*exps)[*nexp].infix_tok_last = tok;
		tok = tok->next;        
	}    
	(*nexp)++;

	return E13_OK;
}