#include <stdio.h>

#include "include/d2.h"
#include "include/lock.h"
#include "include/conf.h"
#include "lib13.h"

#undef TEST_TOKENIZE

#ifdef TEST_TOKENIZE
extern int test_tokenize();
#endif

extern void d2_console_if(struct d2_handle* h);

e13_t d2_init_handle(struct d2_handle* h, FILE* fin, FILE* fout, FILE *ferr){
	d2_init_handle_locks(h);
    h->fin = fin;
    h->fout = fout;
    h->ferr = ferr;
    h->ctxlist_first = NULL;
	return E13_OK;
}

int main(int argc, char* argv[]){

	struct d2_handle h;	

#ifndef NDEBUG
    printf("compile time:%s-%s\n", __DATE__, __TIME__);
#endif

    d2_load_conf(&h.conf, NULL);	
    d2_init_handle(&h, stdin, stdout, stderr);	

#ifdef TEST_TOKENIZE
    test_tokenize();
    //test_tokenize2();
#endif
	if(argc > 1){
		
	} else {
        d2_console_if(&h);
	}
			
}
