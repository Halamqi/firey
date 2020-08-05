#ifndef FF_ASYNCLOGGING_H_
#define FF_ASYNCLOGGING_H_

#include "Threadff.h"
#include "Conditionff.h"
#include "MutexGuardff.h"
#include "LogStreamff.h"
#include "CountDownLatchff.h"

#include <string>
#include <vector>
#include <atomic>

namespace firey{

class AsyncLoggingff{

	public:
		AsyncLoggingff(const AsyncLoggingff&)=delete;
		AsyncLoggingff& operator=(const AsyncLoggingff&)=delete;

		AsyncLoggingff(const std::string& basename,
					   off_t rollSize,
					   int flushInterval=3);
		
		~AsyncLoggingff()
		{
			if(running_)
				stop();
		}

		void append(const char* logline,int len);

		void start()
		{
			running_=true;
			thread_.start();
			latch_.wait();
		}

		void stop()
		{
			running_=false;
			cond_.notify();
			thread_.join();
		}
	private:
		
		void threadFunc();//日志线程的主函数

		typedef FixedBufferff<kLargeBuffer> Buffer;
		typedef std::vector<std::unique_ptr<Buffer>> BufferVector;
		typedef std::unique_ptr<Buffer> BufferPtr;

		const std::string basename_;//argc中的程序名称
		const off_t rollSize_;//每向文件中写入多大的日志，就换一个日志文件继续写
		const int flushInterval_;//每隔多少秒，将日志写入日志文件
		std::atomic<bool> running_;
		
		Threadff thread_;//异步日志线程
		CountDownLatchff latch_;//门闩

		Mutexff mutex_;//互斥锁，用来同步日志的前端和后端
		Conditionff cond_;//条件变量，用来进行线程同步

		BufferPtr currentBuffer_;
		BufferPtr nextBuffer_;

		BufferVector buffers_;
};//class AsyncLoggingff

}//namespace firey

#endif //FF_ASYNCLOGGING_H_
