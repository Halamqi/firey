#ifndef FF_POLLER_H_
#define FF_POLLER_H_

#include <sys/epoll.h>
#include <vector>
#include <map>

struct epoll_event;

namespace firey{

class EventLoopff;
class Channelff;

class Pollerff{

	public:
		Pollerff(EventLoopff* loop);
		~Pollerff();
		Pollerff(const Pollerff&) = delete;
		Pollerff& operator=(const Pollerff&) = delete;

		typedef std::vector<struct epoll_event> eventList;
		typedef std::map<int,Channelff*> ChannelMap;
		typedef std::vector<Channelff*> ChannelList;

		int epollfd() const{return epollFd_;}

		void removeChannel(Channelff* channel);
		void updateChannel(Channelff* channel);
		bool hasChannel(Channelff* channel);

		void poll(int timeoutMs,ChannelList* activeChannels);
	private:
		EventLoopff* ownerLoop_;
		static const int kinitEventListSize=16;

		void fillActiveChannels(int numEvents,ChannelList* activeChannels) const;
		void update(int op,Channelff* channel);	

		eventList eventsList_;
		ChannelMap channels_;



		int epollFd_;

};//class Pollerff
}//namespace firey
#endif //FF_POLLER_H_
