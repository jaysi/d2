#ifndef D2_LOCK_H
#define D2_LOCK_H

#include <pthread.h>

#ifndef MACRO
#define MACRO(A) do { A } while(0)
#endif

#define d2_init_handle_locks(h) MACRO( pthread_mutex_init(&(h)->stream_mx, NULL); pthread_mutex_init(&(h)->ctxlist_mx, NULL);)
#define d2_lock_stream(h)   MACRO( pthread_mutex_lock(&(h)->stream_mx); )
#define d2_unlock_stream(h)   MACRO( pthread_mutex_unlock(&(h)->stream_mx); )
#define d2_lock_ctx(h)  MACRO( pthread_mutex_lock(&(h)->ctxlist_mx); )
#define d2_unlock_ctx(h)  MACRO( pthread_mutex_unlock(&(h)->ctxlist_mx); )
#endif				//D2_LOCK_H
