
#include "Socketopsff.h"

int Socket::createNonblockingOrDie(){
	int sockfd=::socket(AF_INET,SOCK_STREAM|SOCK_NONBLOCK|SOCK_CLOEXEC,IPPROTO_TCP);
	if(sockfd<0){
		fprintf(stderr,"createNonblokingOrDid()%s",strerror(sockfd));
		abort();
	}
	return sockfd;
}

void Socket::bindOrDie(int sockfd,const struct sockaddr_in* addr){
	int ret=::bind(sockfd,(struct sockaddr*)addr,
			static_cast<socklen_t>(sizeof(struct sockaddr_in)));
	if(ret<0){
		fprintf(stderr,"bindOrDie(),%s",strerror(ret));
		abort();
	}
}

void Socket::listenOrDie(int sockfd){
	int ret=::listen(sockfd,128);
	if(ret<0){
		fprintf(stderr,"listenOrDie(),%s",strerror(ret));
		abort();
	}
}

int Socket::accept(int sockfd,struct sockaddr_in* addr){
	socklen_t addrlen=static_cast<socklen_t>(sizeof(*addr));
	int connfd=::accept4(sockfd,(struct sockaddr*)addr,
			&addrlen,SOCK_NONBLOCK|SOCK_CLOEXEC); 
	if(connfd<0){
		int savedErrno=errno;
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
				// unexpected errors
				fprintf(stderr,"listenOrDie(),%s",strerror(connfd));
				abort();
				break;
			default:
				fprintf(stderr,"listenOrDie(),%s",strerror(connfd));
				abort();
				break;
		}
	}
	return connfd;
}

void Socket::closeOrDie(int sockfd){
	int ret=::close(sockfd);
	if(ret<0){
		fprintf(stderr,"listenOrDie(),%s",strerror(ret));
		abort();
	}
}
void Socket::shutdownWrite(int sockfd){
	::shutdown(sockfd,SHUT_WR);
}

void Socket::fromIpPort(const char* ip,uint16_t port,struct sockaddr_in* addr){
	addr->sin_family=AF_INET;
	addr->sin_port=htons(port);
	if(::inet_pton(AF_INET,ip,&addr->sin_addr)<=0){
		fprintf(stderr,"fromIpPort()");
	}
}

void Socket::toIp(char* buf,int size,const struct sockaddr_in* addr){
	assert(size>=INET_ADDRSTRLEN);

	if(::inet_ntop(AF_INET,&addr->sin_addr,buf,static_cast<socklen_t>(size))<0){
		fprintf(stderr,"toIp()");
	}
}

void Socket::toIpPort(char* buf,int size,const struct sockaddr_in* addr){
	toIp(buf,size,addr);

	size_t end=strlen(buf);
	uint16_t port=ntohs(addr->sin_port);
	assert(size-end>3);
	snprintf(buf+end,size-end,":%u",port);

}

struct sockaddr_in Socket::getLocalAddr(int sockfd){
	sockaddr_in localaddr;
	memset(&localaddr,0,sizeof localaddr);
	socklen_t addrLen=static_cast<socklen_t>(sizeof(localaddr));
	if(::getsockname(sockfd,(struct sockaddr*)&localaddr,&addrLen)<0){
		fprintf(stderr,"Socket::getLocalAddr()");
	}
	return localaddr;
}

struct sockaddr_in Socket::getPeerAddr(int sockfd){
	sockaddr_in peerAddr;
	memset(&peerAddr,0,sizeof peerAddr);
	socklen_t addrLen=static_cast<socklen_t>(sizeof(peerAddr));
	if(::getpeername(sockfd,(struct sockaddr*)&peerAddr,&addrLen)<0){
		fprintf(stderr,"Socket::getLocalAddr()");
	}
	return peerAddr;
}

int Socket::getSocketError(int sockfd){
	int optval=0;
	socklen_t optlen=static_cast<socklen_t>(sizeof optval);
	if(::getsockopt(sockfd,SOL_SOCKET,SO_ERROR,&optval,&optlen)<0){
		return errno;
	}
	else return optval;
}

int Socket::connect(int sockfd,const struct sockaddr_in* addr){
	return ::connect(sockfd,(struct sockaddr*)addr,static_cast<socklen_t>(sizeof(struct sockaddr_in)));
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
