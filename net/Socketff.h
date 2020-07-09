#ifndef FF_SOCKET_H_
#define FF_SOCKET_H_

#include "Socketopsff.h"

namespace firey{

class InetAddressff;

class Socketff{
	private:
		const int sockfd_;

	public:
		Socketff(int fd)
			:sockfd_(fd){}

		~Socketff();
		
		Socketff(const Socketff&)=delete;
		Socketff& operator=(const Socketff&)=delete;

		int fd() const{return sockfd_;}
		
		void bindAddress(const InetAddressff& localAddr);
		void listen();
		int accept(InetAddressff& peerAddr);
		
		void shutdown();

		void setTcpNoDelay(bool on);
		void setTcpReuseAddr(bool on);
		void setTcpReusePort(bool on);
		void setTcpKeepAlive(bool on);

};//class Socketff

}//namespace firey

#endif//FF_SOCKET_H_
