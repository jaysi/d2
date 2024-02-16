#include "d2.h"
#include "error13.h"
#include "dmsg.h"

#define dm_pre1(fmt, ...)
#define dm_pre2(fmt, ...)
#define dm_pre_pop(fmt, ...)
#undef PRINT_PREFIX
extern int __d2_tok_preced(d2_tok_enum code);

#ifdef __cplusplus
extern "C" {
#endif
e13_t __d2_pop_tok(struct d2_exp *exp, struct d2_tok **tok);
e13_t
__d2_pop_2tok(struct d2_exp *exp, struct d2_tok **tok1, struct d2_tok **tok2);
void __d2_push_tok(struct d2_exp *exp, struct d2_tok *tok);
void __d2_appand_postfix(struct d2_exp *exp, struct d2_tok *tok);
#ifdef __cplusplus
}
#endif

void __d2_print_prefix(struct d2_exp *exp)
{
	struct d2_tok *tok = exp->prefix_tok_first;
	_dm_flat("%s", "*** prefix { ");
	while (tok) {
		_dm_flat("%s ", tok->rec.data);
		tok = tok->prefix_next;
	}
	_dm_flat("%s", " } prefix ***\n");
}

e13_t d2_infix2prefix(struct d2_exp *exp)
{
	struct d2_tok *enumtok, *poptok;
	exp->prefix_tok_first = NULL;
	enumtok = exp->infix_tok_last;
	exp->stack_top = NULL;
	while (enumtok && enumtok != exp->infix_tok_first->prev) {	//move backwards
		dm_pre1("enumtok->code: %i\n", enumtok->rec.code);
		dm_pre1("enumtok->data: %s\n", enumtok->rec.data);
		if (enumtok->rec.code < TOK_OO_LIMMIT) {	//OPERATOR, TOK_VAR IS THE LIMMIT BETWEEN OPERATOR/OPERAND
			switch (enumtok->rec.code) {
			case TOK_SEMICOLON:

				//ignore
				break;
			case TOK_PAREN_CLOSE:	//since i didn't invert paranthesis!
			case TOK_ARRAY_CLOSE:
				dm_pre2("%s", "push )\n");
				__d2_push_tok(exp, enumtok);
				break;
			case TOK_PAREN_OPEN:	//since i didn't invert paranthesis!
			case TOK_ARRAY_OPEN:
				while (__d2_pop_tok(exp, &poptok) == E13_OK && poptok->rec.code != enumtok->rec.code + 1) {	//the code + 1 should give the _CLOSE
					dm_pre2("( reached, append %s\n",
						poptok->rec.data);
					__d2_appand_postfix(exp, poptok);
				}
				break;
			default:
				while (exp->stack_top && __d2_tok_preced(enumtok->rec.code) >= __d2_tok_preced(exp->stack_top->rec.code)) {	//invert since my preceds are inverted!
					dm_pre2("E: %s <= S: %s\n",
						enumtok->rec.data,
						exp->stack_top->rec.data);
					if (__d2_pop_tok(exp, &poptok) ==
					    E13_OK) {
						dm_pre2("preced, append %s\n",
							poptok->rec.data);
						if (poptok->rec.code !=
						    TOK_PAREN_CLOSE)
							__d2_appand_postfix(exp, poptok);	//append array close, so i'll find out there is an array variable
					}

					else
						break;
				}
				dm_pre2("push %s\n", enumtok->rec.data);
				__d2_push_tok(exp, enumtok);
				break;
			}
		} else {	//OPERAND, append to the end of prefix
			dm_pre2("operand, append %s\n", enumtok->rec.data);
			__d2_appand_postfix(exp, enumtok);
		}
		enumtok = enumtok->prev;	//move backwards, TODO: FIX the block thing
	}

	//pop-append remaining stack
	while (__d2_pop_tok(exp, &poptok) == E13_OK)
		__d2_appand_postfix(exp, poptok);

#ifdef PRINT_PREFIX
	struct d2_tok *tok = exp->prefix_tok_first;
	_dm_flat("%s", "*** prefix { ");
	while (tok) {
		_dm_flat("%s ", tok->rec.data);
		tok = tok->prefix_next;
	}
	_dm_flat("%s", " } prefix ***\n");/*  */
#endif
    
	return E13_OK;

}
