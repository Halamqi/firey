#include "InetAddressff.h"

#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

using namespace firey;


InetAddressff::InetAddressff(uint16_t port){
	memset(&addr_,0,sizeof addr_);
	addr_.sin_family=AF_INET;
	addr_.sin_port=htons(port);
	addr_.sin_addr.s_addr=htonl(INADDR_ANY);
}

InetAddressff::InetAddressff(const std::string& ip,uint16_t port){
	memset(&addr_,0,sizeof addr_);
	Socket::fromIpPort(ip.data(),port,&addr_);
}

std::string InetAddressff::toIpPort() const{
	char buf[64]={0};
	Socket::toIpPort(buf,sizeof buf,&addr_);
	return buf;
}

std::string InetAddressff::toIp() const{
	char buf[64]={0};
	Socket::toIp(buf,sizeof buf,&addr_);
	return buf;
}

uint16_t InetAddressff::toPort() const{
	return ntohs(addr_.sin_port);
}

uint32_t InetAddressff::ipNetEndian() const{
	return addr_.sin_addr.s_addr;
}

