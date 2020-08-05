#include "Timestampff.h"

#include <sys/time.h>
#include <stdio.h>

using namespace firey;

static_assert(sizeof(Timestampff) == sizeof(int64_t),"TimeStamp is same as int64_t");

std::string Timestampff::toString() const{
	char buf[32]={0};
	int64_t seconds=usSinceEpoch_/kusPerSecond;
	int64_t microSeconds=usSinceEpoch_%kusPerSecond;

	snprintf(buf,sizeof buf,"%ld.%06ld" ,seconds,microSeconds);
	return buf;
}

std::string Timestampff::toFormatString(bool show_us) const{
	char buf[64]={0};
	time_t seconds=static_cast<time_t>(usSinceEpoch_/kusPerSecond);
	struct tm tm_time;

	gmtime_r(&seconds,&tm_time);
	if(show_us){
		int microSecond=static_cast<int>(usSinceEpoch_%kusPerSecond);
		snprintf(buf,sizeof buf,"%4d.%02d.%02d %02d:%02d:%02d.%06d",
				tm_time.tm_year+1900,tm_time.tm_mon+1,tm_time.tm_mday,
				(tm_time.tm_hour+8)%24,tm_time.tm_min,tm_time.tm_sec,microSecond);
	}
	else{
		snprintf(buf,sizeof buf,"%4d.%02d.%02d %02d:%02d:%02d",
				tm_time.tm_year+1900,tm_time.tm_mon+1,tm_time.tm_mday,
				(tm_time.tm_hour+8)%24,tm_time.tm_min,tm_time.tm_sec);
	}
	return buf;
}

Timestampff Timestampff::now(){
	struct timeval tv;
	gettimeofday(&tv,NULL);
	int64_t seconds=tv.tv_sec;
	return Timestampff(seconds*kusPerSecond+tv.tv_usec);
}

