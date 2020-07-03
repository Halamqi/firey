#ifndef FF_POLLER_H_
#define FF_POLLER_H_

#include <sys/epoll.h>
#include <vector>
#include <map>

struct epoll_event;

namespace firey{

class ffEventLoop;
class ffChannel;

class ffPoller{

	public:
		ffPoller(ffEventLoop* loop);
		~ffPoller();
		ffPoller(const ffPoller&) = delete;
		ffPoller& operator=(const ffPoller&) = delete;

		typedef std::vector<struct epoll_event> eventList;
		typedef std::map<int,ffChannel*> ChannelMap;
		typedef std::vector<ffChannel*> ChannelList;

		int epollfd() const{return epollFd_;}

		void removeChannel(ffChannel* channel);
		void updateChannel(ffChannel* channel);
		bool hasChannel(ffChannel* channel);

		void poll(int timeoutMs,ChannelList* activeChannels);
	private:
		ffEventLoop* ownerLoop_;
		static const int kinitEventListSize=16;

		void fillActiveChannels(int numEvents,ChannelList* activeChannels) const;
		void update(int op,ffChannel* channel);	

		eventList eventsList_;
		ChannelMap channels_;



		int epollFd_;

};//class ffPoller
}//namespace firey
#endif //FF_POLLER_H_
