#include "Acceptorff.h"

#include "EventLoopff.h"
#include "InetAddressff.h"
#include "Loggingff.h"

#include <fcntl.h>
#include <assert.h>

using namespace firey;

Acceptorff::Acceptorff(EventLoopff* loop,const InetAddressff& listenAddr,bool reuseport)
	:ownerLoop_(loop),
	acceptSocket_(Socket::createNonblockingOrDie()),
	acceptChannel_(ownerLoop_,acceptSocket_.fd()),
	listening_(false),
	idleFd_(::open("/dev/null",O_RDONLY|O_CLOEXEC))
{
	assert(idleFd_>=0);
	acceptSocket_.setTcpReuseAddr(true);
	acceptSocket_.setTcpReusePort(reuseport);
	acceptSocket_.bindAddress(listenAddr);

	acceptChannel_.setReadCallback(
			std::bind(&Acceptorff::handleRead,this));
}

Acceptorff::~Acceptorff(){
	acceptChannel_.disableAll();
	acceptChannel_.remove();
	::close(idleFd_);
}

void Acceptorff::listen(){
	ownerLoop_->assertInLoopThread();

	listening_=true;
	acceptSocket_.listen();
	acceptChannel_.enableReading();
}

void Acceptorff::handleRead(){
	ownerLoop_->assertInLoopThread();

	InetAddressff peerAddr;
	int connfd=acceptSocket_.accept(peerAddr);
	if(connfd>=0){
		if(newConnectionCallback_) newConnectionCallback_(connfd,peerAddr);
	
		else Socket::closeOrDie(connfd);
	}
	else{
		LOG_SYSERR<<"in Acceptor::handleRead()";
		//处理文件描述数达到上限的问题
		if(errno==EMFILE){
			::close(idleFd_);
			idleFd_=::accept(acceptSocket_.fd(),NULL,NULL);
			::close(idleFd_);
			idleFd_=::open("dev/null",O_RDONLY|O_CLOEXEC);
		}
	}
}
