#include "Socketopsff.h"
#include "Loggingff.h"

#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include <assert.h>

using namespace firey;

#if VALGRIND || defined (NO_ACCEPT4)
void setNonBlockingCloseOnExec(int sockfd)
{
	int flags=::fcntl(sockfd,F_GETFL,0);
	flags|=O_NOBLOCK;
	int ret=::fcntl(sockfd,F_SETFL,flags);

	flags=::fcntl(sockfd,F_GETFD,0);
	flags|=FD_CLOEXEC;
	int ret=::fcntl(sockfd,F_SETFD,flags);

	(void)ret;
}
#endif

int Socket::createNonblockingOrDie(){
#if VALGRIND
	int sockfd=::socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if(sockfd<0)
	{
		LOG_SYSFATAL<<"Socket::createNonblockingOrDie()";
	}
	setNonBlockingCloseOnExec(sockfd);
#else
	int sockfd=::socket(AF_INET,SOCK_STREAM|SOCK_NONBLOCK|SOCK_CLOEXEC,IPPROTO_TCP);
	if(sockfd<0){
		LOG_SYSFATAL<<"Socket::createNonblockingOrDie()";
	}
#endif
	return sockfd;
}

void Socket::bindOrDie(int sockfd,const struct sockaddr_in* addr){
	int ret=::bind(sockfd,reinterpret_cast<const struct sockaddr*>(addr),
			static_cast<socklen_t>(sizeof(struct sockaddr_in)));
	if(ret<0){
		LOG_SYSFATAL<<"Socket::bindOrDie()";
	}
}

void Socket::listenOrDie(int sockfd){
	int ret=::listen(sockfd,128);
	if(ret<0){
		LOG_SYSFATAL<<"Socket::listenOrDie()";
	}
}

int Socket::accept(int sockfd,struct sockaddr_in* addr){
	socklen_t addrlen=static_cast<socklen_t>(sizeof(*addr));
#if VALGRIND || defined (NO_ACCEPT4)
	int connfd=::accept(sockfd,(struct sockaddr*)addr,&addrlen);
	setNonBlockingCloseOnExec(connfd);
#else
	int connfd=::accept4(sockfd,reinterpret_cast<struct sockaddr*>(addr),
			&addrlen,SOCK_NONBLOCK|SOCK_CLOEXEC); 
#endif
	if(connfd<0){
		int savedErrno=errno;
		LOG_SYSERR<<"Socket::accpet()";
		switch (savedErrno){
			case EAGAIN:
			case ECONNABORTED:
			case EINTR:
			case EPROTO: // ???
			case EPERM:
				//超过了进程所能打开的文件描述上限
			case EMFILE: // per-process lmit of open file desctiptor ???
				// expected errors
				errno = savedErrno;
				break;
			case EBADF:
			case EFAULT:
			case EINVAL:
			case ENFILE:
			case ENOBUFS:
			case ENOMEM:
			case ENOTSOCK:
			case EOPNOTSUPP:
				LOG_FATAL<<"unexpected error of ::accept"<<savedErrno;
				break;
			default:
				LOG_FATAL<<"unknown error of ::accept"<<savedErrno;
				break;
		}
	}
	return connfd;
}

void Socket::closeOrDie(int sockfd){
	int ret=::close(sockfd);
	if(ret<0){
		LOG_SYSERR<<"Socket::closeOrDie()";
	}
}
void Socket::shutdownWrite(int sockfd){
	if(::shutdown(sockfd,SHUT_WR)<0)
	{
		LOG_SYSERR<<"Socket::shutdownWrite()";
	}
}

void Socket::fromIpPort(const char* ip,uint16_t port,struct sockaddr_in* addr){
	addr->sin_family=AF_INET;
	addr->sin_port=htons(port);
	if(::inet_pton(AF_INET,ip,&addr->sin_addr)<=0){
		LOG_SYSERR<<"Socket::fromIpPort()";
	}
}

void Socket::toIp(char* buf,size_t size,const struct sockaddr_in* addr){
	assert(size>=INET_ADDRSTRLEN);

	if(::inet_ntop(AF_INET,&addr->sin_addr,buf,static_cast<socklen_t>(size))==NULL){
		LOG_SYSERR<<"Socket::toIp()";
	}
}

void Socket::toIpPort(char* buf,size_t size,const struct sockaddr_in* addr){
	toIp(buf,size,addr);

	size_t end=strlen(buf);
	uint16_t port=ntohs(addr->sin_port);
	assert(size>end);
	snprintf(buf+end,size-end,":%u",port);

}

struct sockaddr_in Socket::getLocalAddr(int sockfd){
	sockaddr_in localaddr;
	memset(&localaddr,0,sizeof localaddr);
	socklen_t addrLen=static_cast<socklen_t>(sizeof(localaddr));
	if(::getsockname(sockfd,reinterpret_cast<struct sockaddr*>(&localaddr),&addrLen)<0){
		LOG_SYSERR<<"Socket::getLocalAddr()";
	}
	return localaddr;
}

struct sockaddr_in Socket::getPeerAddr(int sockfd){
	sockaddr_in peerAddr;
	memset(&peerAddr,0,sizeof peerAddr);
	socklen_t addrLen=static_cast<socklen_t>(sizeof(peerAddr));
	if(::getpeername(sockfd,reinterpret_cast<struct sockaddr*>(&peerAddr),&addrLen)<0){
		LOG_SYSERR<<"Socket::getPeerAddr()";
	}
	return peerAddr;
}

int Socket::getSocketError(int sockfd){
	int optval=0;
	socklen_t optlen=static_cast<socklen_t>(sizeof optval);
	if(::getsockopt(sockfd,SOL_SOCKET,SO_ERROR,&optval,&optlen)<0){
		LOG_SYSERR<<"Socket::getSocketError()";
		return errno;
	}
	else return optval;
}

int Socket::connect(int sockfd,const struct sockaddr_in* addr){
	return ::connect(sockfd,
				 	 reinterpret_cast<const struct sockaddr*>(addr),
			  	  	 static_cast<socklen_t>(sizeof(struct sockaddr_in)));
}

ssize_t Socket::read(int sockfd,void* buf,size_t count){
	return ::read(sockfd,buf,count);
}

ssize_t Socket::readv(int sockfd,const struct iovec* iov,int iovcnt){
	return ::readv(sockfd,iov,iovcnt);
}

ssize_t Socket::write(int sockfd,const void* buf,size_t count){
	return ::write(sockfd,buf,count);
}
