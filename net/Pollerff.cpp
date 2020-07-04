#include "Pollerff.h"
#include "EventLoopff.h"
#include "sys/epoll.h"
#include "Channelff.h"

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
	assert(epollFd_>0);
}

Pollerff::~Pollerff(){
	::close(epollFd_);
}

void Pollerff::poll(int timeoutMs,ChannelList* activeChannel){
	int numEvents=::epoll_wait(epollFd_,
			eventsList_.data(),
			static_cast<int>(eventsList_.size()),
			timeoutMs);

	int saveErrno=errno;
	if(numEvents>0){
		fillActiveChannels(numEvents,activeChannel);
		if(numEvents==static_cast<int>(eventsList_.size())){
			eventsList_.resize(2*eventsList_.size());
		}
	}
	else if(numEvents<0){
		errno=saveErrno;
		perror("Pollerff::poll()");
	}
	else {
		printf("nothing happened\n");		
	}
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
	assert(channels_.find(fd)!=channels_.end());
	assert(channels_[fd]==channel);
	assert(channel->isNoneEvent());
	int idx=channel->index();

	//从channels_这个map中将channel移除掉
	assert(idx==kAdded||idx==kDeleted);
	channels_.erase(fd);
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

	if(::epoll_ctl(epollFd_,op,fd,&temp)<0){
		if(op==EPOLL_CTL_DEL)
			perror("Pollerff::update()");
		else abort();
	}

}

//查看channel是否在poller的channalMap中
bool Pollerff::hasChannel(Channelff* channel){
	int fd=channel->fd();
	return channels_.find(fd)!=channels_.end();
}
