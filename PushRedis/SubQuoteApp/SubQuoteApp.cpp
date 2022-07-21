#include "pch.h"	   
#include <cpp_redis/cpp_redis>
#include <tacopie/tacopie>

#include <condition_variable>
#include <iostream>
#include <mutex>
#include <signal.h>

#ifdef _WIN32
#include <Winsock2.h>
using namespace std::chrono;
#endif /* _WIN32 */

std::condition_variable should_exit;

void
sigint_handler(int) {
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


#ifdef _WIN32
	//! Windows netword DLL init
	WORD version = MAKEWORD(2, 2);
	WSADATA data;

	if (WSAStartup(version, &data) != 0) {
		std::cerr << "WSAStartup() failure" << std::endl;
		return -1;
	}
#endif /* _WIN32 */

	//! Enable logging
	cpp_redis::active_logger = std::unique_ptr<cpp_redis::logger>(new cpp_redis::logger);
	const char *password = "03hx5DDDivYmbkTgDlFz";
	const char *hostname = "110.42.6.125"; //110.42.6.125		 47.98.226.195
	int port = 6379;
	cpp_redis::subscriber sub;
	sub.connect(hostname, port, [](const std::string & host, std::size_t port, cpp_redis::subscriber::connect_state status) {
		if (status == cpp_redis::subscriber::connect_state::dropped) {
			std::cout << "连接断开 " << host << ":" << port << std::endl;
			should_exit.notify_all();
		}
		else if (status == cpp_redis::subscriber::connect_state::ok)
		{
			std::cout << "连接成功 " << host << ":" << port << std::endl;
		}
	});
	std::cout << "身份验证……" << std::endl;
	sub.auth(password, [](const cpp_redis::reply& reply) {
		if (reply.is_error())
		{
			std::cerr << "认证失败: " << reply.as_string() << std::endl;
		}
		else {
			std::cout << "认证成功！" << std::endl;
		}
	});

	sub.subscribe("SGX:CN:1906", [](const std::string & chan, const std::string & msg) {
		std::cout << "MESSAGE " << chan << ": " << msg << std::endl;
	});
	sub.commit();

	signal(SIGINT, &sigint_handler);
	std::mutex mtx;
	std::unique_lock<std::mutex> l(mtx);
	should_exit.wait(l);

#ifdef _WIN32
	WSACleanup();
#endif /* _WIN32 */

	return 0;
}