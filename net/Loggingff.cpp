#include "Loggingff.h"
#include "CurrentThreadff.h"

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

namespace firey{
	
	//线程局部存储用来存储线程的errno
	__thread char t_errnobuf[512];
	__thread char t_time[64];
	__thread time_t t_lastSecond;

	const char* strerror_tl(int savedErrno){
		return strerror_r(savedErrno,t_errnobuf,sizeof t_errnobuf);
	}

	//初始化日志等级
	Loggerff::LogLevel initLogLevel()
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
		"TRACE ",
		"DEBUG ",
		"INFO  ",
		"WARN  ",
		"ERROR ",
		"FATAL "
	};//LogLevel name

	//在编译期获取字符串长度
	class Helper{
		public:
		Helper(const char* str,unsigned len)
			:str_(str),
			 len_(len)
		{
			assert(strlen(str)==len_);
		}

		const char* str_;
		const unsigned len_;
	};//class Helper

	inline LogStreamff& operator<<(LogStreamff& s,Helper v)
	{
		s.append(v.str_,v.len_);
		return s;
	}

	inline LogStreamff& operator<<(LogStreamff& s,const Loggerff::SourceFile& file)
	{
		s.append(file.data_,file.size_);
		return s;
	}

	void defaultOutput(const char* msg,int len)
	{
		size_t n=fwrite(msg,1,len,stdout);
		(void)n;
	}

	void defaultFlush()
	{
		fflush(stdout);
	}

	Loggerff::OutputFunc g_output=defaultOutput;
	Loggerff::FlushFunc g_flush=defaultFlush;
}//namespace firey

using namespace firey;

Loggerff::Impl::Impl(LogLevel level,int savedErrno,const SourceFile& file,int line)
	:time_(Timestampff::now()),
	 stream_(),
	 level_(level),
	 line_(line),
	 basename_(file)
{
	formatTime();
	CurrentThreadff::tid();
	//将线程信息输出到LogStream的buffer_中
	stream_<<Helper(CurrentThreadff::tidString(),CurrentThreadff::tidStringLength());
	//将日志等级输出到buffer_中
	stream_<<Helper(LogLevelName[level],6);
	//如果有错误信息就将错误信息输出到buffer_中
	if(savedErrno!=0)
	{
		stream_<<strerror_tl(savedErrno)<<"(errno="<<savedErrno<<")";
	}
}

//采取了一定的优化措施，来避免在一秒内产生的日志，反复的格式化同样的信息（只重新格式化微秒）
void Loggerff::Impl::formatTime()
{
	int64_t microSecondsSinceEpoch=time_.usSinceEpoch();	

	time_t seconds=static_cast<time_t>(microSecondsSinceEpoch/Timestampff::kusPerSecond);
	int microseconds=static_cast<int>(microSecondsSinceEpoch%Timestampff::kusPerSecond);

	//不是在最后一秒内产生的日志,重新格式化秒之前的部分
	//则更新线程局部存储t_time
	if(seconds!=t_lastSecond)
	{
		t_lastSecond=seconds;
		struct tm tm_time;

		gmtime_r(&seconds,&tm_time);

		int len=snprintf(t_time,sizeof(t_time),"%4d%02d%02d %02d:%02d:%02d",
						 tm_time.tm_year+1900,tm_time.tm_mon+1,tm_time.tm_mday,
						 (tm_time.tm_hour+8)%24,tm_time.tm_min,tm_time.tm_sec);
		assert(len==17);(void)len;
	}

	//如果在同一秒内，只需要重新格式化微秒部分
	Fmt us(".%06d ",microseconds);
	assert(us.length()==8);
	stream_<<Helper(t_time,17)<<Helper(us.data(),8);
}

void Loggerff::Impl::finish()
{
	stream_<<" - "<<basename_<<':'<<line_<<'\n';
}

Loggerff::Loggerff(SourceFile file,int line)
	:impl_(INFO,0,file,line)
{}

Loggerff::Loggerff(SourceFile file,int line,LogLevel level)
	:impl_(level,0,file,line)
{}

Loggerff::Loggerff(SourceFile file,int line,LogLevel level,const char* func)
	:impl_(level,0,file,line)
{
	impl_.stream_<<func<<' ';
}

Loggerff::Loggerff(SourceFile file,int line,bool toAbort)
	:impl_(toAbort?FATAL:ERROR,errno,file,line)
{}

Loggerff::~Loggerff()
{
	impl_.finish();
	const LogStreamff::Buffer& buf(stream().buffer());

	//将fixedBuffer中的日志写到指定的目的地
	g_output(buf.data(),buf.length());

	//当此日志的等级为FATAL，
	if(impl_.level_==FATAL)
	{
		g_flush();
		abort();
	}
}

void Loggerff::setLogLevel(LogLevel level)
{
	g_logLevel=level;
}

void Loggerff::setFlush(FlushFunc flush)
{
	g_flush=flush;
}

void Loggerff::setOutput(OutputFunc output)
{
	g_output=output;
}
