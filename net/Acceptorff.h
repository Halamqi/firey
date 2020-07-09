#ifndef FF_ACCEPTOR_H_
#define FF_ACCEPTOR_H_

#include <memory>

namespace firey{

class EventLoopff;
class Channelff;
class Socketff;

class Acceptorff{
	private:
		EventLoopff* ownerLoop_;

		std::unique_ptr<Socketff> acceptSocket_;
		std::unique_ptr<Channelff> acceptChannel_;

	private:
		void handleRead();

	public:
		Acceptorff(EventLoopff* loop);
		~Acceptorff();

		Acceptorff(const Acceptorff&)=delete;
		Acceptorff& operator=(const Acceptorff&)=delete;
		
		void listen();

};

}//namespace firey

#endif //FF_ACCEPTOR_H_
