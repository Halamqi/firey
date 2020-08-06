#include "TcpConnectionff.h"

#include "EventLoopff.h"
#include "Socketff.h"
#include "Channelff.h"
#include "Socketopsff.h"
#include "Loggingff.h"

#include <string.h>

using namespace firey;

void defaultConnectionCallback(const TcpConnectionPtr& conn){
	LOG_TRACE<<conn->localAddr().toIpPort()<<" -> "
		<<conn->peerAddr().toIpPort()<<" is "
		<<(conn->connected()?"UP":"DOWN");
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
	LOG_DEBUG<<"TcpConnectionff::ctor["<<name_<<"] at "
		<<this<<" fd = "<<sockfd;
	connSocket_->setTcpKeepAlive(true);
}

TcpConnectionff::~TcpConnectionff(){
	LOG_DEBUG<<"TcpConnectionff::dtor["<<name_<<"] at "
		<<this
		<<"fd = "<<connSocket_->fd()
		<<" state = "<<stateToString();
	assert(state_==kDisconnected);
}

//TCP写数据-----------------------------------------------------------------------
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
					std::bind(&TcpConnectionff::sendInLoop,this,
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
					std::bind(&TcpConnectionff::sendInLoop,this,
								buff->peek(),buff->readableBytes()));
			buff->retrieveAll();
		}
	}
}

//所有的send最终调用的函数
void TcpConnectionff::sendInLoop(const void* data,size_t len){
	ownerLoop_->assertInLoopThread();

	ssize_t nwrite=0;
	size_t remaining=len;
	bool faultError=false;

	if(state_==kDisconnected){
		LOG_WARN<<"disconnected, give up writting";
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
				LOG_SYSERR<<"TcpConnectionff::sendInLoop()";
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
//TCP写数据---------------------------------------------------------------------------------------------

//关闭写端
void  TcpConnectionff::shutdown()
{
	if(state_==kConnected)
	{
		setState(kDisconnecting);
		ownerLoop_->runInLoop(
				std::bind(&TcpConnectionff::shutdownInLoop,this));
	}
}
void TcpConnectionff::shutdownInLoop()
{
	ownerLoop_->assertInLoopThread();
	//当没有关注可写事件时才关闭写端
	if(!connChannel_->isWriting())
	{
		connSocket_->shutdown();
	}
}

//主动关闭连接
void TcpConnectionff::forceClose()
{
	if(state_==kConnected||state_==kDisconnecting)
	{
		setState(kDisconnecting);
		ownerLoop_->runInLoop(
				std::bind(&TcpConnectionff::forceCloseInLoop,shared_from_this()));//FIXME shared_from_this()??
	}
}

void TcpConnectionff::forceCloseWithDelay(double seconds)
{
	if(state_==kConnected||state_==kDisconnecting)
	{
		setState(kDisconnecting);
		ownerLoop_->runAfter(seconds,
				std::bind(&TcpConnectionff::forceClose,shared_from_this()));//??
	}
}

void TcpConnectionff::forceCloseInLoop()
{
	ownerLoop_->assertInLoopThread();
	if(state_==kConnected||state_==kDisconnecting)
	{
		handleClose();
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

//连接建立成功
void TcpConnectionff::connectionEstablished(){
	ownerLoop_->assertInLoopThread();

	assert(state_==kConnecting);
	setState(kConnected);
	
	connChannel_->tie(shared_from_this());
	
	connChannel_->enableReading();

	connectionCallback_(shared_from_this());
}

//彻底断开连接
void TcpConnectionff::connectionDestroy(){
	ownerLoop_->assertInLoopThread();

	if(state_==kConnected){
		setState(kDisconnected);
		connChannel_->disableAll();
		connectionCallback_(shared_from_this());
	}
	
	connChannel_->remove();
}

//处理读事件
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
		LOG_SYSERR<<"TcpConnectionff::handleRead()";
		handleError();
	}
}

//处理写事件
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
					shutdownInLoop();
				}
			}
		}
		else {
			LOG_SYSERR<<"TcpConnectionff::handleWrite()";
		}
	}
	else {
		LOG_TRACE<<"Connection fd = "<<connChannel_->fd()
			<<"write is down, no more writing";
	}
}

//处理关闭连接事件
void TcpConnectionff::handleClose(){
	ownerLoop_->assertInLoopThread();
	LOG_TRACE<<"fd = "<<connChannel_->fd()<<" state = "<<stateToString();
	assert(state_==kConnected||state_==kDisconnecting);
	setState(kDisconnected);
	connChannel_->disableAll();
	TcpConnectionPtr guardThis(shared_from_this());

	connectionCallback_(guardThis);

	closeCallback_(guardThis);
}

//处理错误事件
void TcpConnectionff::handleError(){
	int err=Socket::getSocketError(connChannel_->fd());
	LOG_ERROR<<"TcpConnectionff::handleError() ["<<name_
		<<"] - SO_ERROR"<<err<<" "<<strerror_tl(err);
}


const char* TcpConnectionff::stateToString() const
{
	switch (state_)
	{
		case kDisconnected:
			return "Disconnected";
		case kConnecting:
			return "Connecting";
		case kConnected:
			return "Connected";
		case kDisconnecting:
			return "Disconnecting";
		default:
			return "unkown state";
	}
}

bool TcpConnectionff::getTcpInfo(struct tcp_info* tcpi) const
{
	return connSocket_->getTcpInfo(tcpi);
}

std::string TcpConnectionff::getTcpInfoString() const
{
	char buf[1024];
	buf[0]='\0';
	connSocket_->getTcpInfoString(buf,sizeof buf);
	return buf;
}
