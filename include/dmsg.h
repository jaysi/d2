#ifndef D2_DM_H
#define D2_DM_H

#if (!defined NDEBUG) || (defined DM_ON)
#define _dm(fmt, ...) fprintf(stderr, "dm::%s/%s/%i:"fmt,__FILE__,__func__,__LINE__, __VA_ARGS__)
#elif (defined NDEBUG) || (defined DM_OFF)
#define _dm(fmt, ...)
#endif

#endif				//D2_DM_H
