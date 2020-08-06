#ifndef FF_CHANNEL_H_
#define FF_CHANNEL_H_

#include <functional>
#include <memory>

namespace firey
{

class EventLoopff;
class Timestampff;

class Channelff
{
	public:
		Channelff(EventLoopff* loop,int fd);
		~Channelff();
		Channelff(const Channelff&) =delete;
		Channelff& operator=(const Channelff&)=delete;

	public:
		typedef std::function<void(Timestampff)> ReadEventCallback;
		typedef std::function<void()> eventCallBack;
		void setReadCallback(ReadEventCallback rcb){
			readCallBack_=std::move(rcb);
		}
		void setWriteCallback(eventCallBack wcb){
			writeCallBack_=std::move(wcb);
		}
		void setErrorCallback(eventCallBack ecb){
			errorCallBack_=std::move(ecb);
		}

		void setCloseCallback(eventCallBack cb){
			closeCallback_=std::move(cb);
		}
		int fd() const{return fd_;}
		int events() const{return events_;}
		int revents() const{ return revents_;}
		void setRevents(int revt){revents_=revt;}

		bool isNoneEvent(){return events_== kNoneEvent;}

		void disableAll(){events_=kNoneEvent;update();}

		int index(){return index_;}
		void setIndex(int idx){index_=idx;}

		EventLoopff* ownerLoop(){return loop_;}

		void enableReading(){events_|=kReadEvent;update();}
		void enableWriting(){events_|=kWriteEvent;update();}

		void disableReading(){events_&=~kReadEvent;update();}
		void disableWriting(){events_&=~kWriteEvent;update();}

		bool isReading(){return events_==kReadEvent;}
		bool isWriting(){return events_==kWriteEvent;}

		void tie(const std::shared_ptr<void>& obj);

		std::string eventToString();
		
		std::string eventToString(int fd,int event);
	public:
		void handleEvent(Timestampff receiveTime);
		void update();
		void remove();
	
	private:
		EventLoopff* loop_;
		
		const int fd_;
		int events_;
		int revents_;

		static const int kNoneEvent;
		static const int kReadEvent;
		static const int kWriteEvent;

		int index_;
		
		ReadEventCallback readCallBack_;
		eventCallBack writeCallBack_;
		eventCallBack errorCallBack_;
		eventCallBack closeCallback_;

		void handleEventWithGuard(Timestampff receiveTime);
		bool eventHandling_;
		bool addToLoop_;

		std::weak_ptr<void> tie_;
		bool tied_;
};//Channelff
}//namespace firey

#endif //FF_CHANNEL_H_
