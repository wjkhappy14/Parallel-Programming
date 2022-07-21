#include "CRedisConn.h"
namespace RedisCpp
{
	// ����������
	const char* CRedisConn::_errDes[ERR_BOTTOM] =
	{
			"No error.",
			"NULL pointer ",
			"No connection to the redis server.",
					"Inser Error,pivot not found.",
					"key not found",
			"hash field not found",
			"error index"
	};

	CRedisConn::CRedisConn()
	{
		_redCtx = NULL;
		_host.clear();
		_password.clear();
		_port = 0;
		_timeout = 0;
		_connected = false;
		_errStr = _errDes[ERR_NO_ERROR];
	}

	void CRedisConn::init(const std::string& host, const uint16_t port, const std::string& password,
		const uint32_t timeout)
	{
		_host = host;
		_port = port;
		_password = password;
		_timeout = timeout;
	}

	bool CRedisConn::_getError(const redisReply* reply)  throw (NullReplyException)
	{
		_errStr = _errDes[ERR_NO_ERROR];
		if (reply == NULL)
		{
			_errStr = _errDes[ERR_NULL];
			throw NullReplyException();
		}
		// have error
		if (reply->type == REDIS_REPLY_ERROR)
		{
			_errStr = reply->str;
			return true;
		}
		else
		{
			return false;
		}
	}

	bool CRedisConn::_getError(const redisContext* redCtx)
	{
		_errStr = _errDes[ERR_NO_ERROR];
		if (redCtx == NULL)
		{
			_errStr = _errDes[ERR_NULL];
			return true;
		}
		if (redCtx->err != 0)
		{
			_errStr = redCtx->errstr;
			return true;
		}
		else
		{
			return false;
		}
	}

	bool CRedisConn::auth(const std::string& password)  throw (NullReplyException)
	{
		if (!_connected)
		{
			_errStr = _errDes[ERR_NO_CONNECT];
			return false;
		}

		bool ret = false;
		_password = password;
		redisReply *reply = static_cast<redisReply *>(redisCommand(_redCtx, "AUTH %s",
			_password.c_str()));

		if (_getError(reply))
		{
			ret = false;
		}
		else
		{
			ret = true;
		}

		if (NULL != reply)
		{
			freeReplyObject(reply);
		}
		return ret;
	}

	bool CRedisConn::connect(void)
	{
		if (_connected)
		{
			disConnect();
		}

		struct timeval timeoutVal;
		timeoutVal.tv_sec = _timeout;
		timeoutVal.tv_usec = 0;

		_redCtx = redisConnectWithTimeout(_host.c_str(), _port, timeoutVal);
		if (_getError(_redCtx))
		{
			if (NULL != _redCtx)
			{
				redisFree(_redCtx);
				_redCtx = NULL;
			}
			_connected = false;
			return false;
		}

		_connected = true;
		return true;

		// if connection  need password
	/*
		if ( _password == "" )
		{
			return true;
		}
		else
		{
			return ( auth( _password ) );   //< ��Ȩʧ��Ҳ������false
		}
	*/
	}

	void CRedisConn::disConnect()
	{
		if (_connected && NULL != _redCtx)
		{
			redisFree(_redCtx);
			_redCtx = NULL;
		}
		_connected = false;
	}

	bool CRedisConn::connect(const std::string &host, const uint16_t port,
		const std::string& password, const uint32_t timeout)
	{
		// Init attribute.
		init(host, port, password, timeout);

		return (connect());
	}

	bool CRedisConn::ping()
	{
		if (!_connected || !_redCtx)
		{
			_errStr = _errDes[ERR_NO_CONNECT];
			return false;
		}

		redisReply *reply = static_cast<redisReply *>(redisCommand(_redCtx, "PING"));
		bool ret = false;

		if (_getError(reply))
		{
			ret = false;
		}
		else
		{
			ret = true;
		}

		if (NULL != reply)
		{
			freeReplyObject(reply);
		}

		return ret;
	}

