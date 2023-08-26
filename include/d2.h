#ifndef D2_H
#define D2_H

#include <assert.h>
#include <pthread.h>
#include <stdio.h>

#include "token.h"

#define MAIN_CTX_NAME   "main"

#ifndef MACRO
#define MACRO(A) do { A; } while(0)
#endif

struct d2_exp {

	char flags;
	char* infixbuf;
    size_t ntok;
    
    //views
	struct d2_tok* infix_tok_first, *infix_tok_last;
    struct d2_tok* prefix_tok_first, *prefix_tok_last;
    struct d2_tok* stack_top;
    //list
	struct d2_exp* next, *prev;
};

struct d2_var {
    char* name;
    long double val;
    struct d2_val* next;
}

#define D2_CTXF_COPY_BUF    (0x01<<0)

struct d2_ctx {	

    char flags;
    size_t bufsize;
    char* buf;

    struct d2_tok* toklist_first;

    size_t nexps;
    struct d2_exp* exps;//dynamic array

    struct d2_var* var_list_first, *var_list_last;

	struct d2_context* next, *prev;

};

struct d2_handle {

	pthread_mutex_t ctxlist_mx;
	struct d2_context *ctxlist_first, *ctx;

	pthread_mutex_t stream_mx;
	//io streams
	FILE* fin, *fout;

};

#endif //D2_H
