#ifndef FF_LOGGING_H_
#define FF_LOGGING_H_

#include "LogStreamff.h"
#include "Timestampff.h"

namespace firey{

class Loggerff{
	public:
		enum LogLevel
		{
			TRACE,
			DEBUG,
			INFO,
			WARN,
			ERROR,
			FATAL,
			NUM_LOG_LEVELS
		};//enum LogLevel

		//获取源文件名（去掉路径）
		class SourceFile{
		public:
			template<int N>
			SourceFile(const char (&arr)[N])
				:data_(arr),
				 size_(N-1)
			{
				//获取data_中指向最后一个'/'的指针
				const char* slash=strrchr(data_,'/');
				if(slash)
				{
					data_=slash+1;
					size_-=static_cast<int>(data_-arr);
				}
			}

			explicit SourceFile(const char* filename)
				:data_(filename)
			{
				const char* slash=strrchr(filename,'/');
				if(slash)
				{
					data_=slash+1;
				}
				size_=static_cast<int>(strlen(data_));
			}

			const char* data_;
			int size_;
		};//class SorceFile

		//Logger's constructor
		//创建临时Logger对象的源文件和行号
		Loggerff(SourceFile file,int line);
		//创建临时Logger对象的源文件和行号,以及对应的日志级别
		Loggerff(SourceFile file,int line,LogLevel level);
		//创建临时Logger对象的源文件和行号，对应的日志级别和创建Logger对象的函数
		Loggerff(SourceFile file,int line,LogLevel level,const char* func);
		//创建临时Logger对象的源文件和行号,是否要abort()
		Loggerff(SourceFile file,int line,bool toAbort);
		~Logger();

		LogStreaff& stream(){return impl_.stream_;}

		static LogLevel logLevel();
		static void setLogLevel(LogLevel level);

		typedef void (*OutputFunc)(const char* msg,int len);
		typedef void (*FlushFunc)();

		static void setOutput(OutputFunc);
		static void setFlush(FlushFunc);

	private:
		//Logger类中功能的实现类
		class Impl{
		public:
			typedef Loggerff::LogLevel LogLevel;
			Impl(LogLevel level,int old_errno,const SourceFile& file,int line);
			void formatTime();
			void finish();

			Timestampff time_;
			LogStreamff stream_;
			LogLevel level_;
			int line_;
			SourceFile basename_;
		};//class Impl

		Impl impl_;

};//class Loggingff

extern Loggerff::LogLevel g_logLevel;
inline Loggerff::logLevel()
{
	return g_logLevel;
}

//定义日志输出的宏
#define LOG_TRACE if(Loggerff::logLevel()<=Loggerff::TRACE) \
	Loggerff(__FILE__,__LINE__,Loggerff::TRACE,__func__).stream()
#define LOG_DEBUG if(Loggerff::LogLevel()<=Loggerff::DEBUG) \
	loggerff(__FILE__,__LINE__,Loggerff::DEBUG,__func__).stream()
#define LOG_INFO if(Loggerff::LogLevel()<=Loggerff::INFO) \
	Loggerff(__FILE__,__LINE__).stream()
#define LOG_WARN Loggerff(__FILE__,__LINE__,Loggerff::WARN).stream()
#define LOG_ERROR Loggerff(__FILE__,__LINE__,Loggerff::ERROR).stream()
#define LOG_FATAL Loggerff(__FILE__,__LINE__,Loggerff::FATAL).stream()
#define LOG_SYSERR Loggerff(__FILE__,__LINE__,false).stream()//do not abort
#define LOG_SYSFATAL Loggerff(__FILE__,__LINE__,true).stream()//abort

const char* strerror_tl(int savedError);

}//namespace firey

#endif//FF_LOGGER_H
