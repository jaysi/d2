#include <string.h>

#include "error13.h"
#include "d2.h"
#include "dmsg.h"

#define dm_var1 _dm
#define dm_var2 _dm_flat

e13_t d2_var_val(struct d2_ctx *ctx, char *name, long double *val)
{
	struct d2_var *var;
	dm_var1("getting varval of %s...", name);
	for (var = ctx->var_list_first; var; var = var->next) {
		if (!strcmp(var->name, name)) {
			*val = var->val;
            dm_var2("found %Lf\n", *val);
			return E13_OK;
		}
	}
    dm_var2("%s", "not found\n");
	return e13_error(E13_NOTFOUND);
}

e13_t __d2_set_var(struct d2_ctx *ctx, char *name, long double val)
{
	struct d2_var *var;
	for (var = ctx->var_list_first; var; var = var->next) {
		if (!strcmp(var->name, name)) {
			var->val = val;
			return E13_OK;
		}
	}
	return e13_error(E13_NOTFOUND);
}

e13_t d2_assign_var(struct d2_ctx *ctx, struct d2_tok *tok, long double *val)
{

	struct d2_var *var;

	dm_var1("assign var %s\n", tok->rec.data);

	if (__d2_set_var(ctx, tok->rec.data, *val) == E13_OK)
		return E13_OK;

	if (tok->rec.code != TOK_STRING)
		return e13_error(E13_CONSTRAINT);

	var = (struct d2_var *)malloc(sizeof(struct d2_var));
	if (!var)
		return e13_error(E13_NOMEM);

	tok->rec.code = TOK_VAR;

	var->name = (char *)malloc(strlen(tok->rec.data) + 1);
	strcpy(var->name, tok->rec.data);
	var->val = *val;
	var->next = NULL;

	if (!ctx->var_list_first) {
		ctx->var_list_first = var;
		ctx->var_list_last = var;
	} else {
		ctx->var_list_last->next = var;
		ctx->var_list_last = var;
	}

	return E13_OK;

}
