#include "d2.h"
#include "if.h"
e13_t d2_init_if(struct d2_handle* h, FILE* fin, FILE* fout){
    h->fin = fin;
    h->fout = fout;
}

void d2_destroy_if(struct d2_handle* h){
    fclose(h->fin);
    fclose(h->fout);
}

e13_t d2_print(struct d2_handle* h, fmt, ...){

}

