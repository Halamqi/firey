#ifndef FF_LOGGING_H_
#define FF_LOGGING_H_

#include "LogStreamff.h"
#include "Timestampff.h"

namespace firey{

class Loggerff
{
	public:
		enum LogLevel
		{
			TRACE,
			DEBUG,
			INFO,
			WARN,
			ERROR,
			FATAL,
			NUM_LOG_LEVELS,
		};

		// compile time calculation of basename of source file
		class SourceFile
		{
			public:
				//??????
				template<int N>
					SourceFile(const char (&arr)[N])
					: data_(arr),
					size_(N-1)
			{
				const char* slash = strrchr(data_, '/'); // builtin function
				if (slash)
				{
					data_ = slash + 1;
					size_ -= static_cast<int>(data_ - arr);
				}
			}

				explicit SourceFile(const char* filename)
					: data_(filename)
				{
					const char* slash = strrchr(filename, '/');
					if (slash)
					{
						data_ = slash + 1;
					}
					size_ = static_cast<int>(strlen(data_));
				}

				const char* data_;
				int size_;
		};

		Loggerff(SourceFile file, int line);
		Loggerff(SourceFile file, int line, LogLevel level);
		Loggerff(SourceFile file, int line, LogLevel level, const char* func);
		Loggerff(SourceFile file, int line, bool toAbort);
		~Loggerff();

		//返回logger持有的LogStream&
		LogStreamff& stream() { return impl_.stream_; }

		static LogLevel logLevel();
		static void setLogLevel(LogLevel level);

		typedef void (*OutputFunc)(const char* msg, int len);
		typedef void (*FlushFunc)();
		static void setOutput(OutputFunc);
		static void setFlush(FlushFunc);
		///omit

	private:
		//Loggerff的实现类，将一条日志的所需信息都放到Impl实现类中
		class Impl
		{
			public:
				typedef Loggerff::LogLevel LogLevel;
				Impl(LogLevel level, int old_errno, const SourceFile& file, int line);
				void formatTime();
				void finish();

				Timestampff time_;
				LogStreamff stream_;
				LogLevel level_;
				int line_;
				SourceFile basename_;
		};

		Impl impl_;
};

extern Loggerff::LogLevel g_logLevel;

inline Loggerff::LogLevel Loggerff::logLevel()
{
	return g_logLevel;
}


//定义日志输出的宏
#define LOG_TRACE if (firey::Loggerff::logLevel() <= firey::Loggerff::TRACE) \
	firey::Loggerff(__FILE__, __LINE__, firey::Loggerff::TRACE, __func__).stream()
#define LOG_DEBUG if (firey::Loggerff::logLevel() <= firey::Loggerff::DEBUG) \
	firey::Loggerff(__FILE__, __LINE__, firey::Loggerff::DEBUG, __func__).stream()
#define LOG_INFO if (firey::Loggerff::logLevel() <= firey::Loggerff::INFO) \
	firey::Loggerff(__FILE__, __LINE__).stream()
#define LOG_WARN firey::Loggerff(__FILE__, __LINE__, firey::Loggerff::WARN).stream()
#define LOG_ERROR firey::Loggerff(__FILE__, __LINE__, firey::Loggerff::ERROR).stream()
#define LOG_FATAL firey::Loggerff(__FILE__, __LINE__, firey::Loggerff::FATAL).stream()
#define LOG_SYSERR firey::Loggerff(__FILE__, __LINE__, false).stream()
#define LOG_SYSFATAL firey::Loggerff(__FILE__, __LINE__, true).stream()

const char* strerror_tl(int savedErrno);

}//namespace firey

#endif//FF_LOGGING_H_
