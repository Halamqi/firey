#ifndef FF_LOGFILE_H_
#define FF_LOGFILE_H_

#include "MutexGuardff.h"

#include <string>
#include <memory>

namespace firey{

class AppendFileff;

class LogFileff{

	public:
		LogFileff(const std::string& basename,
				  off_t rollSize,
				  bool threadSafe=true,
				  int flushInterval=3,
				  int checkEveryN=1024);
		~LogFileff();

		LogFileff(const LogFileff&)=delete;
		LogFileff& operator=(const LogFileff&)=delete;

		void append(const char* logline,int len);
		void flush();
		//重新更换一个日志文件
		bool rollFile();

	private:
		void append_unlocked(const char* logline,int len);

		static std::string getLogFileName(const std::string& basename,time_t* now);

		const std::string basename_;
		const off_t rollSize_;
		const int flushInterval_;
		const int checkEveryN_;

		int count_;

		std::unique_ptr<Mutexff> mutex_;
		time_t startOfPeriod_;//日志系统开始的天数
		time_t lastRoll_;
		time_t lastFlush_;
		
		std::unique_ptr<AppendFileff> file_;

		const static int kRollPerSecond_=60*60*24;//一天的秒数
};//class LogFileff

}//namespace firey

#endif //FF_LOGFILE_H_
