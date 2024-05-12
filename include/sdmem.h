#ifndef SDMEM_H
#define SDMEM_H

#include "error13.h"

struct d2_ctx;
struct d2_exp;
struct d2_tok;

#ifdef __cplusplus
extern "C" {
#endif

e13_t __d2_add_ret(struct d2_ctx *ctx, struct d2_exp *exp);
e13_t __d2_pop_tok(struct d2_exp *exp, struct d2_tok **tok);
e13_t __d2_pop_2tok(struct d2_exp *exp, struct d2_tok **tok1,
			   struct d2_tok **tok2);
void __d2_push_tok(struct d2_exp *exp, struct d2_tok *tok);

e13_t __d2_delete_tok_list(struct d2_ctx *ctx, int free_databuf);

e13_t __d2_skip_tok(struct d2_ctx *ctx, struct d2_tok *tok);
e13_t __d2_realloc_tok_buf(struct d2_tok *tok, char *data,
                size_t datasize, int free_old_data);
struct d2_tok *__d2_enumset_tok_buf(struct d2_ctx *ctx, char *data,
                    size_t datasize);
e13_t __d2_alloc_tok_databuf_pool(struct d2_ctx *ctx, size_t bufsize);
e13_t __d2_alloc_tok_list(struct d2_ctx *ctx, size_t ntok);
e13_t __d2_delete_tok_list(struct d2_ctx *ctx, int free_databuf);
e13_t __d2_free_unused_tok_list(struct d2_ctx* ctx, struct d2_tok* last_tok);//this is vital, terminate unused tokens
size_t __d2_get_tok_databuf_poolsize(char *buf, size_t ntok);


void __d2_appand_postfix(struct d2_exp *exp, struct d2_tok *tok);

#ifdef __cplusplus
}
#endif

#endif				//SDMEM_H
