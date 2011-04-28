#ifndef _ecore_config_h_
#define _ecore_config_h_ 1

/* 如果有 inet_pton 或 inet_ntop 函数时, 请定义 HAS_INET_NTOP */
/* #undef HAS_INET_NTOP */



#ifdef _WIN32
#ifndef _WIN32_WINNT
#define _WIN32_WINNT   0x0502
#endif
#include <winsock2.h>
#include <windows.h>
#ifndef __MINGW32__
#include <SDKDDKVer.h>
#if (NTDDI_VERSION >= NTDDI_VISTA)
# define HAS_INET_NTOP 1
#endif
#endif
#else
# define HAS_INET_NTOP 1
#endif

#if (_WIN32_WINNT >= 0x0600)
#define HAS_GETQUEUEDCOMPLETIONSTATUSEX 1
#endif

/* On Windows, variables that may be in a DLL must be marked specially.  */
#ifdef _MSC_VER
#ifdef _USRDLL
#ifdef  ECORE_BUILDING
# define DLL_VARIABLE __declspec (dllexport)
#else
# define DLL_VARIABLE __declspec (dllimport)
#endif
#else
# define DLL_VARIABLE extern
#endif
#else
# define DLL_VARIABLE
#endif


#endif //_ecore_config_h_
