#include "d2.h"
#include "error13.h"
#include "dmsg.h"

#define dm_exp1 _dm
#define dm_print_tok _dm

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

	return ++nexp;		//will reserve one for termination
}

e13_t d2_expize(struct d2_ctx *ctx, struct d2_exp *parent)
{
	struct d2_tok *tok;
	size_t nexp_est = __d2_nexp(ctx->tok_list_first);
	if (!nexp_est)
		return e13_error(E13_EMPTY);

	ctx->exps = (struct d2_exp *)malloc(nexp_est * sizeof(struct d2_exp));
	if (!(ctx->exps))
		return e13_error(E13_NOMEM);

	tok = ctx->tok_list_first;
	ctx->nexps = 0UL;
	ctx->exps[ctx->nexps].infix_tok_first = tok;
	ctx->exps[ctx->nexps].ntok = 1UL;
	while (tok) {
		dm_print_tok("tok->code: %i, data: %s, next: %s\n",
			     tok->rec.code, tok->rec.data,
			     tok->next ? tok->next->rec.data : "NULL");
		switch (tok->rec.code) {
		case TOK_SEMICOLON:
		case TOK_LABEL:
			dm_print_tok("got ; or : -> *nexp: %li, est: %li\n",
				     ctx->nexps, nexp_est);
			ctx->exps[ctx->nexps].infix_tok_last = tok;
			ctx->nexps++;
			if (tok->next) {
				assert(ctx->nexps < nexp_est - 1);
				ctx->exps[ctx->nexps].infix_tok_first =
				    tok->next;
				ctx->exps[ctx->nexps].ntok = 1UL;
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
			   } else */ ctx->exps[ctx->nexps].ntok++;
			break;
		}
		if (!tok->next)
			ctx->exps[ctx->nexps].infix_tok_last = tok;
		tok = tok->next;
	}			//while(tok)

	return E13_OK;
}
