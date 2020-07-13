#ifndef FF_ACCEPTOR_H_
#define FF_ACCEPTOR_H_

#include "Socketff.h"
#include "Channelff.h"

#include <memory>

namespace firey{

class EventLoopff;
class Channelff;
class Socketff;
class InetAddressff;

class Acceptorff{
	public:
		
		typedef std::function<void(int,const InetAddressff&)> NewConnectionCallback;
	private:
		EventLoopff* ownerLoop_;

		Socketff acceptSocket_;
		Channelff acceptChannel_;

	private:
		void handleRead();

		bool listening_;

		NewConnectionCallback newConnectionCallback_;

		int idleFd_;
	public:
		Acceptorff(EventLoopff* loop,const InetAddressff& listenAddr,bool reusePort);
		~Acceptorff();

		Acceptorff(const Acceptorff&)=delete;
		Acceptorff& operator=(const Acceptorff&)=delete;
		
		void listen();
		bool listening() const{return listening_;}


		void setNewConnectionCallback(NewConnectionCallback cb){
			newConnectionCallback_=std::move(cb);
		}
};

}//namespace firey

#endif //FF_ACCEPTOR_H_
