#include "TcpConnectionff.h"
#include "EventLoopff.h"
#include "Socketff.h"
#include "Channelff.h"
#include "Socketopsff.h"

//#include "Bufferff.h"

#include <string.h>
#include <stdio.h>

using namespace firey;

void defaultConnectionCallback(const TcpConnectionPtr& conn){
	printf("%s -> %s is %s",conn->localAddr().toIpPort().c_str(),
							conn->peerAddr().toIpPort().c_str(),
							(conn->connected()?"UP":"DOWN"));
	const char* buf="Hello , there is firey server!\n";
	conn->send(buf,sizeof(buf));
}

void defaultMessageCallback(const TcpConnectionPtr& conn,
							Bufferff* buffer,
							Timestampff receiveTime){
	buffer->retrieveAll();
}
	
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

void TcpConnectionff::send(const std::string& message){
	send(static_cast<const char*>(message.data()),
		 static_cast<size_t>(message.size()));
}

void TcpConnectionff::send(const void* data,size_t len){
	if(state_==kConnected){
		if(ownerLoop_->isInLoopThread()){
			sendInLoop(data,len);
		}
		else{
			ownerLoop_->runInLoop(
					std::bind(&TcpConnectionff::sendInLoop,shared_from_this().get(),
								data,len));
		}
	}
}

void TcpConnectionff::send(Bufferff* buff){
	if(state_==kConnected){
		if(ownerLoop_->isInLoopThread()){
			sendInLoop(buff->peek(),buff->readableBytes());
			buff->retrieveAll();
		}
		else{
			ownerLoop_->runInLoop(
					std::bind(&TcpConnectionff::sendInLoop,shared_from_this().get(),
								buff->peek(),buff->readableBytes()));
			buff->retrieveAll();
		}
	}
}

void TcpConnectionff::sendInLoop(const void* data,size_t len){
	ownerLoop_->assertInLoopThread();

	ssize_t nwrite=0;
	size_t remaining=len;
	bool faultError=false;

	if(state_==kDisconnected){
		return;
	}

	if(!connChannel_->isWriting()&&outputBuffer_.readableBytes()==0){
		nwrite=Socket::write(connChannel_->fd(),data,len);
		if(nwrite>=0){
			remaining=len-nwrite;
			if(remaining==0&&writeCompleteCallback_){
				ownerLoop_->queueInLoop(std::bind(writeCompleteCallback_,shared_from_this()));
			}
		}
		else{//nwite<0
			nwrite=0;
			if(errno!=EWOULDBLOCK&&errno!=EAGAIN){
				if(errno==EPIPE||errno==ECONNRESET){
					faultError=true;
				}
			}
		}
	}

	assert(remaining<len);
	if(!faultError&&remaining>0){
		size_t oldLen=outputBuffer_.readableBytes();

		if(oldLen+remaining>=highWaterMark_&&oldLen<highWaterMark_&&highWaterCallback_){
			ownerLoop_->queueInLoop(std::bind(highWaterCallback_,shared_from_this()));
		}

		outputBuffer_.append(static_cast<const char*>(data)+nwrite,remaining);

		if(!connChannel_->isWriting()){
			connChannel_->enableWriting();
		}
	}
}

//开始读
void  TcpConnectionff::startRead(){
	ownerLoop_->runInLoop(
			std::bind(&TcpConnectionff::startReadInLoop,this));
}


void TcpConnectionff::startReadInLoop(){
	ownerLoop_->assertInLoopThread();

	if(!reading_||!connChannel_->isReading()){
		connChannel_->enableReading();
		reading_=true;
	}
}

//结束读
void TcpConnectionff::stopRead(){
	ownerLoop_->runInLoop(
			std::bind(&TcpConnectionff::stopReadInLoop,this));
}

void TcpConnectionff::stopReadInLoop(){
	ownerLoop_->assertInLoopThread();

	if(reading_||connChannel_->isReading()){
		connChannel_->disableReading();
		reading_=false;
	}
}

void TcpConnectionff::connectionEstablished(){
	ownerLoop_->assertInLoopThread();

	assert(state_==kConnecting);
	setState(kConnected);
	
	connChannel_->tie(shared_from_this());
	
	connChannel_->enableReading();

	connectionCallback_(shared_from_this());
}

void TcpConnectionff::connectionDestroy(){
	ownerLoop_->assertInLoopThread();

	if(state_==kConnected){
		setState(kDisconnected);
		connChannel_->disableAll();
		connectionCallback_(shared_from_this());
	}
	
	connChannel_->remove();
}

void TcpConnectionff::handleRead(Timestampff receiveTime){
	ownerLoop_->assertInLoopThread();

	int saveErrno=0;

	ssize_t n=inputBuffer_.readFd(connChannel_->fd(),&saveErrno);
	if(n>0){
		messageCallback_(shared_from_this(),&inputBuffer_,receiveTime);
	}
	else if(n==0){
		handleClose();
	}
	else {
		errno=saveErrno;
		handleError();
	}
}

void TcpConnectionff::handleWrite(){
	ownerLoop_->assertInLoopThread();

	if(connChannel_->isWriting()){
		ssize_t n=Socket::write(connChannel_->fd(),
								outputBuffer_.peek(),
								outputBuffer_.readableBytes());
		if(n>0){
			outputBuffer_.retrieve(n);
			if(outputBuffer_.readableBytes()==0){
				connChannel_->disableWriting();
				if(writeCompleteCallback_){
					ownerLoop_->queueInLoop(std::bind(writeCompleteCallback_,shared_from_this()));
				}
				if(state_==kDisconnecting){
					//TODO
				}
			}
		}
		else {
			//TODO LOG_SYSERR<<
		}
	}
	else {
		//TODO LOG_TRACE<<
	}
}

void TcpConnectionff::handleClose(){
	ownerLoop_->assertInLoopThread();
	//TODO LOG_TRACE<<
	assert(state_==kConnected||state_==kDisconnecting);
	setState(kDisconnected);
	connChannel_->disableAll();
	TcpConnectionPtr guardThis(shared_from_this());

	connectionCallback_(guardThis);

	closeCallback_(guardThis);
}

void TcpConnectionff::handleError(){
	int err=Socket::getSocketError(connChannel_->fd());
	//TODO LOG_ERROR<
	fprintf(stderr,"TcpConnectionff::handleError(),%s",strerror(err));
}