	bool CRedisConn::reconnect()
	{
		return (connect());
	}

	const std::string CRedisConn::getErrorStr() const
	{
		return _errStr;
	}

	redisReply* CRedisConn::redisCmd(const char *format, ...)
	{
		va_list ap;
		va_start(ap, format);
		redisReply* reply = static_cast<redisReply *>(redisvCommand(_redCtx, format, ap));
		va_end(ap);
		return reply;
	}

	CRedisConn::~CRedisConn()
	{
		disConnect();
	}

	////////////////////////////////// list ��ķ��� ////////////////////////////////////////

	bool CRedisConn::lpush(const std::string& key, const std::string& value, uint64_t& retval)
		throw (NullReplyException)
	{
		if (!_connected || !_redCtx)
		{
			_errStr = _errDes[ERR_NO_CONNECT];
			return false;
		}

		retval = 0;
		bool ret = false;

		redisReply *reply = redisCmd("LPUSH %s %s", key.c_str(), value.c_str());

		if (_getError(reply))
		{
			ret = false;
		}
		else
		{
			retval = reply->integer;
			ret = true;
		}

		if (NULL != reply)
		{
			freeReplyObject(reply);
		}
		return ret;

	}

	bool CRedisConn::lpop(const std::string& key, std::string& value) throw (NullReplyException)
	{
		if (!_connected || !_redCtx)
		{
			_errStr = _errDes[ERR_NO_CONNECT];
			return false;
		}

		bool ret = false;
		redisReply *reply = redisCmd("LPOP %s", key.c_str());

		if (_getError(reply))
		{
			ret = false;
		}
		else
		{
			// ʧ��
			if (NULL == reply->str)
			{
				_errStr = _errDes[ERR_NO_KEY];
				value = "";
				ret = false;
			}
			else
			{
				value = reply->str;
				ret = true;
			}
		}

		if (NULL != reply)
		{
			freeReplyObject(reply);
		}

		return ret;
	}

	bool CRedisConn::_getArryToList(redisReply* reply, ValueList& valueList)
	{
		if (NULL == reply)
		{
			return false;
		}

		std::size_t num = reply->elements;

		for (std::size_t i = 0; i < num; i++)
		{
			valueList.push_back(reply->element[i]->str);
		}

		//	ValueList::iterator it = valueList.begin();
		//
		//	for( ; it != valueList.end(); it++ )
		//	{
		//		std::cout << "valueList: "<< *it << std::endl;
		//	}
		return true;
	}

	bool CRedisConn::_getArryToMap(redisReply* reply, ValueMap& valueMap)
	{
		if (NULL == reply)
		{
			return false;
		}

		std::size_t num = reply->elements;

		for (std::size_t i = 0; i < num; i += 2)
		{
			valueMap.insert(
				std::pair<std::string, std::string>(reply->element[i]->str,
					reply->element[i + 1]->str));
		}

		return true;
	}

	bool CRedisConn::lrange(const std::string &key, uint32_t start, int32_t end,
		ValueList& valueList) throw (NullReplyException)
	{
		if (!_connected || !_redCtx)
		{
			_errStr = _errDes[ERR_NO_CONNECT];
			return false;
		}

		bool ret = false;
		redisReply *reply = redisCmd("LRANGE %s %d %d", key.c_str(), start, end);

		if (_getError(reply))
		{
			ret = false;
		}
		else
		{
			if (REDIS_REPLY_ARRAY == reply->type && 0 == reply->elements) //<  key��list���͵� start > end
			{
				_errStr = std::string(_errDes[ERR_INDEX]) + " or "
					+ _errDes[ERR_NO_KEY];
				ret = false;

			}
			else
			{
				_getArryToList(reply, valueList);
				ret = true;
			}
		}

		if (NULL != reply)
		{
			freeReplyObject(reply);
		}

		return ret;
	}

