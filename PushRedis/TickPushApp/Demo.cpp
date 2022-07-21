#include "TapQuoteAPI.h"
#include "TapAPIError.h"
#include "Quote.h"
#include "QuoteConfig.h"

#include <iostream>
#include <string.h>
using namespace std;

int main(int argc, char* argv[])
{
	//取得API的版本信息
	cout << GetTapQuoteAPIVersion() << endl;

	//创建API实例
	TAPIINT32 iResult = TAPIERROR_SUCCEED;
	TapAPIApplicationInfo stAppInfo;
	strcpy(stAppInfo.AuthCode, DEFAULT_AUTHCODE);
	strcpy(stAppInfo.KeyOperationLogPath, "");
	ITapQuoteAPI *pAPI = CreateTapQuoteAPI(&stAppInfo, iResult);
	if (NULL == pAPI){
		cout << "创建API实例失败，错误码：" << iResult <<endl;
		return -1;
	}

	//设定ITapQuoteAPINotify的实现类，用于异步消息的接收
	Quote objQuote;
	pAPI->SetAPINotify(&objQuote);
	

	//启动测试，订阅行情
	objQuote.SetAPI(pAPI);
	objQuote.RunTest();


	return 0;
}