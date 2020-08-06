#include "TcpServerff.h"
#include "EventLoopff.h"
#include "Acceptorff.h"
#include "InetAddressff.h"
#include "EventLoopThreadPoolff.h"
#include "Loggingff.h"

#include <stdio.h>

using namespace firey;

TcpServerff::TcpServerff(EventLoopff* loop,
		const InetAddressff& listenAddr,
		const std::string& name,
		Option reuseport)
	:ownerLoop_(loop),
	acceptor_(new Acceptorff(loop,listenAddr,reuseport==kReusePort)),
	ipPort_(listenAddr.toIpPort()),
	name_(name),
	//注意成员初始化顺序
	//ioThreadPool_必须要在name_之后初始化，因为用到了name_
	//或者在初始化EventLoopThreadPoolff时不使用name_
	ioThreadPool_(new EventLoopThreadPoolff(ownerLoop_,name_)),
	isRuning_(false),
	nextConnId_(1)
{
	acceptor_->setNewConnectionCallback(
			std::bind(&TcpServerff::newConnection,this,_1,_2));
	LOG_DEBUG<<"TcpServerff::TcpServerff() ["<<name_<<"] constructing";
}

TcpServerff::~TcpServerff(){
	ownerLoop_->assertInLoopThread();
	LOG_TRACE<<"TcpServerff::~TcpServerff() ["<<name_<<"] destructing";
	for(auto& item:connections_){
		TcpConnectionPtr conn(item.second);
		item.second.reset();
		conn->getLoop()->runInLoop(
				std::bind(&TcpConnectionff::connectionDestroy,conn));
	}
}

void TcpServerff::newConnection(int sockfd,const InetAddressff& peerAddr){
	ownerLoop_->assertInLoopThread();

	char buf[64];//store server information
	snprintf(buf,sizeof buf,"-%s#%d",ipPort_.c_str(),nextConnId_);
	nextConnId_++;
	std::string connName=name_+buf;

	LOG_INFO<<"TcpServer::newConnection() ["<<name_
		<<"] - new connection ["<<connName
		<<"] from "<<peerAddr.toIpPort();

	//新的TCP连接到来，接受并分发到IO线程池中
	InetAddressff localAddr(Socket::getLocalAddr(sockfd));
	EventLoopff* ioLoop=ioThreadPool_->getNextLoop();
	TcpConnectionPtr conn(new TcpConnectionff(ioLoop,
									          connName,
											  sockfd,
											  localAddr,
											  peerAddr));
	connections_[connName]=conn;
	conn->setConnectionCallback(connectionCallback_);
	conn->setMessageCallback(messageCallback_);
	conn->setWriteCompleteCallback(writeCompleteCallback_);
	conn->setCloseCallback(
			std::bind(&TcpServerff::removeConnection,this,_1));
	ioLoop->runInLoop(
			std::bind(&TcpConnectionff::connectionEstablished,conn));
}

void TcpServerff::start(){
	if(!isRuning_){
		ioThreadPool_->start(threadInitCallback_);
		isRuning_=true;
		assert(!acceptor_->listening());
		ownerLoop_->runInLoop(
				std::bind(&Acceptorff::listen,acceptor_.get()));
	}
	LOG_DEBUG<<"Server ["<<name_<<"] is started"
		<<(ioThreadPool_->threadNum()>0?" with a iothreadpool":"");
}

void TcpServerff::removeConnection(const TcpConnectionPtr& conn){
	ownerLoop_->runInLoop(
			std::bind(&TcpServerff::removeConnectionInLoop,this,conn));
}

void TcpServerff::removeConnectionInLoop(const TcpConnectionPtr& conn){
	ownerLoop_->assertInLoopThread();

	LOG_INFO<<"TcpServerff::removeConnectionInLoop() ["<<name_
		<<"] - connection"<<conn->name();

	size_t n=connections_.erase(conn->name());
	(void) n;
	assert(n==1);

	EventLoopff* ioLoop=conn->getLoop();
	/*KEY!!! use queueInLoop instead of runInLoop*/
	ioLoop->queueInLoop(
			std::bind(&TcpConnectionff::connectionDestroy,conn));
}

//设置线程池的大小
void TcpServerff::setThreadNum(int threadNum){
	assert(threadNum>=0);
	ioThreadPool_->setThreadNum(threadNum);		
}
