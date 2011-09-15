


#ifdef _WIN32

#ifdef HAVE_BPR_CONFIG_H
# include "config.h"
#endif


#include <windows.h>
#include "win32/dlfcn.h"


#define RTLD_BINDING_MASK   (0x00003)       /*!< Mask of binding time value. */


static const char   *s_error;


static void set_error_(DWORD dwErr)
{
    switch(dwErr)
    {
        case    ERROR_SUCCESS:
            s_error = NULL;
            break;
        case    ERROR_MOD_NOT_FOUND:
            s_error = "Module not found";
            break;
        case    ERROR_PROC_NOT_FOUND:
            s_error = "Symbol not found";
            break;
        case    ERROR_BAD_EXE_FORMAT:
            s_error = "Invalid image format";
            break;
        case    ERROR_SHARING_VIOLATION:
            s_error = "Sharing violation";
            break;
        default:
            s_error = "Operation failed";
            break;
    }
}


#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

DLL_VARIABLE void *dlopen(char const *moduleName, int mode)
{
    HMODULE hModule;

    if(mode & RTLD_LAZY)
    {
        OutputDebugStringA("Library does not support RTLD_LAZY; using RTLD_NOW\n");
    }

    hModule =   LoadLibraryA(moduleName);

    if(NULL == hModule)
    {
        set_error_(GetLastError());
    }
    else
    {
        set_error_(ERROR_SUCCESS);
    }

    return hModule;
}

DLL_VARIABLE int dlclose(void *hModule)
{
    if(!FreeLibrary((HMODULE)hModule))
    {
        set_error_(GetLastError());

        return 1;
    }

    set_error_(ERROR_SUCCESS);

    return 0;
}

DLL_VARIABLE void *dlsym(void *hModule, char const *symbolName)
{
    union
    {
        void    (*pfn)(void);
        void    *pv;
    } u;

    u.pfn = (void (*)(void))GetProcAddress((HMODULE)hModule, symbolName);

    if(NULL == u.pfn)
    {
        set_error_(GetLastError());
    }
    else
    {
        set_error_(ERROR_SUCCESS);
    }

    return u.pv;
}

DLL_VARIABLE char const *dlerror(void)
{
    char const  *err = s_error;

    s_error = NULL;

    return err;
}

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif