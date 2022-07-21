#include "pch.h"	   
#include <cpp_redis/cpp_redis>
#include <tacopie/tacopie>

#include <condition_variable>
#include <iostream>
#include <mutex>
#include <signal.h>

#include <Winsock2.h>
using namespace std::chrono;

std::condition_variable should_exit;
//https://github.com/Cylix/cpp_redis/wiki/Examples#redis-subscriber

void sigint_handler(int) {
	should_exit.notify_all();
}

int
main(void) {

	printf("\x1B[35mTexting\033[0m\n");

	const WORD colors[] =
	{
	0x1A, 0x2B, 0x3C, 0x4D, 0x5E, 0x6F,
	0xA1, 0xB2, 0xC3, 0xD4, 0xE5, 0xF6
	};

	HANDLE hstdin = GetStdHandle(STD_INPUT_HANDLE);
	HANDLE hstdout = GetStdHandle(STD_OUTPUT_HANDLE);
	WORD   index = 0;

	// Remember how things were when we started
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(hstdout, &csbi);

	// Tell the user how to stop
	SetConsoleTextAttribute(hstdout, 0xEC);
	std::cout << "Press any key to quit.\n";


	//! Windows netword DLL init
	WORD version = MAKEWORD(2, 2);
	WSADATA data;

	if (WSAStartup(version, &data) != 0) {
		std::cerr << "WSAStartup() failure" << std::endl;
		return -1;
	}

	//! Enable logging
	cpp_redis::active_logger = std::unique_ptr<cpp_redis::logger>(new cpp_redis::logger);
	const char *password = "1234QWERasdf";
	const char *hostname = "10.0.1.5"; //110.42.6.125		 47.98.226.195
	int port = 6379;
	cpp_redis::client client;

	client.connect(hostname, 6379, [](const std::string& host, std::size_t port, cpp_redis::client::connect_state status) {
		if (status == cpp_redis::client::connect_state::dropped) {
			std::cout << "disconnect :" << host << ":" << port << std::endl;
			//should_exit.notify_all();
		}
		else if (status == cpp_redis::client::connect_state::ok)
		{
			std::cout << "connect: " << host << ":" << port << std::endl;
		}
	});

	std::cout << "auth...ing" << std::endl;
	auto auth = client.auth(password);
	client.sync_commit();
	auto reply = auth.get();

	if (reply.is_error())
	{
		std::cerr << "auth……erroer: " << reply.as_string() << std::endl;
	}
	else {
		std::cout << "auth is ok" << std::endl;
	}

	
	while (true)
	{
		auto now = system_clock::now();
		auto milliseconds = now.time_since_epoch().count();
		long long x = 0;
		while (true)
		{
			x++;
			auto value = std::to_string(x);
			auto key = std::to_string(milliseconds) + ":" + value;

			client.set(key, value);
			client.publish("now", std::to_string(milliseconds));
			client.sync_commit();
		}
	}

	signal(SIGINT, &sigint_handler);
	std::mutex mtx;
	std::unique_lock<std::mutex> l(mtx);


	should_exit.wait(l);

	WSACleanup();

	return 0;
}
