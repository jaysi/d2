#ifndef D2_STREAM_H
#define D2_STREAM_H

#include "d2.h" //usnig d2_handle stream buffers

#ifndef MACRO
#define MACRO(A) do { A; } while(0)
#endif

#ifdef D2_GTK
#elif defined (D2_QT)
#define print(h, fmt, ...)
#else //use CLI
#define print(h, icon, fmt, ...)  MACRO(	d2_lock_stream(h);\
											fprintf(h->fout, ICON);\
											fprintf(h->fout, fmt, ##__VA_ARGS__);\
											fflush(h->fout);\
											d2_unlock_stream(h);	)
#endif //D2_CLI

#endif //D2_STREAM_H