#ifndef FF_SOCKET_H_
#define FF_SOCKET_H_

#include "Socketopsff.h"

struct tcp_info;

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
		//peerAddr用来存储客户端的地址
		int accept(InetAddressff& peerAddr);
		
		void shutdown();

		void setTcpNoDelay(bool on);
		void setTcpReuseAddr(bool on);
		void setTcpReusePort(bool on);
		void setTcpKeepAlive(bool on);

		bool getTcpInfo(struct tcp_info*) const;
		bool getTcpInfoString(char*,int) const;

};//class Socketff

}//namespace firey

#endif//FF_SOCKET_H_
