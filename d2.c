#include "include/d2.h"
#include "include/lock.h"
#include "include/conf.h"
#include "lib13.h"

e13_t d2_init(struct d2_handle* h, struct d2_conf* conf){
	d2_init_handle_locks(h);	
	return E13_OK;
}

int main(int argc, char* argv[]){

	struct d2_handle h;
	struct d2_conf conf;

	d2_load_conf(&conf, NULL);

	d2_init(&h, &conf);	

	if(argc > 1){
		
	} else {

	}
			
}
