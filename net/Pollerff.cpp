#include "Pollerff.h"
#include "EventLoopff.h"
#include "sys/epoll.h"
#include "Channelff.h"
#include "Loggingff.h"

#include <string.h>
#include <assert.h>

#include <stdio.h>

using namespace firey;

static const int kNew=-1;
static const int kAdded=1;
static const int kDeleted=2;

Pollerff::Pollerff(EventLoopff* loop)
	:ownerLoop_(loop),
	eventsList_(kinitEventListSize),
	epollFd_(::epoll_create(1))
{
	LOG_SYSFATAL<<"Pollerff::Pollerff()";
}

Pollerff::~Pollerff(){
	::close(epollFd_);
}

Timestampff Pollerff::poll(int timeoutMs,ChannelList* activeChannel){
	LOG_TRACE<<"fd total count "<<channels_.size();
	int numEvents=::epoll_wait(epollFd_,
			eventsList_.data(),
			static_cast<int>(eventsList_.size()),
			timeoutMs);
	Timestampff returnTime(Timestampff::now());
	int saveErrno=errno;
	if(numEvents>0){
		LOG_TRACE<<numEvents<<" events happened";
		fillActiveChannels(numEvents,activeChannel);
		if(numEvents==static_cast<int>(eventsList_.size())){
			eventsList_.resize(2*eventsList_.size());
		}
	}
	else if(numEvents<0){
		if(saveErrno==EINTR)
		{
			errno=saveErrno;
			LOG_SYSERR<<"Pollerff::poll()";
		}
	}
	else {
		LOG_TRACE<<"nothing happened";
	}
	return returnTime;
}

void Pollerff::fillActiveChannels(int numEvents,ChannelList* activeChannels) const {
	for(int i=0;i<numEvents;i++){
		Channelff* channel=static_cast<Channelff*>(eventsList_[i].data.ptr);
		int fd=channel->fd();
		ChannelMap::const_iterator it=channels_.find(fd);
		assert(it!=channels_.end());
		assert(it->second==channel);
		channel->setRevents(eventsList_[i].events);
		activeChannels->push_back(channel);
	}
}

void Pollerff::updateChannel(Channelff* channel){
	ownerLoop_->assertInLoopThread();
	const int idx=channel->index();
	LOG_TRACE<<"fd = "<<channel->fd()
		<<" events = "<<channel->events()
		<<" index = "<<idx;
	if(idx==kNew||idx==kDeleted){
		int fd=channel->fd();
		if(idx==kNew){
			assert(channels_.find(fd)==channels_.end());
			channels_[fd]=channel;
		}
		else//idx==kDelete
		{
			assert(channels_.find(fd)!=channels_.end());
			assert(channels_[fd]==channel);
		}

		channel->setIndex(kAdded);
		update(EPOLL_CTL_ADD,channel);
	}

	else//idx==kAdded
	{
		int fd=channel->fd();
		assert(channels_.find(fd)!=channels_.end());
		assert(channels_[fd]==channel);
		assert(idx==kAdded);

		if(channel->isNoneEvent()){
			update(EPOLL_CTL_DEL,channel);
			channel->setIndex(kDeleted);
		}
		else{
			update(EPOLL_CTL_MOD,channel);
		}
	}
}

void Pollerff::removeChannel(Channelff* channel){
	ownerLoop_->assertInLoopThread();
	int fd=channel->fd();
	LOG_TRACE<<"fd = "<<fd;

	assert(channels_.find(fd)!=channels_.end());
	assert(channels_[fd]==channel);
	assert(channel->isNoneEvent());

	int idx=channel->index();

	//从channels_这个map中将channel移除掉
	assert(idx==kAdded||idx==kDeleted);
	size_t n=channels_.erase(fd);
	(void)n;
	assert(n==1);

	if(idx==kAdded){
		update(EPOLL_CTL_DEL,channel);	
	}
	channel->setIndex(kNew);
}

void Pollerff::update(int op,Channelff* channel){
	struct epoll_event temp;
	memset(&temp,0,sizeof temp);

	temp.events=channel->events();
	temp.data.ptr=static_cast<void*>(channel);

	int fd=channel->fd();
	LOG_TRACE<<"epoll_ctl op = "<<operationToString(op)
		<<"fd = "<<fd
		<<"event = { "<<channel->eventToString()<<" }";
	if(::epoll_ctl(epollFd_,op,fd,&temp)<0){
		if(op==EPOLL_CTL_DEL)
		{
			LOG_SYSERR<<"epoll_ctl op = "<<operationToString(op)
				<<" fd = "<<fd;
		}
		else
		{
			LOG_SYSFATAL<<"epoll_ctl op = "<<operationToString(op)
				<<"fd = "<<fd;
		}
	}

}

//查看channel是否在poller的channalMap中
bool Pollerff::hasChannel(Channelff* channel){
	int fd=channel->fd();
	return channels_.find(fd)!=channels_.end();
}

const char* Pollerff::operationToString(int op)
{
	switch (op)
	{
		case EPOLL_CTL_ADD:
			return "ADD";
		case EPOLL_CTL_DEL:
			return "DEL";
		case EPOLL_CTL_MOD:
			return "MOD";
		default:
			assert(false&& "ERROR op");
			return "Unknown Operation";
	}
}
