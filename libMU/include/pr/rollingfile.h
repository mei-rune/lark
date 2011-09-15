

#ifndef _rollingfile_h_
#define _rollingfile_h_ 1

#include "platform.h"
#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * 一个循环记录的文件句柄
 */
struct _rollingfile_s;

typedef struct _rollingfile_s rollingfile_t;

/**
 * 创建一个可以循环记录的文件句柄
 * @params[in] nm  文件名
 * @params[in] max_size  循环的阈值，当文件大小超过此值时，就
 * 可以创建新文件
 * @params[in] remain_count 文件的保留个数
 */
DLL_VARIABLE rollingfile_t* rollingfile_create(const char* nm, size_t max_size, size_t remain_count);

/**
 * 关闭可以循环记录的文件句柄
 */
DLL_VARIABLE void rollingfile_free(rollingfile_t* rf);

/**
 * 向文件中写入数据，注意当当文件大小超过循环阈值时，就删除最
 * 早的文件，并创建新文件，将数据写入新文件中
 */
DLL_VARIABLE void rollingfile_write(rollingfile_t* rf, const char* data, size_t n);

/**
 * 将缓存中的数据立即写入磁盘
 */
DLL_VARIABLE void rollingfile_flush(rollingfile_t* rf);

#ifdef __cplusplus
}
#endif

#endif /* _rollingfile_h_ */
	

