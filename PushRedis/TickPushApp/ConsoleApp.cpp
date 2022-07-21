#include "TapQuoteAPI.h"
#include "TapAPIError.h"
#include "Quote.h"
#include "QuoteConfig.h"

#include "pch.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include<WinSock2.h>

#include"ConsoleApp.h"
using namespace std;
using namespace QuotePushRedis;


int main(int argc, char **argv) {

	//cout << "控制台显示乱码纠正";

	// 控制台显示乱码纠正
	//system("chcp 65001"); //设置字符集 （使用SetConsoleCP(65001)设置无效，原因未知）

	//CONSOLE_FONT_INFOEX info = { 0 };
	//// 以下设置字体
	//info.cbSize = sizeof(info);
	//info.dwFontSize.Y = 16; // leave X as zero
	//info.FontWeight = FW_NORMAL;
	//wcscpy(info.FaceName, L"Consolas");
	//SetCurrentConsoleFontEx(GetStdHandle(STD_OUTPUT_HANDLE), NULL, &info);


	auto hello = std::string{ "Hello!" };
	char* numbers[10] = { "A","B","C","D","E","F","G" };
	std::string s = "How are you ";

	auto str = hello.c_str();
	TAPISTR_10 str10 = "Hello";

	cout << GetTapQuoteAPIVersion() << endl;

	TAPIINT32 iResult = TAPIERROR_SUCCEED;
	TapAPIApplicationInfo stAppInfo;
	strcpy(stAppInfo.AuthCode, DEFAULT_AUTHCODE);
	strcpy(stAppInfo.KeyOperationLogPath, "");
	ITapQuoteAPI *pAPI = CreateTapQuoteAPI(&stAppInfo, iResult);
	if (NULL == pAPI) {
		cout << "........" << iResult << endl;
		//	return -1;
	}
	Quote objQuote;
	pAPI->SetAPINotify(&objQuote);

	objQuote.SetAPI(pAPI);
	objQuote.Run();

	auto c = getchar();
	return 0;
}