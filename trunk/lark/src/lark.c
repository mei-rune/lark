// lark.cpp : 定义控制台应用程序的入口点。
//

#include "ecore.h"

ecore_t* core = NULL;

void clientHandler(ecore_io_t* client)
{
	while(client->core->is_running)
	{
		char buf[1024];
		unsigned int ret = ecore_io_read_some(client, buf, 1024);
		if( -1 == ret)
		{
			printf(client->core->error.ptr);
			break;
		}

		if(!ecore_io_write(client, buf, ret))
		{
			printf(client->core->error.ptr);
			break;
		}
	}

	ecore_io_close(client);
	free(client);
}

void acceptHandler(ecore_io_t* listen_io)
{
	while(listen_io->core->is_running)
	{
		ecore_io_t* client = (ecore_io_t*)calloc(1, sizeof(ecore_io_t));
		if(!ecore_io_accept(listen_io, client))
		{
			printf(listen_io->core->error.ptr);
			break;
		}

		if(0 != ecore_start_thread(core, &clientHandler, client))
		{
			printf(listen_io->core->error.ptr);
			break;
		}
	}
	ecore_io_close(listen_io);
}

int main(int argc, char* argv[])
{
	char err[ECORE_MAX_ERR_LEN+1];
	string_t address;
	ecore_t core;
	ecore_io_t listen_io;
  
	if(!ecore_init(&core,err,ECORE_MAX_ERR_LEN))
	{
	  printf(err);
	  return 1;
	}

	if(!ecore_io_listion_at(&core, &listen_io, string_create(&address, "tcp://0.0.0.0:8111")))
	{
		printf(core.error.ptr);
		return 1;
	}
	
	ecore_start_thread(&core,(void (*)(void*)) &acceptHandler, &listen_io);

	while(core.is_running)
	{
		if(0 != ecore_poll(&core, 1000))
		{
			printf(core.error.ptr);
			break;
		}
	}
	

	ecore_finalize(&core);
	return 0;
}

