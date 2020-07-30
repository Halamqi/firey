#include "Loggingff.h"
#include "CurretThreadff.h"

#include <errno.h>
#include <stdio.h>
#include <string.h>


namespace firey{
	
	//线程局部存储用来存储线程的errno
	__thread char t_errnobuf[512];
	__thread char t_time[64];
	__thread time_t t_lastSecond;

	const char* strerror_tl(int savedErrno){
		return strerror_r(savedErrno,t_errnobuf,sizeof t_errnobuf);
	}

	//初始化日志等级
	Logger::LogLevel initLogLevel()
	{
		//去环境变量列表中寻找环境变量
		if(::getenv("FIREY_LOG_TRACE"))
			return Loggerff::TRACE;
		if(::getenv("FIREY_LOG_DEBUG"))
			return Loggerff::DEBUG;
		else
			return Loggerff::INFO;
	}

	//设置日志等级
	Loggerff::LogLevel g_logLevel=initLogLevel();

	const char* LogLevelName[Loggerff::NUM_LOG_LEVELS]=
	{
		"TRACE",
		"DEBUG",
		"INFO",
		"WARN",
		"ERROR",
		"FATAL"
	};//LogLevel name

	//在编译期获取字符串长度
	class Helper{
		public:
		Helper(const char* str,unsigned len)
			:str_(str),
			 len_(len)
		{
			assert(strlen(str)==len);
		}

		const char* str_;
		const unsigned len_;
	};//class Helper

	inline LogStreamff& operator<<(LogStreamff& s,Helper v)
	{
		s.append(v.str(),v.len());
		return s;
	}

	inline LogStreamff& operator<<(LogStreamff& s,const SourceFile& file)
	{
		s.append(file.data_,file.size_);
		return s;
	}

	void defaultOutput(const char* msg,int len)
	{
		size_t n=fwrite(msg,1,len,stdout);
		void(n);
	}

	void defaultFlush()
	{
		fflush(stdout);
	}

	Loggerff::OutputFunc g_output=defaultOutput;
	Loggerff::FlushFunc g_flush=defaultFlush;
}//namespace firey

using namespace firey;


