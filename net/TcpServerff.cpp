#include "TcpServerff.h"
#include "EventLoopff.h"
#include "Acceptorff.h"
#include "InetAddressff.h"
#include "TcpConnectionff.h"

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
	isRuning_(false),
	nextConnId_(1)
{
	acceptor_->setNewConnectionCallback(
			std::bind(&TcpServerff::newConnection,this,_1,_2));		
}

TcpServerff::~TcpServerff(){
	ownerLoop_->assertInLoopThread();

	for(auto& item:connections_){
		TcpConnectionPtr conn(item.second);
		item.second.reset();
		//conn->getLoop()->runInLoop(
		//		std::bind(&TcpConnectionff::connectionDestroy,conn));
	}
}

void TcpServerff::newConnection(int sockfd,const InetAddressff& peerAddr){
	ownerLoop_->assertInLoopThread();

	char buf[64];//store server information
	snprintf(buf,sizeof buf,"-%s#%d",ipPort_.c_str(),nextConnId_);
	nextConnId_++;
	std::string connName=name_+buf;

	InetAddressff localAddr(Socket::getLocalAddr(sockfd));
	TcpConnectionPtr conn(new TcpConnectionff(ownerLoop_,connName,sockfd,localAddr,peerAddr));
	connections_[connName]=conn;
	conn->setConnectionCallback(connectionCallback_);
	conn->setMessageCallback(messageCallback_);
	conn->setWriteCompleteCallback(writeCompleteCallback_);
	conn->setCloseCallback(
			std::bind(&TcpServerff::removeConnection,this,_1));
//	ownerLoop_->runInLoop(
//			std::bind(&TcpConnectionff::connectionEstablished,conn));
}

void TcpServerff::start(){
	if(!isRuning_){
		isRuning_=true;
		assert(!acceptor_->listening());
		ownerLoop_->runInLoop(
				std::bind(&Acceptorff::listen,acceptor_.get()));
	}
		
}

void TcpServerff::removeConnection(const TcpConnectionPtr& conn){
	ownerLoop_->runInLoop(
			std::bind(&TcpServerff::removeConnectionInLoop,this,conn));
}

void TcpServerff::removeConnectionInLoop(const TcpConnectionPtr& conn){
	ownerLoop_->assertInLoopThread();

	//TODO

	size_t n=connections_.erase(conn->name());
	(void) n;
	assert(n==1);

//	EventLoopff* ioLoop=conn->getLoop();
//	ioLoop->runInLoop(
//			std::bind(&TcpConnectionff::connectionDestroy,conn));
}
