#ifndef FF_HTTP_CONTEXT_H_
#define FF_HTTP_CONTEXT_H_

#include "HttpRequestff.h"

namespace firey{

class Bufferff;

class HttpContextff{
	public:
		enum HttpRequestParseState
		{
			kExpectRequestLine,
			kExpectHeaders,
			kExpectBody,
			kGotAll
		};//enum HttpRequestParseState

		HttpContext()
			:state_(kExpectRequestLine)
		{
		}

		//从Tcp连接的接收缓冲区中，解析出http请求
		bool parseRequest(Bufferff* buf,Timestampff receiveTime);

		bool gotAll() const{return state_==kGotAll;}

		//处理完一个http请求之后，重置httpContext的内容
		void reset()
		{
			state_=kExpectRequestLine;
			HttpRequestff dummy;
			request_.swap(dummy);
		}

		const HttpRequest& request() const
		{return request_;}

		HttpRequest& request()
		{return request_;}

	private:
		//处理请求行
		bool processRequestLine(const char* begin,const char* end);
		
		HttpRequestParseState state_;
		HttpRequestff request_;
};//class HttpContextff

}//namespace firey;
#endif //FF_HTTP_CONTEXT_H_
