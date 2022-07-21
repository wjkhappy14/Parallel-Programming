#include "pch.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include<WinSock2.h>
#include <hiredis.h>
#include"ConsoleApp.h"

int main(int argc, char **argv) {

	printf("ConsoleApp::Pi %d %s", ConsoleApp::Pi);

	unsigned int j;
	redisContext *c;
	redisReply *reply;
	const char *password = (argc > 1) ? argv[3] : "123456";
	const char *hostname = (argc > 1) ? argv[1] : "114.67.236.124";
	int port = (argc > 2) ? atoi(argv[2]) : 6379;

	struct timeval timeout = { 1, 500000 }; // 1.5 seconds
	c = redisConnectWithTimeout(hostname, port, timeout);
	if (c == NULL || c->err) {
		if (c) {
			printf("Connection error: %s\n", c->errstr);
			redisFree(c);
		}
		else {
			printf("Connection error: can't allocate redis context\n");
		}
	}
	else
	{
		printf("已经连接上Redis....\n");
	}
	int retval = redisAppendCommand(c, "SET Name Angkor");

	/* Disconnects and frees the context */
	//redisFree(c);
	redisCommand(c, "SET %s %s", "foo", "hello world");
	redisCommand(c, "AUTH %s", password);
	redisCommand(c, "SET %s", password);

	getchar();
	//exit(1);
	return 0;
}
