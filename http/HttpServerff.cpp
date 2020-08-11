#include "HttpServerff.h"

#include "EventLoopff.h"
#include "Loggingff.h"
#include "HttpContextff.h"
#include "HttpRequestff.h"
#include "HttpResponseff.h"

namespace firey{
	void defaultHttpCallback(const HttpRequestff& req,HttpResponseff* resp)
	{
		resp->setStatusCode(HttpResponseff::k404NotFound);
		resp->setStatusMessage("Not Found");
		resp->setCloseConnection(true);
	}
}

using namespace firey;

HttpServerff::HttpServerff(EventLoopff* loop,
						   const InetAddressff& listenAddr,
						   const std::string& name,
						   TcpServerff::Option option)
	:server_(loop,listenAddr,name,option),
	httpCallback_(defaultHttpCallback)
{
	server_.setConnectionCallback(
			std::bind(&HttpServerff::onConnection,this,_1));
	server_.setMessageCallback(
			std::bind(&HttpServerff::onMessage,this,_1,_2,_3));
}

void HttpServerff::start()
{
	LOG_WARN<<"HttpServer ["<<server_.serverName()
		<<"] starts listenning on "<<server_.ipPort();
	server_.start();
}

//将TCP连接中的context_设置为HttpContext
void HttpServerff::onConnection(const TcpConnectionPtr& conn)
{
	if(conn->connected())
	{
		conn->setContext(HttpContextff());
	}
}

void HttpServerff::onMessage(const TcpConnectionPtr& conn,
							 Bufferff* buffer,
							 Timestampff receiveTime)
{
	HttpContextff* context=boost::any_cast<HttpContextff>(conn->getMutableContext());

	//如果请求体出错
	if(!context->parseRequest(buffer,receiveTime))
	{
		conn->send("HTTP/1.1 400 Bad Request\r\n\r\n");
		conn->shutdown();
	}
	if(context->gotAll())
	{
		//???
		onRequest(conn,context->request());
		context->reset();
	}
}

void HttpServerff::onRequest(const TcpConnectionPtr& conn,
							 const HttpRequestff& request)
{
	//确定这个HTTP请求是长连接还是短连接
	const std::string& connection=request.getHeader("Connection");
	bool close=connection=="close"||
			   (request.getVersion()==HttpRequestff::kHttp10&&connection!="Keep-Alive");
	//根据长连接和短连接的不同，来对HTTP请求进行应答
	HttpResponseff response(close);

	//用户设置的回调，也就是这个Http服务器，具有哪些功能
	httpCallback_(request,&response);

	Bufferff buf;

	response.appendToBuffer(&buf);
	conn->send(&buf);
	if(response.closeConnection())
	{
		conn->shutdown();
	}
}