	bool CRedisConn::rpush(const std::string& key, const std::string& value, uint64_t& retval)
		throw (NullReplyException)
	{
		if (!_connected || !_redCtx)
		{
			_errStr = _errDes[ERR_NO_CONNECT];
			return false;
		}

		retval = 0;
		bool ret = false;

		redisReply *reply = redisCmd("RPUSH %s %s", key.c_str(), value.c_str());

		if (_getError(reply))
		{
			ret = false;
		}
		else
		{
			retval = reply->integer;
			ret = true;
		}

		if (NULL != reply)
		{
			freeReplyObject(reply);
		}

		return ret;
	}

	bool CRedisConn::rpop(const std::string& key, std::string& value) throw (NullReplyException)
	{
		if (!_connected || !_redCtx)
		{
			_errStr = _errDes[ERR_NO_CONNECT];
			return false;
		}

		bool ret = false;
		redisReply *reply = redisCmd("RPOP %s", key.c_str());

		if (_getError(reply))
		{
			ret = false;
		}
		else
		{
			// ʧ��
			if (NULL == reply->str)
			{
				_errStr = _errDes[ERR_NO_KEY];
				value = "";
				ret = false;
			}
			else
			{
				value = reply->str;
				ret = true;
			}
		}

		if (NULL != reply)
		{
			freeReplyObject(reply);
		}

		return ret;
	}

	bool CRedisConn::lindex(const std::string& key, int32_t index, std::string& value)
		throw (NullReplyException)
	{
		if (!_connected || !_redCtx)
		{
			_errStr = _errDes[ERR_NO_CONNECT];
			return false;
		}

		bool ret = false;
		redisReply *reply = redisCmd("LINDEX %s %d", key.c_str(), index);

		if (_getError(reply))
		{
			ret = false;
		}
		else
		{
			// ʧ��
			if (REDIS_REPLY_NIL == reply->type)
			{
				_errStr = std::string(_errDes[ERR_NO_KEY]) + " or " +
					_errDes[ERR_INDEX];
				value = "";
				ret = false;
			}
			else
			{
				value = reply->str;
				ret = true;
			}
		}

		if (NULL != reply)
		{
			freeReplyObject(reply);
		}

		return ret;
	}

	bool CRedisConn::linsert(const std::string& key, INSERT_POS position, const std::string& pivot,
		const std::string& value, int64_t& retval) throw (NullReplyException)
	{
		if (!_connected || !_redCtx)
		{
			_errStr = _errDes[ERR_NO_CONNECT];
			return false;
		}

		std::string pos;

		if (BEFORE == position)
		{
			pos = "BEFORE";
		}
		else if (AFTER == position)
		{
			pos = "AFTER";
		}

		bool ret = false;
		redisReply *reply = redisCmd("LINSERT %s %s %s %s", key.c_str(), pos.c_str(),
			pivot.c_str(), value.c_str());

		if (_getError(reply))	//< ����list ����
		{
			ret = false;
		}
		else
		{
			if (REDIS_REPLY_INTEGER == reply->type)
			{
				if (reply->integer == -1)
				{
					_errStr = _errDes[ERR_NO_PIVOT];
					ret = false;
				}
				else if (reply->integer == 0)
				{
					_errStr = _errDes[ERR_NO_KEY];
					ret = false;
				}
				else
				{
					retval = reply->integer;
					ret = true;
				}
			}
			else
			{
				ret = false;
			}
		}

		if (NULL != reply)
		{
			freeReplyObject(reply);
		}

		return ret;
	}

