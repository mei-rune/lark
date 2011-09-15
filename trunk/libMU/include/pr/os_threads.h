
#ifndef OS_THREADS
#define OS_THREADS

#ifdef _WIN32
#include "win32/pthread_windows.h"
#else
#include <pthread.h>
#endif 

#ifdef __cplusplus
extern "C" 
{
#endif

DLL_VARIABLE void _system_lock();
DLL_VARIABLE void _system_unlock();

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus

class system_lock
{
public:
	system_lock()
	{
		_system_lock();
	}

	~system_lock()
	{
		_system_unlock();
	}
private:
	system_lock(const system_lock&);
	system_lock& operator=(const system_lock&);
};

#endif


#endif 

