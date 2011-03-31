// lark.cpp : 定义控制台应用程序的入口点。
//

#include "ecore.h"

ecore_t* core = NULL;

void log(ecore_t* core, int level, const char* fmt, ... )
{
}

void clientHandler(ecore_io_t* client)
{
	for(;core->is_running;)
	{
	
	}
}

void acceptHandler(ecore_io_t* listen_io)
{
	for(;_ecore_is_running();)
	{
		ecore_io_t* client = ecore_io_accept(listen_io);
		if( NULL == client)
		{
			log(listen_io->
		}

			  ecore_start_thread(core, &clientHandler, client);
	}
}

int main(int argc, char* argv[])
{
  core = ecore_new();

  ecore_io_t* listion_io = ecore_io_listion_at(string_create("tcp://127.0.0.1:8111"));

  ecore_thread(core, &acceptHandler, listion_io);
  ecore_loop(core);

  ecore_free(core);
  return 0;
}

