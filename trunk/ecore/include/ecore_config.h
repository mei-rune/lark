#ifndef _ecore_config_h_
#define _ecore_config_h_ 1

/* ����� inet_pton �� inet_ntop ����ʱ, �붨�� HAS_INET_NTOP */
/* #undef HAS_INET_NTOP */

/* On Windows, variables that may be in a DLL must be marked specially.  */
#ifdef _MSC_VER
#ifdef BUILDING_DLL
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
