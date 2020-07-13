#include "Socketff.h"
#include "InetAddressff.h"

#include <sys/types.h>

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

/*void Socketff::setTcpNoDelay(bool on){
	int optval=on?1:0;
	::setsockopt(sockfd_,IPPROTO_TCP,TCP_NODELAY,&optval,static_cast<socklen_t>(sizeof optval));
}*/

void Socketff::setTcpReuseAddr(bool on){
	int optval=on?1:0;
	::setsockopt(sockfd_,SOL_SOCKET,SO_REUSEADDR,&optval,static_cast<socklen_t>(sizeof optval));
}

void Socketff::setTcpReusePort(bool on){
	#ifdef SO_REUSEPORT
	int optval = on ?1:0;
	::setsockopt(sockfd_,SOL_SOCKET,SO_REUSEPORT,&optval,static_cast<socklen_t>(sizeof optval));
	#endif
}

void Socketff::setTcpKeepAlive(bool on){
	int optval=on?1:0;
	::setsockopt(sockfd_,SOL_SOCKET,SO_KEEPALIVE,&optval,static_cast<socklen_t>(sizeof optval));
}

void Socketff::shutdown(){
	Socket::shutdownWrite(sockfd_);
}
