#include "Socketff.h"
#include "InetAddressff.h"

#include <sys/socket.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

namespace Socket{
	int createNonblockingOrDie(){
		int sockfd=::socket(AF_INET,SOCK_STREAM|SOCK_NONBLOCK|SOCK_CLOEXEC,IPPROTO_TCP);
		if(sockfd<0){
			fprintf(stderr,"createNonblokingOrDid()%s",strerror(sockfd));
			abort();
		}
		return sockfd;
	}

	void bindOrDie(int sockfd,const struct sockaddr_in* addr){
		int ret=::bind(sockfd,(struct sockaddr*)addr,
				static_cast<socklen_t>(sizeof(struct sockaddr_in)));
		if(ret<0){
			fprintf(stderr,"bindOrDie(),%s",strerror(ret));
			abort();
		}
	}

	void listenOrDie(int sockfd){
		int ret=::listen(sockfd,128);
		if(ret<0){
			fprintf(stderr,"listenOrDie(),%s",strerror(ret));
			abort();
		}
	}

	int accept(int sockfd,struct sockaddr_in* addr){
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

}

using namespace firey;

Socketff::~Socketff(){
	::close(sockfd_);
}

void Socketff::bindAddress(const InetAddressff& localAddr){
	Socket::bindOrDie(sockfd_,localAddr.getSockAddr());
}

void Socketff::listen(){
	Socket::listenOrDie(sockfd_);
}

int Socketff::accept(InetAddressff& peerAddr){
	struct sockaddr_in addr;
	memset(&addr,0,sizeof addr);
	int connfd=Socket::accept(sockfd_,&addr);
	if(connfd>=0){
		peerAddr.setSockAddr(addr);
	}
	return connfd;
}
