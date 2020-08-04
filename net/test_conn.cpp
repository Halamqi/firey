#include "EventLoopff.h"
#include "TcpServerff.h"
#include "Loggingff.h"
#include "AsyncLoggingff.h"

#include <stdio.h>

using namespace firey;

class echoServer{
	private:
		EventLoopff* loop_;
		TcpServerff server_;

	public:
		echoServer(EventLoopff* loop,const InetAddressff& addr,const std::string& name="echoServer")
			:loop_(loop),
			server_(loop,addr,name)
			{
				server_.setConnectionCallback(
						std::bind(&echoServer::onConnection,this,_1));
				server_.setMessageCallback(
						std::bind(&echoServer::onMessage,this,_1,_2,_3));
			}
		
		void start(){
			server_.start();
		}

		void onConnection(const TcpConnectionPtr& conn){
			const char* message="there is firey echo server,please enter a message:\n";
			const char* close="happy for servering you,GOODBYE\n";
			LOG_INFO<<conn->peerAddr().toIpPort().c_str()<<" --> "<<
					  conn->localAddr().toIpPort().c_str()
					  <<(conn->connected()?"UP":"DOWN");
			if(conn->connected())
				conn->send(message,strlen(message));
			else conn->send(close,strlen(close));
		}

		void onMessage(const TcpConnectionPtr& conn,
						Bufferff* buffer,
						Timestampff receiveTime){
			std::string msg=buffer->retrieveAllAsString();
			LOG_INFO<<conn->peerAddr().toIpPort().c_str()<<" send a message: "<<msg.c_str();
			conn->send(msg);
		}
};

const int kRollSize=1024*1024*1024;

std::unique_ptr<AsyncLoggingff> g_async;

void AsyncOutput(const char* logline,int len)
{
	g_async->append(logline,len);
}

void setLogging(const char* basename)
{
	Loggerff::setOutput(AsyncOutput);
	g_async.reset(new AsyncLoggingff(::basename(basename),kRollSize));
	g_async->start();
}

int main(int argc,char* argv[]){
	
	setLogging(argv[0]);

	EventLoopff loop;
	InetAddressff serverAddr(12345);

	echoServer server(&loop,serverAddr);
	server.start();
	loop.loop();
	return 0;
}
