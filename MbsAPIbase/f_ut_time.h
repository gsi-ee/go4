#ifndef f_ut_time_h
#define f_ut_time_h

#include "typedefs.h"

#include <time.h>

CHARS *f_ut_time (CHARS *pc_time);

#ifdef WIN32

#define CLOCK_REALTIME 1
int clock_gettime(int clockid, struct timespec *tp);

#endif

#endif
