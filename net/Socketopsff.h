#ifndef FF_SOCKETOPS_H_
#define FF_SOCKETOPS_H_

#include <sys/socket.h>

#include <arpa/inet.h>
#include <unistd.h>

namespace Socket{
	int createNonblockingOrDie();

	void bindOrDie(int sockfd,const struct sockaddr_in* addr);

	void listenOrDie(int sockfd);

	int accept(int sockfd,struct sockaddr_in* addr);

	void closeOrDie(int sockfd);

	void shutdownWrite(int sockfd);

	void fromIpPort(const char* ip,uint16_t port,struct sockaddr_in* addr);

	void toIp(char* buf,size_t size,const struct sockaddr_in* addr);
	
	void toIpPort(char* buf,size_t size,const struct sockaddr_in* addr);

	struct sockaddr_in getLocalAddr(int sockfd);

	struct sockaddr_in getPeerAddr(int sockfd);

	int getSocketError(int sockfd);

	int connect(int sockfd,const struct sockaddr_in* addr);
	ssize_t read(int sockfd,void* buf,size_t count);
	ssize_t readv(int sockfd,const struct iovec* iov,int iovcnt);
	ssize_t write(int sockfd,const void* buf,size_t count);
	
	
}//namespace Socket

#endif//FF_SOCKETOPS_H_
