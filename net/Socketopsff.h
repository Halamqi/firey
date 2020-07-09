#ifndef FF_SOCKETOPS_H_
#define FF_SOCKETOPS_H_

#include <sys/socket.h>

#include <stdio.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <string>
#include <assert.h>

namespace Socket{
	int createNonblockingOrDie();

	void bindOrDie(int sockfd,const struct sockaddr_in* addr);

	void listenOrDie(int sockfd);

	int accept(int sockfd,struct sockaddr_in* addr);

	void shutdownWrite(int sockfd);

	void fromIpPort(const char* ip,uint16_t port,struct sockaddr_in* addr);

	void toIp(char* buf,int size,const struct sockaddr_in* addr);
	
	void toIpPort(char* buf,int size,const struct sockaddr_in* addr);
	
}//namespace Socket

#endif//FF_SOCKETOPS_H_
