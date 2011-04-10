// lark.cpp : 定义控制台应用程序的入口点。
//

#include "ecore.h"

ecore_t cores[3];
int current = 0;

ecore_t* select_core()
{
	return &cores[++current%3];
}

void clientHandler(ecore_io_t* client)
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

void acceptHandler(ecore_io_t* listen_io)
{
	ecore_io_t* client;

	ecore_log_message(0, ECORE_LOG_SYSTEM, "监听线程已启动!");
	while(listen_io->core->is_running)
	{
		ecore_t* core = select_core();
		ecore_log_message(0, ECORE_LOG_SYSTEM, "尝试监听一个连接!");
		client = (ecore_io_t*)calloc(1, sizeof(ecore_io_t));
		if(ECORE_RC_OK != ecore_io_accept(listen_io, core, client))
		{
			ecore_log_message(0, ECORE_LOG_FATAL, listen_io->core->error.ptr);
			break;
		}

		ecore_log_message(0, ECORE_LOG_SYSTEM, "监听一个连接成功!");

		if(0 != ecore_start_thread(core, (void (*)(void*))&clientHandler, client, &client->name))
		{
			ecore_log_message(0, ECORE_LOG_FATAL, listen_io->core->error.ptr);
			break;
		}
	}
	ecore_io_close(listen_io);
}

void logWrite(const log_message_t** msg, size_t n)
{
	int i;
	for(i = 0; i < n; ++i)
	{
		printf(msg[i]->message);
	}
}

void _loop(ecore_t* core)
{
	char err[ECORE_MAX_ERR_LEN+1];

	if(ECORE_RC_OK != ecore_init(core,err,ECORE_MAX_ERR_LEN))
	{
		ecore_log_message(0, ECORE_LOG_FATAL, err);
		return;
	}

	while(core->is_running)
	{
		ecore_rc ret = ecore_poll(core, 1000);
		if(ECORE_RC_OK == ret)
			continue;

		if(ECORE_RC_TIMEOUT != ret)
		{
			ecore_log_message(0, ECORE_LOG_FATAL, core->error.ptr);
			break;
		}
	}


	ecore_finialize(core);
}

int main(int argc, char* argv[])
{
	int i;
	char err[ECORE_MAX_ERR_LEN+1];
	ecore_system_config_t config;
	ecore_io_t listen_io;
	ecore_handle_t listen_thread;
	char listen_address[256] = "tcp://0.0.0.0:8111";


	memset(&config, 0, sizeof(config));
	memset(&cores, 0, sizeof(cores));
	memset(&listen_io, 0, sizeof(listen_io));

	config.log_level = ECORE_LOG_ALL;
	config.log_callback = &logWrite;
	config.backend_threads = 7;

	if(ECORE_RC_OK != ecore_system_init(&config, err, ECORE_MAX_ERR_LEN))
	{
		printf(err);
		return 1;
	}



	if(ECORE_RC_OK != ecore_init(&cores[0], err, ECORE_MAX_ERR_LEN))
	{
		ecore_log_message(0, ECORE_LOG_FATAL, err);
		goto e1;
	}

	for(i =1; i < 3; ++i)
	{
		if(ECORE_RC_OK != ecore_system_queueTask(&_loop, &cores[i], err, ECORE_MAX_ERR_LEN))
			ecore_log_message(0, ECORE_LOG_FATAL, err);
	}


	if(ECORE_RC_OK != ecore_io_listion_at_url(&cores[0], &listen_io, listen_address))
	{
		ecore_log_message(0, ECORE_LOG_FATAL, cores[0].error.ptr);
		goto e2;
	}

	if(ECORE_RC_OK != ecore_start_thread2(&cores[0], (void (*)(void*)) &acceptHandler, &listen_io, "accept thread"))
	{
		ecore_log_message(0, ECORE_LOG_FATAL, cores[0].error.ptr);
		goto e3;
	}


	while(cores[0].is_running)
	{
		ecore_rc ret = ecore_poll(&cores[0], 1000);
		if(ECORE_RC_OK == ret)
			continue;

		if(ECORE_RC_TIMEOUT != ret)
		{
			ecore_log_message(0, ECORE_LOG_FATAL, cores[0].error.ptr);
			break;
		}
	}
e3:
	//ecore_thread_join();
	ecore_io_close(&listen_io);
e2:
	ecore_finialize(&cores[0]);
e1:
	ecore_system_finialize();
	return 0;
}

