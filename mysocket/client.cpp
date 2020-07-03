#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "mySock.h"
#include <iostream>

int main(int argc,char* argv[]){
	struct sockaddr_in server;
	bzero(&server,sizeof server);
	server.sin_family=AF_INET;
	inet_pton(AF_INET,argv[1],&server.sin_addr);
	server.sin_port=htons(atoi(argv[2]));
	int connfd=::socket(AF_INET,SOCK_STREAM,0);	
	int ret=::connect(connfd,(struct sockaddr*)&server,sizeof server);
	if(ret<0) return -1;
	char buffer[1024]; 
	recv(connfd,buffer,1024,0);
	std::cout<<buffer<<std::endl;
	while(1);
	return 0;
}
