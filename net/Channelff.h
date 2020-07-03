#ifndef FF_CHANNEL_H_
#define FF_CHANNEL_H_

#include <functional>


namespace firey
{
class ffEventLoop;

class ffChannel
{
	public:
		ffChannel(ffEventLoop* loop,int fd);
		~ffChannel();
		ffChannel(const ffChannel&) =delete;
		ffChannel& operator=(const ffChannel&)=delete;

	public:
		typedef std::function<void()> eventCallBack;
		void setReadCallBack(eventCallBack rcb){
			readCallBack_=std::move(rcb);
		}
		void setWriteCallBack(eventCallBack wcb){
			writeCallBack_=std::move(wcb);
		}
		void setErrorCallBack(eventCallBack ecb){
			errorCallBack_=std::move(ecb);
		}

		int fd() const{return fd_;}
		int events() const{return events_;}
		int revents() const{ return revents_;}
		void setRevents(int revt){revents_=revt;}

		bool isNoneEvent(){return events_== kNoneEvent;}

		void disableAll(){events_=kNoneEvent;update();}

		int index(){return index_;}
		void setIndex(int idx){index_=idx;}

		ffEventLoop* ownerLoop(){return loop_;}

		void enableReading(){events_=kReadEvent;update();}
		void enableWriting(){events_=kWriteEvent;update();}

		bool isReading(){return events_==kReadEvent;}
		bool isWriting(){return events_==kWriteEvent;}

	public:
		void handleEvent();
		void update();
		void remove();
	
	private:
		ffEventLoop* loop_;
		
		const int fd_;
		int events_;
		int revents_;

		static const int kNoneEvent;
		static const int kReadEvent;
		static const int kWriteEvent;

		int index_;
		
		eventCallBack readCallBack_;
		eventCallBack writeCallBack_;
		eventCallBack errorCallBack_;

		bool eventHandling_;
		bool addToLoop_;
};//ffChannel
}//namespace firey

#endif //FF_CHANNEL_H_
