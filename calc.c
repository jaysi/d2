#include "error13.h"
#include "d2.h"

//TODO: temporary, replace
#define d2_perr(handle, fmt, ...) fprintf(stderr, fmt, __VA_ARGS__)

extern e13_t __d2_pop_tok(struct d2_exp* exp, struct d2_tok** tok);
extern void __d2_push_tok(struct d2_exp* exp, struct d2_tok* tok);

e13_t d2_run_exp(struct d2_exp* exp){
	struct d2_tok* enumtok, *poptok1, *poptok2;

	enumtok = exp->prefix_tok_first;

	while(enumtok){
		
		switch(enumtok->rec.code){

			case TOK_MULT:
			if(__d2_pop_tok(exp, &poptok1) != E13_OK || __d2_pop_tok(exp, &poptok2) != E13_OK){
				d2_perr(NULL, "%s\n", "syntax error");
				return e13_error(E13_SYNTAX);
			} else {
				poptok2->dval = poptok2->dval * poptok1->dval;
				poptok2->rec.code = TOK_NUMBER;
				__d2_push_tok(exp, poptok2);
			}
			break;

			case TOK_DIV:
			if(__d2_pop_tok(exp, &poptok1) != E13_OK || __d2_pop_tok(exp, &poptok2) != E13_OK){
				d2_perr(NULL, "%s\n", "syntax error");
				return e13_error(E13_SYNTAX);
			} else {
				poptok2->dval = poptok2->dval / poptok1->dval;
				poptok2->rec.code = TOK_NUMBER;
				__d2_push_tok(exp, poptok2);
			}
			break;

			case TOK_ADD:
			if(__d2_pop_tok(exp, &poptok1) != E13_OK || __d2_pop_tok(exp, &poptok2) != E13_OK){
				d2_perr(NULL, "%s\n", "syntax error");
				return e13_error(E13_SYNTAX);
			} else {
				poptok2->dval = poptok2->dval + poptok1->dval;
				poptok2->rec.code = TOK_NUMBER;
				__d2_push_tok(exp, poptok2);
			}
			break;

			case TOK_SUB:
			if(__d2_pop_tok(exp, &poptok1) != E13_OK || __d2_pop_tok(exp, &poptok2) != E13_OK){
				d2_perr(NULL, "%s\n", "syntax error");
				return e13_error(E13_SYNTAX);
			} else {
				poptok2->dval = poptok2->dval - poptok1->dval;
				poptok2->rec.code = TOK_NUMBER;
				__d2_push_tok(exp, poptok2);
			}
			break;

			case TOK_NUMBER:
			__d2_push_tok(exp, enumtok);
			break;

			default:	
			//ignore		
			break;

		}

		enumtok = enumtok->prefix_next;
	}

	return E13_OK;
}