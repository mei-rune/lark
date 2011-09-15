#ifdef _WIN32
#include <io.h>
#endif
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "rollingfile.h"
#include "string_buffer.h"

#ifdef __cplusplus
extern "C"
{
#endif


struct _rollingfile_s
{
	char* nm;
	size_t max_size;
	size_t remain_count;

	size_t current_size;

	FILE* out;
};

void debugPrintf(const char* src, const char* dst)
{
	printf("roll file %s to %s - %s\n", src, dst, strerror(errno));
//#ifdef _WIN32
//	{
//		char buf[1024];
//		snprintf(buf, 1024, "roll file %s to %s - %s\n", src, dst, strerror(errno));
//		OutputDebugString(buf);
//	}
//#endif
}

void ensure_file_size(rollingfile_t* rf)
{
	string_buffer_t bak1;
	string_buffer_t bak2;
	size_t n;
	if(rf->current_size < rf->max_size)	
	{
		if(0 == rf->out)
		{
			rf->out = fopen(rf->nm, "a+");
			
			if(0 == rf->out)
				printf("打开日志文件 '%s' 失败 - %s", rf->nm, strerror(errno));
		}
		return;
	}

	if(0 != rf->out)
	{
		fclose(rf->out);
		rf->out = 0;
	}

	string_buffer_init(&bak1);
	string_buffer_init(&bak2);

	string_buffer_assign(&bak2, rf->nm);
	string_buffer_append_sprintf(&bak2,".%u", rf->remain_count);
	remove(string_data(&bak2));

	for(n = rf->remain_count-1; n > 0; --n)
	{
		string_buffer_assign(&bak1, rf->nm);
		string_buffer_append_sprintf(&bak1,".%u", n);
		if(0 != rename(string_data(&bak1), string_data(&bak2)))
			debugPrintf(string_data(&bak1), string_data(&bak2));
		string_buffer_swap(&bak1, &bak2);
	}
	if(0 != rename(rf->nm, string_data(&bak2)))
	{
		debugPrintf(rf->nm, string_data(&bak2));
	}
	else
	{
		rf->current_size = 0;
		rf->out = fopen(rf->nm, "a+");
		
		if(0 == rf->out)
			printf("打开日志文件 '%s' 失败 - %s", rf->nm, strerror(errno));
	}


	string_buffer_destroy(&bak1);
	string_buffer_destroy(&bak2);
}

DLL_VARIABLE rollingfile_t* rollingfile_create(const char* nm, size_t max_size, size_t remain_count)
{
	rollingfile_t* ret;
	if(0 == nm)
		return 0;

	ret = (rollingfile_t*)my_malloc(sizeof(rollingfile_t));
	ret->nm = strdup(nm);
	ret->max_size = max_size;
	ret->remain_count = remain_count;
	ret->current_size = max_size + 10;
	ret->out = fopen(nm, "a+");
	if(0 == ret->out)
	{
		my_free(ret->nm);
		my_free(ret);
		return 0;
	}	

	return ret;	
}

DLL_VARIABLE void rollingfile_free(rollingfile_t* rf)
{
	if(0 == rf)
		return;

	if(0 != rf->out)
		fclose(rf->out);
	if(0 != rf->nm)
		my_free(rf->nm);
	my_free(rf);
}


DLL_VARIABLE void rollingfile_write(rollingfile_t* rf, const char* data, size_t n)
{
	if(0 == rf)
		return;

	ensure_file_size(rf);

	if(0 == rf->out)
		return;

	fwrite(data, sizeof(char), n, rf->out);
	rf->current_size += n;
}

DLL_VARIABLE void rollingfile_flush(rollingfile_t* rf)
{
	if(0 == rf || 0 == rf->out)
		return;

	fflush(rf->out);
}

#ifdef __cplusplus
}
#endif
