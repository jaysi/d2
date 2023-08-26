#include "error13.h"
#include "d2.h"

//TODO: temporary, replace
#define d2_perr(handle, fmt, ...) fprintf(stderr, fmt, __VA_ARGS__)

extern e13_t __d2_pop_tok(struct d2_exp* exp, struct d2_tok** tok);
extern void __d2_push_tok(struct d2_exp* exp, struct d2_tok* tok);
e13_t d2_var_val(struct d2_ctx* ctx, char* name, long double* val);
e13_t d2_assign_var(struct d2_ctx* ctx, char* name, long double* val);

e13_t __d2_tok_val(struct d2_ctx* ctx, struct d2_tok* tok, long double* val){
    switch(tok->rec.code){
        case TOK_NUMBER:
            *val = tok->dval;
            return E13_OK;
        break;
        case TOK_VAR:
            return d2_var_val(ctx, tok->rec.data, val);
        break;
        default:
        break;
    }
    return e13_error(E13_NOTVALID);
}

e13_t d2_run_pre(struct d2_ctx* ctx, struct d2_exp* exp){
	struct d2_tok* enumtok, *poptok1, *poptok2;

	enumtok = exp->prefix_tok_first;

	while(enumtok){
		
		switch(enumtok->rec.code){

            case TOK_PLUS1:
            if(__d2_pop_tok(exp, &poptok1) != E13_OK){
				d2_perr(NULL, "%s\n", "syntax error");
				return e13_error(E13_SYNTAX);
            } else {
                poptok1->dval = poptok1->dval+1;
                __d2_push_tok(exp, poptok1);
            }
            break;

            case TOK_MINUS1:
            if(__d2_pop_tok(exp, &poptok1) != E13_OK){
				d2_perr(NULL, "%s\n", "syntax error");
				return e13_error(E13_SYNTAX);
            } else {
                poptok1->dval = poptok1->dval-1;
                __d2_push_tok(exp, poptok1);
            }
            break;

            case TOK_LOGIC_NOT:
            if(__d2_pop_tok(exp, &poptok1) != E13_OK){
				d2_perr(NULL, "%s\n", "syntax error");
				return e13_error(E13_SYNTAX);
            } else {
                poptok1->dval = !(poptok1->dval);
                __d2_push_tok(exp, poptok1);
            }
            break;

            case TOK_BIT_NOT:
            if(__d2_pop_tok(exp, &poptok1) != E13_OK){
				d2_perr(NULL, "%s\n", "syntax error");
				return e13_error(E13_SYNTAX);
            } else {
                poptok1->dval = (long double)(~(long long)(poptok1->dval));
                __d2_push_tok(exp, poptok1);
            }
            break;            

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

			case TOK_REMAIN:
			if(__d2_pop_tok(exp, &poptok1) != E13_OK || __d2_pop_tok(exp, &poptok2) != E13_OK){
				d2_perr(NULL, "%s\n", "syntax error");
				return e13_error(E13_SYNTAX);
			} else {
				poptok2->dval = poptok2->dval % poptok1->dval;
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