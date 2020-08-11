#ifndef FF_HTTP_SERVER_H_
#define FF_HTTP_SERVER_H_

#include "TcpServerff.h"

#include <functional>
#include <string>

namespace firey{

class HttpRequestff;
class HttpResponseff;

class HttpServerff{

	public:
		typedef std::function<void(const HttpRequestff&,HttpResponseff*)> HttpCallback;
		HttpServerff(const HttpServerff&)=delete;
		HttpServerff& operator=(const HttpServerff&)=delete;

		HttpServerff(EventLoopff* loop,
					 const InetAddressff& listenAddr,
					 const std::string& name,
					 TcpServerff::Option option=TcpServerff::kNoReusePort);
		EventLoopff* getLoop(){return server_.getLoop();}

		void setHttpCalback(const HttpCallback& cb)
		{
			httpCallback_=cb;
		}

		void setThreadNum(int nums){server_.setThreadNum(nums);}

		void start();

	private:
		void onConnection(const TcpConnectionPtr& conn);
		void onMessage(const TcpConnectionPtr& conn,
					   Bufferff* buffer,
					   Timestampff receiveTime);
		//处理http请求
		void onRequest(const TcpConnecionPtr&,const HttpRequest&);
		
		TcpServerff server_;
		//http服务器最主要的逻辑
		HttpCallback httpCallback_;

};//class TcpServerff

}//namespace firey

#endif //FF_HTTP_SERVER_H_
