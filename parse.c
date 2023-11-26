#include "d2.h"
#include "error13.h"

#define dm_exp1(fmt, ...)	//fprintf(stderr, fmt, __VA_ARGS__)

struct d2_tok *d2_blockize(struct d2_tok *first)
{
	struct d2_tok *tok = first;

	while (tok) {
		switch (tok->rec.code) {
		case TOK_BLOCK_OPEN:
			tok = d2_blockize(tok->next);
			break;
		case TOK_BLOCK_CLOSE:
			first->blockend = tok->prev;
			return tok->next;	//
			break;
		default:
			tok = tok->next;
			break;
		}
	}
	return NULL;
}

size_t __d2_nexp(struct d2_tok *toklist_first)
{
	struct d2_tok *tok;
	size_t nexp = 0UL;
	tok = toklist_first;
	dm_exp1("tokfirst = %s, nexp = %lu\n", tok ? tok->rec.data : "NULL",
		nexp);
	while (tok) {
		switch (tok->rec.code) {
		case TOK_SEMICOLON:
		case TOK_LABEL:
			nexp++;
			dm_exp1("got exp @ %p, data = %s, nexp = %lu\n",
				(void *)tok, tok->rec.data, nexp);
			break;
		default:
			//if(tok->blockend) {tok = tok->blockend; nexp++;}
			dm_exp1("no exp @ %s, nexp = %lu\n", tok->rec.data,
				nexp);
			break;
		}
		tok = tok->next;
	}

	return nexp;
}

e13_t d2_expize(struct d2_exp *parent, struct d2_tok *toklist_first,
		struct d2_exp **exps, size_t *nexp)
{
	struct d2_tok *tok;
	size_t nexp_est = __d2_nexp(toklist_first);
	if (!nexp_est)
		return e13_error(E13_EMPTY);

	*exps = (struct d2_exp *)malloc(nexp_est * sizeof(struct d2_exp));
	if (!(*exps))
		return e13_error(E13_NOMEM);

	tok = toklist_first;
	*nexp = 0UL;
	(*exps)[*nexp].infix_tok_first = tok;
	(*exps)[*nexp].ntok = 1UL;
	while (tok) {
		switch (tok->rec.code) {
		case TOK_SEMICOLON:
		case TOK_LABEL:    
			(*exps)[*nexp].infix_tok_last = tok;
      if(tok->next){
			  assert(*nexp < nexp_est-1);
  			(*nexp)++;
	  		(*exps)[*nexp].infix_tok_first = tok->next;
		  	(*exps)[*nexp].ntok = 1UL;
      }
			break;
		default:
			/*           
			   if(tok->blockend) {
			   (*exps)[*nexp].infix_tok_last = tok->blockend;
			   //d2_expize(&(*exps)[*nexp], (*exps)[*nexp].infix_tok_first, )//become recursive!
			   assert(*nexp < nexp_est);
			   (*nexp)++;
			   tok = tok->blockend;
			   (*exps)[*nexp].infix_tok_first = tok->next;
			   (*exps)[*nexp].ntok = 1UL;
			   } else */ (*exps)[*nexp].ntok++;
			break;
		}
		if (!tok->next)
			(*exps)[*nexp].infix_tok_last = tok;
		tok = tok->next;
	}			//while(tok)

	return E13_OK;
}
