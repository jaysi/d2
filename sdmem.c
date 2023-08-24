//static - dynamic memmory manager for allocating many small objects in a single buffer
#include "error13.h"

struct sdmem_node {
	size_t size;
	char* buf;
	struct sdmem_node* next;
};

struct sdmem_s {
    size_t n;
	size_t size;
	char* buf;
    	
};

int sdmem_init(struct sdmem_s* ctl, char* buf, size_t size, size_t n){
	return e13_error(E13_IMPLEMENT);
}