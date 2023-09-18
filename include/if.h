#ifndef D2_IF_H
#define D2_IF_H

#include "error13.h"
#include "debug13.h"

#define dm _DebugMsg

#ifdef __cplusplus
extern "C" {
#endif

	e13_t d2_init_if(struct d2_handle *h, FILE * fin, FILE * fout);
	void d2_destroy_if(struct d2_handle *h);
	e13_t d2_print(struct d2_handle *h, char *fmt, ...);

#define d2pr d2_print

#ifdef __cplusplus
}
#endif
#endif				//D2_IF_H
