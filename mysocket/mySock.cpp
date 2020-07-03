#include "mySock.h"
#include "myerror.h"

MySocket::MySocket(){
	sockfd_=::socket(AF_INET,
					SOCK_STREAM|
					SOCK_CLOEXEC,
					IPPROTO_TCP);
	if(sockfd_<0) myerror("socket failed"); 
}

MySocket::~MySocket(){
	::close(sockfd_);
}

void MySocket::bind(const struct sockaddr_in* addr){
	if(addr==nullptr) myerror("bind failed addr is empty");
	int ret=::bind(sockfd_,(struct sockaddr*)addr,sizeof(*addr));
	if(ret==-1)
	 	myerror("system call bind failed");
}

void MySocket::listen(int listenNum){
	if(::listen(sockfd_,listenNum)!=0)
		myerror("system call listen failed");
}

int MySocket::accept(struct sockaddr_in* clientAddr){
	socklen_t clientLen=sizeof(*clientAddr);
	bzero(clientAddr,clientLen);
	int connfd=::accept(sockfd_,(struct sockaddr*)clientAddr,&clientLen);
	if(connfd<0)
		myerror("system call accept failed");
	return connfd;
}

