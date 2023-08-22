#include "d2.h"
#include "error13.h"

int __d2_tok_preced(d2_tok_enum code);

void __d2_pop_tok(struct d2_tok* stack, size_t* nstack, struct d2_tok** tok){

}

void __d2_push_tok(struct d2_tok* stack, size_t* nstack, struct d2_tok* tok){
    
}

e13_t infix2prefix(struct d2_exp* exp){
    size_t ntok, nstack;
    struct d2_tok* tok;
    struct d2_tok* stack;    
    stack = (struct d2_tok*)malloc(exp->ntok*sizeof(struct d2_tok));    
    if(!stack) return e13_error(E13_NOMEM);
    nstack = 0UL;
    for(ntok = 0UL, tok = exp->infix_tok_last; tok, tok!=exp->infix_tok_first; tok = tok->prev){
        if(tok->rec.code < TOK_OO_LIMMIT){//TOK_VAR IS THE LIMMIT BETWEEN OPERATOR/OPERAND
            switch(tok->rec.code){
                
            }
        } else {//append to the end of prefix

        }
    }
}