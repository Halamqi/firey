#ifndef FF_SERVER_H_
#define FF_SERVER_H_

#include "Callbacksff.h"

#include <map>
#include <string>

namespace firey{

class Acceptorff;
class EventLoopff;
class InetAddressff;

class TcpServerff{
	public:

		enum Option{
			kNoReusePort,
			kReusePort,
		};

		TcpServerff(EventLoopff* loop,const InetAddressff& listenAddr,const std::string& name,Option reuseport=kNoReusePort);
		~TcpServerff();

		TcpServerff(const TcpServerff&)=delete;
		TcpServerff& operator=(const TcpServerff&)=delete;

		//get server information
		const std::string& serverName(){return name_;}
		const std::string& ipPort(){return ipPort_;}
		EventLoopff* getLoop(){return ownerLoop_;}


		void start();//start server

		//set server callbacks
		void setConnectionCallback(ConnectionCallback cb){
			connectionCallback_=std::move(cb);
		}
		
		void setMessageCallback(MessageCallback cb){
			messageCallback_=std::move(cb);
		}

		void setWriteCompleteCallback(WriteCompleteCallback cb){
			writeCompleteCallback_=std::move(cb);
		}

	private:
		EventLoopff* ownerLoop_;
		std::unique_ptr<Acceptorff> acceptor_;
		const std::string ipPort_;
		const std::string name_;

		ConnectionCallback connectionCallback_;
		MessageCallback messageCallback_;

		WriteCompleteCallback writeCompleteCallback_;
		HighWaterCallback highWaterCallback_;

		bool isRuning_;
		int nextConnId_;//server is always in main thread so no need to lock

		typedef std::map<std::string,TcpConnectionPtr> ConnectionMap;
		ConnectionMap connections_;

		void newConnection(int sockfd,const InetAddressff& peerAddr);
		void removeConnection(const TcpConnectionPtr& conn);
		void removeConnectionInLoop(const TcpConnectionPtr& conn);

};//class TcpServerff

}//namespace firey
#endif//FF_SERVER_H_
