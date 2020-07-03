#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

class MySocket{
	public:
		MySocket();
		~MySocket();
		void bind(const struct sockaddr_in* addr);
		void listen(int listenNum);
		int accept(struct sockaddr_in* clientAddr);
		int getSockFd() const { return sockfd_;}
	private:
		int sockfd_;
};
