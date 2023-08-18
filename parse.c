#include "d2.h"
#include "error13.h"

size_t d2_nexp(struct d2_tok* toklist_first){
	struct d2_tok* tok;
	size_t nexp = 0UL;
	for(tok = toklist_first; tok; tok = tok->next){
		switch(tok->rec.code){
			case TOK_SEMICOLON:
			case TOK_LABEL:
			case TOK_BLOCK_CLOSE:
			nexp++;
			default:
			break;
		}
	}
	return nexp;
}

e13_t d2_expize(struct d2_tok* toklist_first, struct d2_exp** exps, size_t* nexp){
	struct d2_tok* tok;
	size_t nexp_est = d2_nexp(toklist_first);
	if(!nexp_est) return e13_error(E13_EMPTY);

	*exps = (struct d2_exp*)malloc(nexp_est*sizeof struct d2_exp);
	if(!(*exps)) return e13_error(E13_NOMEM);

	*nexp = 0UL;
	tok = toklist_first;
	(*exps)[*nexp].infix_tok_first = tok;
	while(tok){
		switch(tok->rec.code){
			case TOK_SEMICOLON:
			case TOK_LABEL:
			case TOK_BLOCK_CLOSE:
			(*exps)[*nexp].infix_tok_last = tok;
			assert(*nexp < nexp_est);
			(*nexp)++;
			(*exps)[*nexp].infix_tok_first = tok->next;
			break;
			default:
			break;
		}
		tok = tok->next;
	}
}