
#ifndef PLATFORM_H
#define PLATFORM_H 1

#ifdef _WIN32
#define socket_type SOCKET
#define iovec WSABUF
#else
 typedef int socket_type;
 #define closesocket  close
#endif

 #if __GNUC__
typedef long long int int64;
typedef unsigned long long int uint64;
#else
typedef __int64 int64;
typedef unsigned __int64 uint64;
#endif

#ifndef NULL
#define NULL 0
#endif

typedef int mu_boolean;

#ifndef MU_FALSE
#define MU_FALSE ((mu_boolean)0)
#endif

#ifndef MU_TRUE
#define MU_TRUE ((mu_boolean)1)
#endif

#ifndef EMPTY
#define EMPTY
#endif

#ifdef _WIN32
// 定义此宏表示将在 win2003 上运行
#ifndef _WIN32_WINNT
#define _WIN32_WINNT  0x0501
#endif

#define snprintf _snprintf
#define vsnprintf _vsnprintf
#define vscprintf _vscprintf
#define strncasecmp _strnicmp
#define strcasecmp _stricmp
#define strtoll _strtoi64
#define strtoull _strtoui64

//#define filelength _filelength
//#define fileno _fileno
//#define getcwd _getcwd
//#define stat _stat
//#define access _access
#else
#define vscprintf(fmt, argList) vsnprintf(0, 0, fmt, argList)
#endif

#define my_malloc   malloc
#define my_free     free
#define my_realloc  realloc
#define my_calloc   calloc
#define my_strdup   strdup



/* On Windows, variables that may be in a DLL must be marked specially.  */
#ifdef _MSC_VER 
#ifdef _USRDLL
#if  MU_EXPORTS
# define DLL_VARIABLE __declspec (dllexport)
# define DLL_VARIABLE_C extern "C" __declspec (dllexport)
#else
# define DLL_VARIABLE __declspec (dllimport)
# define DLL_VARIABLE_C extern "C" __declspec (dllimport)
#endif
#else
# define DLL_VARIABLE extern
# define DLL_VARIABLE_C extern
#endif
#else
# define DLL_VARIABLE extern
# define DLL_VARIABLE_C extern
#endif

#define mu_max(x,y) (((x) > (y))?(x):(y))
#define mu_min(x,y) (((x) < (y))?(x):(y))

#endif