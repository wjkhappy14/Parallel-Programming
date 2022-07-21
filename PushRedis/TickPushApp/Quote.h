#ifndef QUOTE_H
#define QUOTE_H
#include "TapQuoteAPI.h"
#include "SimpleEvent.h"
#include <sstream>
#include <chrono>
using namespace std;
using namespace std::chrono;
namespace QuotePushRedis
{
	namespace Helper {
		template < typename T > std::string to_string(const T& n)
		{
			std::ostringstream stm;
			stm << n;
			return stm.str();
		}
		template < typename T > std::string format_quote(const TapAPIQuoteWhole *info)
		{
			std::string exchangeNo(info->Contract.Commodity.ExchangeNo);
			std::string commodityNo(info->Contract.Commodity.CommodityNo);
			std::string contractNo1(info->Contract.ContractNo1);

			return exchangeNo + commodityNo + contractNo1;
		}
		
	}
	class Quote : public ITapQuoteAPINotify
	{
	public:
		Quote(void);
		~Quote(void);

		void SetAPI(ITapQuoteAPI* pAPI);
		void Run();
		void SubscribeItems();
		milliseconds get_milliseconds(void);
		std::string datetime_timestamp(std::string  datetime = "1970.01.01 00:00:00.000");
		static	void  ConnectRedis();

	public:
		//对ITapQuoteAPINotify的实现
		virtual void TAP_CDECL OnRspLogin(TAPIINT32 errorCode, const TapAPIQuotLoginRspInfo *info);
		virtual void TAP_CDECL OnAPIReady();
		virtual void TAP_CDECL OnDisconnect(TAPIINT32 reasonCode);
		virtual void TAP_CDECL OnRspQryCommodity(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPIQuoteCommodityInfo *info);
		virtual void TAP_CDECL OnRspQryContract(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPIQuoteContractInfo *info);
		virtual void TAP_CDECL OnRspSubscribeQuote(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPIQuoteWhole *info);
		virtual void TAP_CDECL OnRspUnSubscribeQuote(TAPIUINT32 sessionID, TAPIINT32 errorCode, TAPIYNFLAG isLast, const TapAPIContract *info);
		virtual void TAP_CDECL OnRtnQuote(const TapAPIQuoteWhole *info);

	private:

		ITapQuoteAPI* m_pAPI;
		TAPIUINT32	m_uiSessionID;
		SimpleEvent m_Event;
		bool		m_bIsAPIReady;

	};

#endif // QUOTE_H
}