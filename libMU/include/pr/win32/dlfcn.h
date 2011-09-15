
#ifndef _win32_dlfcn_h_
#define _win32_dlfcn_h_ 1

#include <sys/types.h>
#include <platform.h>


#ifndef _WIN32
# error This file is only currently defined for compilation on Win32 systems
#endif /* _WIN32 */

/* /////////////////////////////////////////////////////////////////////////
 * Constants and definitions
 */

#define RTLD_LAZY           (0x00001)       /*!< Lazy function call binding. */
#define RTLD_NOW            (0x00002)       /*!< Immediate function call binding. */

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/** \brief Loads the given module, 
 *
 * \param moduleName The name of the module. The name is passed through to Win32's
 * LoadLibrary(), so the value of this parameter is interpreted according to Win32's
 * loading rules.
 * \param mode The loading mode. This is ignored on Win32 platforms, and the function
 * always behaves as if RTLD_NOW was specified
 * \note 
 */
DLL_VARIABLE void        *dlopen(char const *moduleName, int mode);

/** \brief Closes the given module
 *
 * \param hModule The handle of the executable module to close
 * \retval 0 success
 * \retval !0 failure. dlerror() will return an error string
 */
DLL_VARIABLE int         dlclose(void *hModule);

/** \brief Looks up a symbol
 *
 * \param hModule The handle of the executable module in which to search for the symbol
 * \param symbolName The name of the symbol
 * \return The pointer to the symbol, or NULL if no matching symbol was found
 * \retval NULL The symbol was not found
 */
DLL_VARIABLE void        *dlsym(void *hModule, char const *symbolName);

/** \brief Returns a descriptive string for the last error
 *
 * \return A pointer to a string describing the last error, or NULL.
 * \retval NULL No error occured, or dlerror() already called
 * \note Each call to this function resets it, so that a second call will always return NULL
 */
DLL_VARIABLE char const  *dlerror(void);

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* _win32_dlfcn_h_ */

