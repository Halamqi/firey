#include "EventLoopff.h"
#include "Acceptorff.h"
#include "InetAddressff.h"

#include <string>
#include <assert.h>

using namespace firey;

void onConnection(int sockfd,const InetAddressff& addr){
	int client_info=addr.toPort();
	printf("client:%d connect on server",client_info);
	fflush(stdout);
	char buf[]="hello world!\n";
	ssize_t n=::write(sockfd,buf,sizeof buf);
	assert(n>=0);
	::close(sockfd);
}

void quit(EventLoopff* loop){
	loop->quit();
}

void* quitThread(void* arg){
	EventLoopff* loop=reinterpret_cast<EventLoopff*>(arg);
	loop->runAfter(20.0,std::bind(&quit,loop));
	return NULL;
}
int main(){
	EventLoopff loop;
	InetAddressff serverAddr(12345);
	Acceptorff acceptor(&loop,serverAddr,true);
	acceptor.setNewConnectionCallback(onConnection);
	acceptor.listen();
	pthread_t tid;
	pthread_create(&tid,NULL,quitThread,reinterpret_cast<void*>(&loop));
	pthread_detach(tid);
	loop.loop();
	return 0;
}