	bool CRedisConn::llen(const std::string& key, uint64_t& retval) throw (NullReplyException)
	{
		if (!_connected || !_redCtx)
		{
			_errStr = _errDes[ERR_NO_CONNECT];
			return false;
		}

		bool ret = false;
		redisReply *reply = redisCmd("LLEN %s", key.c_str());

		if (_getError(reply))
		{
			ret = false;
		}
		else
		{
			if (REDIS_REPLY_INTEGER == reply->type && (0 == reply->integer))
			{
				_errStr = _errDes[ERR_NO_KEY];
				ret = false;
			}
			else
			{
				retval = reply->integer;
				ret = true;
			}
		}

		if (NULL != reply)
		{
			freeReplyObject(reply);
		}

		return ret;
	}

	//////////////////hash����/////////////////////////////
	bool CRedisConn::hget(const std::string& key, const std::string& filed, std::string& value)
		throw (NullReplyException)
	{
		if (!_connected || !_redCtx)
		{
			_errStr = _errDes[ERR_NO_CONNECT];
			return false;
		}

		bool ret = false;
		redisReply *reply = redisCmd("HGET %s %s", key.c_str(), filed.c_str());

		if (_getError(reply))
		{
			ret = false;
		}
		else
		{
			if (REDIS_REPLY_NIL == reply->type)
			{
				_errStr = std::string(_errDes[ERR_NO_KEY]) + " or " +
					_errDes[ERR_NO_FIELD];
				ret = false;
			}
			else
			{
				value = reply->str;
				ret = true;
			}
		}
		if (NULL != reply)
		{
			freeReplyObject(reply);
		}
		else
		{

		}

		return ret;
	}

	bool CRedisConn::hset(const std::string& key, const std::string& filed,
		const std::string& value, uint32_t& retval) throw (NullReplyException)
	{
		if (!_connected || !_redCtx)
		{
			_errStr = _errDes[ERR_NO_CONNECT];
			return false;
		}

		bool ret = false;
		redisReply *reply = redisCmd("HSET %s %s %s", key.c_str(), filed.c_str(),
			value.c_str());

		if (_getError(reply))
		{
			ret = false;
		}
		else
		{
			retval = reply->integer;
			ret = true;
		}

		if (NULL != reply)
		{
			freeReplyObject(reply);
		}

		return ret;

	}

	bool CRedisConn::hdel(const std::string& key, const std::string& filed, uint32_t& retval)
		throw (NullReplyException)
	{
		if (!_connected || !_redCtx)
		{
			_errStr = _errDes[ERR_NO_CONNECT];
			return false;
		}

		bool ret = false;
		redisReply *reply = redisCmd("HDEL %s %s", key.c_str(), filed.c_str(), retval);

		if (_getError(reply))
		{
			ret = false;
		}
		else
		{
			if (REDIS_REPLY_INTEGER == reply->type && 0 == reply->integer)
			{
				_errStr = std::string(_errDes[ERR_NO_KEY]) + " or " + _errDes[ERR_NO_FIELD];
			}
			//std::cout<<"type = "<<reply->type<<"   integer = "<< reply->integer<<std::endl;
			//std::cout<<"str = " << reply->str<<std::endl;
			else
			{
				retval = reply->integer;
				ret = true;
			}
		}

		if (NULL != reply)
		{
			freeReplyObject(reply);
		}

		return ret;
	}

	bool CRedisConn::hgetall(const std::string& key, ValueMap& valueMap) throw (NullReplyException)
	{
		if (!_connected || !_redCtx)
		{
			_errStr = _errDes[ERR_NO_CONNECT];
			return false;
		}

		bool ret = false;
		redisReply *reply = redisCmd("HGETALL %s", key.c_str());

		if (_getError(reply))
		{
			ret = false;
		}
		else
		{

			if (REDIS_REPLY_ARRAY == reply->type && 0 == reply->elements)
			{
				_errStr = _errDes[ERR_NO_KEY];

			}
			else
			{
				_getArryToMap(reply, valueMap);
				ret = true;
			}
		}

		if (NULL != reply)
		{
			freeReplyObject(reply);
		}

		return ret;
	}

} /* namespace RedisCpp */
