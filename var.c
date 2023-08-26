#include "error13.h"
#include "d2.h"

e13_t d2_var_val(struct d2_ctx* ctx, char* name, long double* val){
    struct d2_var* var;
    for(var = ctx->var_list_first; var; var = var->next){
        if(!strcmp(var->name, name)){
            *val = var->val;
            return E13_OK;
        }
    }
    return e13_error(E13_NOTFOUND);
}

e13_t __d2_set_var(struct d2_ctx* ctx, char* name, long double val){
    struct d2_var* var;
    for(var = ctx->var_list_first; var; var = var->next){
        if(!strcmp(var->name, name)){
            var->val = val;
            return E13_OK;
        }
    }
    return e13_error(E13_NOTFOUND);
}

e13_t d2_assign_var(struct d2_ctx* ctx, char* name, long double* val){

    long double v;
    struct d2_var* var;

    if(__d2_set_val(ctx, name, *val) == E13_OK) return E13_OK;

    var = (struct d2_var*)malloc(sizeof(struct d2_var));
    if(!var) return e13_error(E13_NOMEM);

    var->name = (char*)malloc(strlen(name)+1);
    strcpy(var->name, name);
    var->val = *val;
    var->next = NULL;

    if(!ctx->var_list_first){
        ctx->var_list_first = var;
        ctx->var_list_last = var;
    } else {
        ctx->var_list_last->next = var;
        ctx->var_list_last = var;
    }

    return E13_OK;

}