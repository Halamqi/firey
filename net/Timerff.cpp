#include "Timerff.h"

using namespace firey;

std::atomic<uint64_t> Timerff::timerCreated_(0);

Timerff::Timerff(timerCallback cb,Timestampff when,double interval)
	:callback_(std::move(cb)),
	interval_(interval),
	isRepeat_(interval_>0.0),
	expireTime_(when),
	sequence_(++timerCreated_)
{
	
}

Timerff::~Timerff(){}

void Timerff::restart(Timestampff now){
	if(!isRepeat_) expireTime_=Timestampff();
	else expireTime_=addTime(now,interval_);	
}
