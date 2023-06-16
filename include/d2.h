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

struct expression{

    char flags;
    char* infix;

    struct d2_tok* infix_tok_first, *prefix_tok_first;

};

struct d2_context {

	//expression buffer
	size_t expbufsize;
	char* expbuf;

	//last answer array
	fnum_t* last_ans;
	
	struct d2_tok* toklist_first;
	
	struct d2_context* next;
};

struct d2_handle {

	pthread_mutex_t ctxlist_mx;
	struct d2_context *ctxlist_first, *ctx;

	pthread_mutex_t stream_mx;
	//io streams
	FILE* fin, *fout;

};

#endif //D2_H
