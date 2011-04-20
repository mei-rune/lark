// lark.cpp : 定义控制台应用程序的入口点。
//

#include "ecore.h"


ecore_application_t application;
int current = 0;

ecore_t* select_core()
{
	return &application.backend_cores[++current%3];
}

void clientHandler(ecore_t* local_core, ecore_io_t* client)
{
	char buf[1024];
	size_t ret;

	ecore_log_format(0, ECORE_LOG_SYSTEM, "新的连接到来 - %s <> %s!"
		, client->local_address.ptr, client->remote_address.ptr);
	while(client->core->is_running)
	{
		ecore_log_message(0, ECORE_LOG_SYSTEM, "尝试读数据!");
		ret = ecore_io_read_some(client, buf, 1024);
		if( -1 == ret)
		{
			ecore_log_message(0, ECORE_LOG_FATAL, client->core->error.ptr);
			break;
		}

		if(0 == ret)
		{
			ecore_log_message(0, ECORE_LOG_SYSTEM, client->core->error.ptr);
			break;
		}

		ecore_log_message(0, ECORE_LOG_SYSTEM, "尝试写数据!");
		if(ECORE_RC_OK != ecore_io_write(client, buf, ret))
		{
			ecore_log_message(0, ECORE_LOG_FATAL, client->core->error.ptr);
			break;
		}
	}

	ecore_log_format(0, ECORE_LOG_SYSTEM, "连接[ %s <> %s ] 断开!"
		, client->local_address.ptr, client->remote_address.ptr);

	ecore_io_close(client);
	free(client);
}

void acceptHandler(ecore_t* local_core, const char* listen_address)
{
	char err[ECORE_MAX_ERR_LEN+1];
	ecore_io_t listen_io;
	ecore_io_t* client;

	memset(&listen_io, 0, sizeof(listen_io));
	
	if(ECORE_RC_OK != ecore_io_listion_at_url(local_core, &listen_io, listen_address))
	{
		ecore_log_message(0, ECORE_LOG_FATAL, local_core->error.ptr);
		return ;
	}


	ecore_log_message(0, ECORE_LOG_SYSTEM, "监听线程已启动!");
	while(listen_io.core->is_running)
	{
		ecore_t* core = select_core();
		ecore_log_message(0, ECORE_LOG_SYSTEM, "尝试监听一个连接!");
		client = (ecore_io_t*)calloc(1, sizeof(ecore_io_t));
		if(ECORE_RC_OK != ecore_io_accept(&listen_io, core, client))
		{
			ecore_log_message(0, ECORE_LOG_FATAL, listen_io.core->error.ptr);
			break;
		}

		ecore_log_message(0, ECORE_LOG_SYSTEM, "监听一个连接成功!");

		if(ECORE_RC_OK != ecore_queueTask(core, &clientHandler, client, &client->name, err, ECORE_MAX_ERR_LEN))
		{
			ecore_log_message(0, ECORE_LOG_FATAL, err);
			break;
		}
	}
	ecore_io_close(&listen_io);
}

void logWrite(const log_message_t** msg, size_t n)
{
	int i;
	for(i = 0; i < n; ++i)
	{
		printf(msg[i]->message);
	}
}


int main(int argc, char* argv[])
{
	char err[ECORE_MAX_ERR_LEN+1];
	char listen_address[256] = "tcp://0.0.0.0:8111";


	memset(&application, 0, sizeof(application));

	application.log_level = ECORE_LOG_ALL;
	application.log_callback = &logWrite;
	application.backend_threads = 7;
	application.backend_core_num = 3;

	if(ECORE_RC_OK != ecore_application_init(&application, err, ECORE_MAX_ERR_LEN))
	{
		printf(err);
		return 1;
	}
	
	if(ECORE_RC_OK != ecore_application_queueTask_c(&acceptHandler,
		listen_address, "accept thread", err, ECORE_MAX_ERR_LEN))
	{
		printf(err);
		return 1;
	}

	ecore_application_loop();
	return 0;
}

