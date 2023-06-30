#include "include/d2.h"
#include "include/lock.h"
#include "include/conf.h"
#include "lib13.h"

/*
    global() {
        
        var1;
        var2;
        var3;

        main() {
            
        }

        fn1(arg1, arg2){
            return var;
        }

        fn2(){

        }
    }
*/

e13_t d2_init(struct d2_handle* h, struct d2_conf* conf){
	d2_init_handle_locks(h);	
	return E13_OK;
}

int main(int argc, char* argv[]){

	struct d2_handle h;
	struct d2_conf conf;

#ifndef NDEBUG
    printf("compile time:%s-%s\n", __DATE__, __TIME__);
#endif

	d2_load_conf(&conf, NULL);

	d2_init(&h, &conf);	

    test_tokenize();

	if(argc > 1){
		
	} else {

	}
			
}
