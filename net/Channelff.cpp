#include "EventLoopff.h"
#include "Channelff.h"

#include <sys/epoll.h>
#include <assert.h>

#include <sstream>
#include <string>

using namespace firey;

const int Channelff::kNoneEvent=0;
const int Channelff::kReadEvent=EPOLLIN;
const int Channelff::kWriteEvent=EPOLLOUT;

Channelff::Channelff(EventLoopff* loop,int fd)
	:loop_(loop),
	fd_(fd),
	events_(0),
	revents_(0),
	index_(-1),
	eventHandling_(false),
	addToLoop_(false),
	tied_(false)
{
}

Channelff::~Channelff(){
	assert(!eventHandling_);
	assert(!addToLoop_);

	if(loop_->isInLoopThread()){
		
	}
}

void Channelff::update(){
	loop_->assertInLoopThread();
	addToLoop_=true;
	loop_->updateChannel(this);
}

void Channelff::handleEvent(Timestampff receiveTime){
	std::shared_ptr<void> guard;
	if(tied_){
		guard=tie_.lock();
		if(guard){
			handleEventWithGuard(receiveTime);
		}
	}
	else{
		handleEventWithGuard(receiveTime);
	}
}

void Channelff::handleEventWithGuard(Timestampff receiveTime){
	loop_->assertInLoopThread();
	//if(revents_ & EPOLLNVAL){
		
	//}
	if(revents_ & EPOLLERR){
		if(errorCallBack_) errorCallBack_();
	}
	if(revents_ & (EPOLLIN|EPOLLPRI|EPOLLRDHUP)){
		if(readCallBack_) readCallBack_(receiveTime);
	}
	if(revents_ & EPOLLOUT){
		if(writeCallBack_) writeCallBack_();	
	}
}

void Channelff::remove(){
	assert(isNoneEvent());
	addToLoop_=false;
	loop_->removeChannel(this);
}

void Channelff::tie(const std::shared_ptr<void>& obj){
	tie_=obj;
	tied_=true;
}

//学习这种设计模式
std::string Channelff::eventToString()
{
	return eventToString(fd_,events_);
}

std::string Channelff::eventToString(int fd,int event)
{
	std::ostringstream oss;
	oss<<fd<<":";
	if(event&EPOLLIN)
	{
		oss<<"IN ";
	}
	if(event&EPOLLPRI)
	{
		oss<<"PRI ";
	}
	if(event&EPOLLOUT)
	{
		oss<<"OUT";
	}
	if(event&EPOLLHUP)
	{
		oss<<"HUP ";
	}
	if(event&EPOLLRDHUP)
	{
		oss<<"RDHUP ";
	}
	if(event&EPOLLERR)
	{
		oss<<"ERR ";
	}
#ifdef EPOLLNVAL
	if(event&EPOLLNVAL)
	{
		oss<<"NVAL";
	}
#endif
	return oss.str();
}
