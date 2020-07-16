#include "TcpConnectionff.h"
#include "EventLoopff.h"
#include "Socketff.h"
#include "Channelff.h"
#include "Socketopsff.h"

//#include "Bufferff.h"

#include <stdio.h>

using namespace firey;

void defaultConnectionCallback(const TcpConnectionPtr& conn){
	printf("%s -> %s is %s",conn->localAddr().toIpPort().c_str(),
							conn->peerAddr().toIpPort().c_str(),
							(conn->connected()?"UP":"DOWN"));
	const char* buf="Hello , there is firey server!\n";
	conn->send(buf,sizeof(buf));
}

/*void defaultMessageCallback(const TcpConnectionPtr& conn,
							Bufferff* buffer,
							TimeStampff receiveTime){
	conn
}*/
	
TcpConnectionff::TcpConnectionff(EventLoopff* loop,
								const std::string& name,
								int sockfd,
								const InetAddressff& localAddr,
								const InetAddressff& peerAddr)
	:ownerLoop_(loop),
	name_(name),
	localAddr_(localAddr),
	peerAddr_(peerAddr),
	state_(kConnecting),
	reading_(false),
	connChannel_(new Channelff(ownerLoop_,sockfd)),
	connSocket_(new Socketff(sockfd)),
	highWaterMark_(64*1024*1024)
{
	connChannel_->setReadCallback(
			std::bind(&TcpConnectionff::handleRead,this,_1));
	connChannel_->setWriteCallback(
			std::bind(&TcpConnectionff::handleWrite,this));
	connChannel_->setCloseCallback(
			std::bind(&TcpConnectionff::handleClose,this));
	connChannel_->setErrorCallback(
			std::bind(&TcpConnectionff::handleError,this));
	connSocket_->setTcpKeepAlive(true);
}

TcpConnectionff::~TcpConnectionff(){
	//LOG TODO
	assert(state_==kDisconnected);
}

void send(const std::string& message){
	
}
