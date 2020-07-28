#ifndef FF_SERVER_H_
#define FF_SERVER_H_

#include "Callbacksff.h"
#include "TcpConnectionff.h"

#include <map>
#include <string>

namespace firey{

class Acceptorff;
class EventLoopff;
class InetAddressff;
class EventLoopThreadPoolff;

class TcpServerff{
	public:
	
		typedef std::function<void(EventLoopff*)> ThreadInitCallback;
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

		/*对io线程池的操作*/
		//设置io线程池线程的数量
		void setThreadNum(int threadNum);
		//在io线程在loop之前，使用EventLoop完成一些事情
		void setThreadInitCallback(const ThreadInitCallback& cb){
			threadInitCallback_=cb;
		}
		//获取服务器的io线程池
		std::shared_ptr<EventLoopThreadPoolff> threadPool(){
			return ioThreadPool_;
		}

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

		std::shared_ptr<EventLoopThreadPoolff> ioThreadPool_;

		ThreadInitCallback threadInitCallback_;

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
