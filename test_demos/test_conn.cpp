#include "EventLoopff.h"
#include "TcpServerff.h"

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
			printf("%s-->%s is %s\n",conn->peerAddr().toIpPort().data(),
								   conn->localAddr().toIpPort().data(),
								   (conn->connected()?"UP":"DOWN"));
			if(conn->connected())
				conn->send(message,strlen(message));
			else conn->send(close,strlen(close));
		}

		void onMessage(const TcpConnectionPtr& conn,
						Bufferff* buffer,
						Timestampff receiveTime){
			std::string msg=buffer->retrieveAllAsString();
			printf("%s send a message: %s",conn->peerAddr().toIpPort().data(),msg.data());
			conn->send(msg);
			abort();
		}
};

int main(){
	EventLoopff loop;
	InetAddressff serverAddr(12345);

	echoServer server(&loop,serverAddr);
	server.start();
	loop.loop();
	return 0;
}
