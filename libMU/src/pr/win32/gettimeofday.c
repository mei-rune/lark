#ifdef HAVE_BPR_CONFIG_H
# include "config.h"
#endif

#ifndef UNIXEM_DOCUMENTATION_SKIP_SECTION
# define _SYNSOFT_VER_C_TIME_MAJOR      2
# define _SYNSOFT_VER_C_TIME_MINOR      0
# define _SYNSOFT_VER_C_TIME_REVISION   8
# define _SYNSOFT_VER_C_TIME_EDIT       22
#endif /* !UNIXEM_DOCUMENTATION_SKIP_SECTION */

/* /////////////////////////////////////////////////////////////////////////
 * Includes
 */

#ifdef _WIN32
# include <windows.h>
#include "platform.h"

#ifdef __cplusplus
extern "C" {
#endif

struct timezone;

/* /////////////////////////////////////////////////////////////////////////
 * Helper functions
 */

/** This function is from the STLSoft libraries */
extern long FILETIMEToUNIXTime(FILETIME const *ft, long *microseconds)
{
#if defined(__COMO__)
    long long   i;
#else /* ? compiler */
    LONGLONG    i;
#endif /* compiler */

    i = ft->dwHighDateTime;
    i <<= 32;
    i |= ft->dwLowDateTime;

#if (   (   defined(__BORLANDC__) && \
            __BORLANDC__ >= 0x0582) || \
        defined(__DMC__) || \
        defined(__COMO__) || \
        defined(__GNUC__) || \
        defined(__MWERKS__))
    i -= 116444736000000000LL;
#else
    i -= 116444736000000000L;
#endif /* compiler */
    if(NULL != microseconds)
    {
        *microseconds = (long)((i % 10000000) / 10);
    }
    i /= 10000000;

    return (long)i;
}

/* /////////////////////////////////////////////////////////////////////////
 * API functions
 */

DLL_VARIABLE void gettimeofday(struct timeval *tv, void *dummy)
{
    SYSTEMTIME  st;
    FILETIME    ft;

    ((void)dummy);

    GetSystemTime(&st);
    (void)SystemTimeToFileTime(&st, &ft);

    tv->tv_sec = FILETIMEToUNIXTime(&ft, &tv->tv_usec);
}

#ifdef __cplusplus
}
#endif

#endif
