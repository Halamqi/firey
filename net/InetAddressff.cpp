#include "InetAddressff.h"

#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

using namespace firey;

namespace detail{
	void fromIpPort(const char* ip,uint16_t port,struct sockaddr_in* addr){
		addr->sin_family=AF_INET;
		addr->sin_port=htons(port);
		if(::inet_pton(AF_INET,ip,&addr->sin_addr)<=0){
			fprintf(stderr,"fromIpPort()");
		}
	}

	void toIp(char* buf,int size,const struct sockaddr_in* addr){
		assert(size>=INET_ADDRSTRLEN);

		if(::inet_ntop(AF_INET,&addr->sin_addr,buf,static_cast<socklen_t>(size))<0){
			fprintf(stderr,"toIp()");
		}
	}

	void toIpPort(char* buf,int size,const struct sockaddr_in* addr){
		toIp(buf,size,addr);

		size_t end=strlen(buf);
		uint16_t port=ntohs(addr->sin_port);
		assert(size-end>3);
		snprintf(buf+end,size-end,":%u",port);
	}

}

InetAddressff::InetAddressff(uint16_t port){
	memset(&addr_,0,sizeof addr_);
	addr_.sin_family=AF_INET;
	addr_.sin_port=htons(port);
	addr_.sin_addr.s_addr=htonl(INADDR_ANY);
}

InetAddressff::InetAddressff(const std::string& ip,uint16_t port){
	memset(&addr_,0,sizeof addr_);
	detail::fromIpPort(ip.data(),port,&addr_);
}

std::string InetAddressff::toIpPort() const{
	char buf[64]={0};
	detail::toIpPort(buf,sizeof buf,&addr_);
	return buf;
}

std::string InetAddressff::toIp() const{
	char buf[64]={0};
	detail::toIp(buf,sizeof buf,&addr_);
	return buf;
}

uint16_t InetAddressff::toPort() const{
	return ntohs(addr_.sin_port);
}

uint32_t InetAddressff::ipNetEndian() const{
	return addr_.sin_addr.s_addr;
}

