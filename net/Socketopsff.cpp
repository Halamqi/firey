
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

