#include "error13.h"
#include "include/d2.h"
#include "include/dmsg.h"
#include "include/var.h"
#include "include/sdmem.h"
#include <errno.h>

//TODO: temporary, replace
#define d2_perr(handle, fmt, ...)	fprintf(stderr, fmt, __VA_ARGS__)
#define __dm_calc _dm
#define __dm_def_calc _dm
#define __dm_enum(fmt, ...)

e13_t __d2_strtold(char* data, fnum_t* val){
	/*
	char const * const str = "blah";
	char const * endptr;

	int n = strtol(str, &endptr, 0);

	if (endptr == str) {  no conversion was performed  }

	else if (*endptr == '\0') { the entire string was converted }

	else {  the unconverted rest of the string starts at endptr }    
	*/
	char* endptr;
	errno = 0;
	*val = strtold(data, &endptr);
	if(errno || *endptr != '\0') return e13_error(E13_FORMAT);
	return E13_OK;
}

e13_t __d2_tok_val(struct d2_ctx *ctx, struct d2_tok *tok, long double *val)
{
	switch (tok->rec.code) {
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

void __d2_assign_tok_val(struct d2_ctx *ctx, struct d2_tok *tok,
			 long double val)
{
	tok->dval = val;
}

e13_t __d2_assert_2tok(d2_tok_enum tokid, struct d2_tok* tok1, struct d2_tok* tok2){
	switch(tokid){
		case TOK_PLUS1:
		case TOK_MINUS1:
		case TOK_LOGIC_NOT:
		case TOK_BIT_NOT:
			if(tok1->rec.code != TOK_NUMBER || tok1->rec.code != TOK_VAR)
				return e13_error(E13_FORMAT);
		break;
		case TOK_MULT:
		case TOK_DIV:
		case TOK_REMAIN:
		case TOK_ADD:
		case TOK_SUB:
		case TOK_BIT_SHIFT_LEFT:
		case TOK_BIT_SHIFT_RIGHT:
		case TOK_LT:
		case TOK_LE:
		case TOK_GT:
		case TOK_GE:
		case TOK_EQ:
		case TOK_NE:
		case TOK_BIT_AND:
		case TOK_BIT_XOR:
		case TOK_BIT_OR:
		case TOK_LOGIC_AND:
		case TOK_LOGIC_OR:
				
			if(	tok1->rec.code != TOK_NUMBER || tok1->rec.code != TOK_VAR ||
				tok2->rec.code != TOK_NUMBER || tok2->rec.code != TOK_VAR)
					return e13_error(E13_FORMAT);

		default:
		break;
	}
	return E13_OK;
}

e13_t d2_run_pre(struct d2_ctx *ctx, struct d2_exp *exp)
{
	struct d2_tok *enumtok, *poptok1, *poptok2;
	long double val1, val2;
	e13_t err;
  int has_ret = 1;

	enumtok = exp->prefix_tok_first;

	while (enumtok) {

		__dm_enum("enum: %s\n", enumtok->rec.data);

		//phase a. extract needed tokens
		switch(enumtok->rec.code){
			//cases need 1 token
			case TOK_PLUS1:
			case TOK_MINUS1:
			case TOK_LOGIC_NOT:
			case TOK_BIT_NOT:
			if (__d2_pop_tok(exp, &poptok1) != E13_OK) {
				d2_perr(NULL, "%s\n", "syntax error");
				return e13_error(E13_SYNTAX);
			} else {
				if ((err =
					 __d2_tok_val(ctx, poptok1,
						  &val1)) != E13_OK)
					return err;
			}
			break;

			//cases need 2 tokens			
			case TOK_MULT:
			case TOK_DIV:
			case TOK_REMAIN:
			case TOK_ADD:
			case TOK_SUB:
			case TOK_BIT_SHIFT_LEFT:
			case TOK_BIT_SHIFT_RIGHT:
			case TOK_LT:
			case TOK_LE:
			case TOK_GT:
			case TOK_GE:
			case TOK_EQ:
			case TOK_NE:
			case TOK_BIT_AND:
			case TOK_BIT_XOR:
			case TOK_BIT_OR:
			case TOK_LOGIC_AND:
			case TOK_LOGIC_OR:

			if (__d2_pop_2tok(exp, &poptok1, &poptok2) != E13_OK) {
				d2_perr(NULL, "%s\n", "syntax error");
				return e13_error(E13_SYNTAX);
			} else {
				if ((err =
					 __d2_tok_val(ctx, poptok1,
						  &val1)) != E13_OK)
					return err;
				if ((err =
					 __d2_tok_val(ctx, poptok2,
						  &val2)) != E13_OK)
					return err;
			}

			break;
			//other cases!!
			default:
			break;
		}

		//phase b. do the calc
    has_ret = 1;
		switch (enumtok->rec.code) {
  
		case TOK_PLUS1:
				__d2_assign_tok_val(ctx, poptok1, val1 + 1);
				__d2_push_tok(exp, poptok1);
			break;

		case TOK_MINUS1:
				__d2_assign_tok_val(ctx, poptok1, val1 - 1);
				__d2_push_tok(exp, poptok1);
			break;

		case TOK_LOGIC_NOT:
				__d2_assign_tok_val(ctx, poptok1, !val1);
				__d2_push_tok(exp, poptok1);
			break;
			
		case TOK_BIT_NOT:			
				__d2_assign_tok_val(ctx, poptok1,
							(long
							 double)(~(long
								   long)(val1)));
				__d2_push_tok(exp, poptok1);
			break;

		case TOK_MULT:
				__d2_assign_tok_val(ctx, poptok2, val2 * val1);
				poptok2->rec.code = TOK_NUMBER;
				__d2_push_tok(exp, poptok2);
			break;

		case TOK_DIV:
				__d2_assign_tok_val(ctx, poptok2, val2 / val1);
				poptok2->rec.code = TOK_NUMBER;
				__d2_push_tok(exp, poptok2);
			break;

		case TOK_REMAIN:
				__d2_assign_tok_val(ctx, poptok2,
							(long double)((long long)
								  val2 %
								  (long long)
								  val1));
				poptok2->rec.code = TOK_NUMBER;
				__d2_push_tok(exp, poptok2);
			break;

		case TOK_ADD:
				__d2_assign_tok_val(ctx, poptok2, val2 + val1);
				poptok2->rec.code = TOK_NUMBER;
				__d2_push_tok(exp, poptok2);
			break;

		case TOK_SUB:
				__d2_assign_tok_val(ctx, poptok2, val2 - val1);
				poptok2->rec.code = TOK_NUMBER;
				__d2_push_tok(exp, poptok2);
			break;

		case TOK_BIT_SHIFT_LEFT:
				__d2_assign_tok_val(ctx, poptok2,
							(long double)((long long)
								  val2 << (long
									   long)
								  val1));
				poptok2->rec.code = TOK_NUMBER;
				__d2_push_tok(exp, poptok2);
			break;

		case TOK_BIT_SHIFT_RIGHT:
				__d2_assign_tok_val(ctx, poptok2,
							(long double)((long long)
								  val2 >> (long
									   long)
								  val1));
				poptok2->rec.code = TOK_NUMBER;
				__d2_push_tok(exp, poptok2);
			break;

		case TOK_LT:
				__d2_assign_tok_val(ctx, poptok2, val1 < val2);
				poptok2->rec.code = TOK_NUMBER;
				__d2_push_tok(exp, poptok2);
			break;

		case TOK_LE:
				__d2_assign_tok_val(ctx, poptok2, val1 <= val2);
				poptok2->rec.code = TOK_NUMBER;
				__d2_push_tok(exp, poptok2);
			break;

		case TOK_GT:
				__d2_assign_tok_val(ctx, poptok2, val1 > val2);
				poptok2->rec.code = TOK_NUMBER;
				__d2_push_tok(exp, poptok2);
			break;

		case TOK_GE:
				__d2_assign_tok_val(ctx, poptok2, val1 >= val2);
				poptok2->rec.code = TOK_NUMBER;
				__d2_push_tok(exp, poptok2);
			break;

		case TOK_EQ:
				__d2_assign_tok_val(ctx, poptok2, val1 == val2);
				poptok2->rec.code = TOK_NUMBER;
				__d2_push_tok(exp, poptok2);
			break;

		case TOK_NE:
				__d2_assign_tok_val(ctx, poptok2, val1 != val2);
				poptok2->rec.code = TOK_NUMBER;
				__d2_push_tok(exp, poptok2);
			break;

		case TOK_BIT_AND:
				__d2_assign_tok_val(ctx, poptok2,
							(long double)((long long)
								  val2 & (long
									  long)
								  val1));
				poptok2->rec.code = TOK_NUMBER;
				__d2_push_tok(exp, poptok2);
			break;

		case TOK_BIT_OR:
				__d2_assign_tok_val(ctx, poptok2,
							(long double)((long long)
								  val2 | (long
									  long)
								  val1));
				poptok2->rec.code = TOK_NUMBER;
				__d2_push_tok(exp, poptok2);
			break;

		case TOK_LOGIC_AND:
				__d2_assign_tok_val(ctx, poptok2,
							(long double)((long long)
								  val2
								  && (long long)
								  val1));
				poptok2->rec.code = TOK_NUMBER;
				__d2_push_tok(exp, poptok2);
			break;

		case TOK_LOGIC_OR:
				__d2_assign_tok_val(ctx, poptok2,
							(long double)((long long)
								  val2
								  || (long long)
								  val1));
				poptok2->rec.code = TOK_NUMBER;
				__d2_push_tok(exp, poptok2);
			break;

		case TOK_TERN_COND:
			return e13_error(E13_IMPLEMENT);
			break;

		case TOK_ASSIGN:
        if(__d2_pop_2tok(exp, &poptok1, &poptok2) != E13_OK) {
          d2_perr(NULL, "%s\n", "syntax error");
          return e13_error(E13_SYNTAX);
        }      
				if ((err =
					d2_assign_var(ctx, poptok1,
						poptok2)) != E13_OK)
					return err;
        __d2_tok_val(ctx, poptok2, &val2);
				__d2_assign_tok_val(ctx, poptok1, val2);
				__d2_push_tok(exp, poptok1);
        has_ret = 0;
			break;
		case TOK_ASSIGN_ADD:
        if(__d2_pop_2tok(exp, &poptok1, &poptok2) != E13_OK) {
          d2_perr(NULL, "%s\n", "syntax error");
          return e13_error(E13_SYNTAX);
        }
        __d2_tok_val(ctx, poptok1, &val1);
        __d2_tok_val(ctx, poptok2, &val2);
        __d2_assign_tok_val(ctx, poptok2, val1 + val2);
				if ((err =
					d2_assign_var(ctx, poptok1,
						poptok2)) != E13_OK)
					return err;
        has_ret = 0;
		break;
		case TOK_ASSIGN_SUB:
        __dm_calc("%s\n", "got assign sub");
        if(__d2_pop_2tok(exp, &poptok1, &poptok2) != E13_OK) {
          d2_perr(NULL, "%s\n", "syntax error");
          return e13_error(E13_SYNTAX);
        }
        __d2_tok_val(ctx, poptok1, &val1);
        __d2_tok_val(ctx, poptok2, &val2);
        __d2_assign_tok_val(ctx, poptok2, val1 - val2);
				if ((err =
					d2_assign_var(ctx, poptok1,
						poptok2)) != E13_OK)
					return err;
        has_ret = 0;
		break;
		case TOK_ASSIGN_MULT:
        __dm_calc("%s\n", "got assign mult");
        if(__d2_pop_2tok(exp, &poptok1, &poptok2) != E13_OK) {
          d2_perr(NULL, "%s\n", "syntax error");
          return e13_error(E13_SYNTAX);
        }
        __d2_tok_val(ctx, poptok1, &val1);
        __d2_tok_val(ctx, poptok2, &val2);
        __d2_assign_tok_val(ctx, poptok2, val1 * val2);
				if ((err =
					d2_assign_var(ctx, poptok1,
						poptok2)) != E13_OK)
					return err;
        has_ret = 0;
		break;
		case TOK_ASSIGN_DIV:
        if(__d2_pop_2tok(exp, &poptok1, &poptok2) != E13_OK) {
          d2_perr(NULL, "%s\n", "syntax error");
          return e13_error(E13_SYNTAX);
        }
        __d2_tok_val(ctx, poptok1, &val1);
        __d2_tok_val(ctx, poptok2, &val2);
        __d2_assign_tok_val(ctx, poptok2, val1 / val2);
				if ((err =
					d2_assign_var(ctx, poptok1,
						poptok2)) != E13_OK)
					return err;
        has_ret = 0;
		break;
		case TOK_ASSIGN_REMAIN:
        if(__d2_pop_2tok(exp, &poptok1, &poptok2) != E13_OK) {
          d2_perr(NULL, "%s\n", "syntax error");
          return e13_error(E13_SYNTAX);
        }
        __d2_tok_val(ctx, poptok1, &val1);
        __d2_tok_val(ctx, poptok2, &val2);
        __d2_assign_tok_val(ctx, poptok2, (long long)val1 % (long long)val2);
				if ((err =
					d2_assign_var(ctx, poptok1,
						poptok2)) != E13_OK)
					return err;
        has_ret = 0;
		break;
		case TOK_ASSIGN_BIT_SHIFT_LEFT:
        if(__d2_pop_2tok(exp, &poptok1, &poptok2) != E13_OK) {
          d2_perr(NULL, "%s\n", "syntax error");
          return e13_error(E13_SYNTAX);
        }
        __d2_tok_val(ctx, poptok1, &val1);
        __d2_tok_val(ctx, poptok2, &val2);
        __d2_assign_tok_val(ctx, poptok2, (long long)val1 << (long long)val2);
				if ((err =
					d2_assign_var(ctx, poptok1,
						poptok2)) != E13_OK)
					return err;
        has_ret = 0;
		break;
		case TOK_ASSIGN_BIT_SHIFT_RIGHT:
        if(__d2_pop_2tok(exp, &poptok1, &poptok2) != E13_OK) {
          d2_perr(NULL, "%s\n", "syntax error");
          return e13_error(E13_SYNTAX);
        }
        __d2_tok_val(ctx, poptok1, &val1);
        __d2_tok_val(ctx, poptok2, &val2);
        __d2_assign_tok_val(ctx, poptok2, (long long)val1 >> (long long)val2);
				if ((err =
					d2_assign_var(ctx, poptok1,
						poptok2)) != E13_OK)
					return err;
        has_ret = 0;
		break;
		case TOK_ASSIGN_BIT_AND:
        if(__d2_pop_2tok(exp, &poptok1, &poptok2) != E13_OK) {
          d2_perr(NULL, "%s\n", "syntax error");
          return e13_error(E13_SYNTAX);
        }
        __d2_tok_val(ctx, poptok1, &val1);
        __d2_tok_val(ctx, poptok2, &val2);
        __d2_assign_tok_val(ctx, poptok2, (long long)val1 & (long long)val2);
				if ((err =
					d2_assign_var(ctx, poptok1,
						poptok2)) != E13_OK)
					return err;
        has_ret = 0;
		break;
		case TOK_ASSIGN_BIT_XOR:
        if(__d2_pop_2tok(exp, &poptok1, &poptok2) != E13_OK) {
          d2_perr(NULL, "%s\n", "syntax error");
          return e13_error(E13_SYNTAX);
        }
        __d2_tok_val(ctx, poptok1, &val1);
        __d2_tok_val(ctx, poptok2, &val2);
        __d2_assign_tok_val(ctx, poptok2, (long long)val1 ^ (long long)val2);
				if ((err =
					d2_assign_var(ctx, poptok1,
						poptok2)) != E13_OK)
					return err;
        has_ret = 0;
		break;
		case TOK_ASSIGN_BIT_OR:
        if(__d2_pop_2tok(exp, &poptok1, &poptok2) != E13_OK) {
          d2_perr(NULL, "%s\n", "syntax error");
          return e13_error(E13_SYNTAX);
        }
        __d2_tok_val(ctx, poptok1, &val1);
        __d2_tok_val(ctx, poptok2, &val2);
        __d2_assign_tok_val(ctx, poptok2, (long long)val1 | (long long)val2);
				if ((err =
					d2_assign_var(ctx, poptok1,
						poptok2)) != E13_OK)
					return err;
        has_ret = 0;
		break;

		case TOK_STRING:
			//TODO: this is so ugly to try resolve vars each time you run !
			if(d2_var_val(ctx, enumtok->rec.data, &enumtok->dval) == E13_OK) enumtok->rec.code = TOK_VAR;
      has_ret = 0;
		case TOK_NUMBER:
		case TOK_VAR:
			__d2_push_tok(exp, enumtok);
			break;

		default:
			//ignore
      __dm_def_calc("ignore: %s: %Lf\n", enumtok->rec.data, enumtok->dval);
			break;

		}

		enumtok = enumtok->prefix_next;
	}

	//__dm_calc("ans is %Lf, data is %s\n", exp->stack_top->dval,
	//	  exp->stack_top->rec.data);

	if(has_ret) __d2_add_ret(ctx, exp);

	return E13_OK;
}
