#include "Socketff.h"
#include "InetAddressff.h"

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

void Socketff::setTcpNoDelay(bool on){}

void Socketff::setTcpReuseAddr(bool on){}

void Socketff::setTcpReusePort(bool on){}

void Socketff::setTcpKeepAlive(bool on){}

void Socketff::shutdown(){
	Socket::shutdownWrite(sockfd_);
}
