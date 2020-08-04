#include "LogFileff.h"
#include "FileUtilff.h"

#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <time.h>

namespace firey{
	namespace ProcessInfo{
		std::string hostname()
		{
			char buf[256];
			if(::gethostname(buf,sizeof buf)==0)
			{
				buf[sizeof(buf)-1]='\0';
				return buf;
			}
			else return "unkownhost";
		}

		std::string pidToString(){
			char buf[32];
			snprintf(buf,sizeof buf,".%d",::getpid());
			return buf;
		}
	}//namespace ProcessInfo
}//namespace firey
using namespace firey;

LogFileff::LogFileff(const std::string& basename,
		off_t rollSize,
		bool threadSafe,
		int flushInterval,
		int checkEveryN)
	:basename_(basename),
	rollSize_(rollSize),
	flushInterval_(flushInterval),
	checkEveryN_(checkEveryN),
	count_(0),
	mutex_(threadSafe?new Mutexff:nullptr),
	startOfPeriod_(0),
	lastRoll_(0),
	lastFlush_(0)
{
	assert(basename.find('/')==std::string::npos);//确保basename中没有'/'
	rollFile();
}

LogFileff::~LogFileff()=default;

void LogFileff::append(const char* logline,int len)
{
	if(mutex_)
	{
		MutexGuardff lock(*mutex_);
		append_unlocked(logline,len);
	}
	else
	{
		append_unlocked(logline,len);
	}
}

void LogFileff::flush()
{
	if(mutex_)
	{
		MutexGuardff lock(*mutex_);
		file_->flush();
	}
	else
	{
		file_->flush();
	}
}

void LogFileff::append_unlocked(const char* logline,int len)
{
	file_->append(logline,len);

	//控制日志文件的大小，超出了rollSize_，则换个新的日志文件写
	if(file_->writtenBytes()>rollSize_)
	{
		rollFile();
	}
	else
	{
		++count_;//每调用一次append_unlocked，count就++
		if(count_>=checkEveryN_)
		{
			count_=0;
			//获取当前时间
			time_t now=::time(NULL);
			//计算当前时间在哪一天
			time_t thisPeriod_=now/kRollPerSecond_*kRollPerSecond_;
			if(thisPeriod_!=startOfPeriod_)
			{
				rollFile();
			}
			else if(now - lastFlush_>flushInterval_)
			{
				lastFlush_=now;
				file_->flush();
			}
		}
	}
}

bool LogFileff::rollFile()
{
	time_t now=0;
	std::string filename=getLogFileName(basename_,&now);
	time_t start=now/kRollPerSecond_*kRollPerSecond_;

	if(now>lastRoll_)
	{
		lastRoll_=now;
		lastFlush_=now;
		startOfPeriod_=start;
		file_.reset(new AppendFileff(filename));//重新创建出一个日志文件
		return true;
	}
	return false;
}

/*获取新的日志文件名称*/
std::string LogFileff::getLogFileName(const std::string& basename,time_t* now)
{
	std::string filename;
	filename.reserve(basename.size()+64);
	filename=basename;

	char timebuf[32];
	struct tm tm_time;
	*now=::time(NULL);
	gmtime_r(now,&tm_time);
	snprintf(timebuf,sizeof timebuf,".%4d%02d%02d-%02d%02d%02d.",
			tm_time.tm_year+1900,tm_time.tm_mon+1,tm_time.tm_mday,
			(tm_time.tm_hour+8)%24,tm_time.tm_min,tm_time.tm_sec);
	filename+=timebuf;

	filename+=ProcessInfo::hostname();
	filename+=ProcessInfo::pidToString();

	filename+=".log";

	return filename;
}
