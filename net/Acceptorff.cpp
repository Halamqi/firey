#include "Acceptorff.h"
#include "EventLoopff.h"
#include "Channelff.h"
#include "InetAddressff.h"
#include "Socketff.h"

using namespace firey;

Acceptorff::Acceptorff(EventLoopff* loop)
	:ownerLoop_(loop),
	acceptSocket_(new Socketff(Socket::createNonblockingOrDie())),
	acceptChannel_(new Channelff(ownerLoop_,acceptSocket_->fd()))
{
	acceptChannel_->setReadCallBack(
			std::bind(&Acceptorff::handleRead,this));
	acceptChannel_->enableReading();
}

Acceptorff::~Acceptorff(){
	acceptChannel_->disableAll();
	acceptChannel_->remove();
}

