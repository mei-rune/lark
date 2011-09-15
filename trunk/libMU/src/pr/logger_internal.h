
#ifndef _logger_internal_h_
#define _logger_internal_h_ 1

#include "logger_spi.h"

#ifdef __cplusplus
extern "C" {
#endif

log_appender_t* _appender_create_from_url(const char* url);

log_appender_t* _appender_create(const char* name
								, log_message_fn_t cb
								, void (*finialize)(log_appender_t*)
								, void* ctxt);

void _appender_free(log_appender_t* appender);

void _appender_hash_free(void* key, void* val);


log_appender_t* _appender_create_from_properties(properties_t* pr);


extern logging_module_t  tcp_module;
extern logging_module_t  file_module;
extern logging_module_t  rollingfile_module;
extern logging_module_t  writer_module;
extern logging_module_t  console_module;

#ifdef __cplusplus
}
#endif

#endif

