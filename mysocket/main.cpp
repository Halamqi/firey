#include <iostream>
#include "mySock.h"

int main(){
	MySocket server;
	struct sockaddr_in serverAddr;
	bzero(&serverAddr,sizeof serverAddr);
	serverAddr.sin_family=AF_INET;
	serverAddr.sin_addr.s_addr=htonl(INADDR_ANY);
	serverAddr.sin_port=htons(8000);

	server.bind(&serverAddr);
	server.listen(10);
	struct sockaddr_in clientAddr;
	int connfd=server.accept(&clientAddr);
	char sendMsg[]="hello";
	send(connfd,sendMsg,sizeof(sendMsg),0);
	return 0;
}
