#include "AsyncLoggingff.h"
#include "LogFileff.h"
#include "Timestampff.h"

#include <assert.h>

using namespace firey;

AsyncLoggingff::AsyncLoggingff(const std::string& basename,
		off_t rollSize,
		int flushInterval)
	:basename_(basename),
	rollSize_(rollSize),
	flushInterval_(flushInterval),
	running_(false),
	thread_(std::bind(&AsyncLoggingff::threadFunc,this),"logging"),
	latch_(1),
	mutex_(),
	cond_(mutex_),
	currentBuffer_(new Buffer),
	nextBuffer_(new Buffer),
	buffers_()
{
	currentBuffer_->bZero();
	nextBuffer_->bZero();
	buffers_.reserve(16);
}

void AsyncLoggingff::append(const char* logline,int len)
{
	MutexGuardff lock(mutex_);
	if(currentBuffer_->avail()>len)
	{
		currentBuffer_->append(logline,len);
	}
	else
	{
		buffers_.push_back(std::move(currentBuffer_));
		if(nextBuffer_)
		{
			currentBuffer_ = std::move(nextBuffer_);
		}
		else
		{
			currentBuffer_.reset(new Buffer);
		}
		assert(currentBuffer_->avail()>=len);
		currentBuffer_->append(logline,len);
		cond_.notify();
	}
}

//异步日志线程的主函数

void AsyncLoggingff::threadFunc()
{
	assert(running_);
	latch_.countDown();

	LogFileff outputFile(basename_,rollSize_,false);
	BufferPtr newBuffer1(new Buffer);
	BufferPtr newBuffer2(new Buffer);
	newBuffer1->bZero();
	newBuffer2->bZero();

	BufferVector bufferToWrite;

	bufferToWrite.reserve(16);

	while(running_)
	{
		assert(newBuffer1&&newBuffer1->length()==0);
		assert(newBuffer2&&newBuffer2->length()==0);
		assert(bufferToWrite.size()==0);

		//临界区
		{
			MutexGuardff lock(mutex_);
			if(buffers_.empty())
			{
				cond_.waitForSeconds(flushInterval_);
			}
			buffers_.push_back(std::move(currentBuffer_));
			currentBuffer_=std::move(newBuffer1);
			bufferToWrite.swap(buffers_);
			if(!nextBuffer_)
			{
				nextBuffer_=std::move(newBuffer2);
			}
		}
		assert(!bufferToWrite.empty());

		//如果日志线程中待写入日志文件的缓冲区过多，则抛弃一些缓冲区
		if(bufferToWrite.size()>25)
		{
			char buf[256];
			snprintf(buf,sizeof buf,"Dropped log message at %s, %zd larger buffers\n",
					Timestampff::now().toFormatString().c_str(),
					bufferToWrite.size()-2);
			fputs(buf,stderr);
			outputFile.append(buf,static_cast<int>(strlen(buf)));
			bufferToWrite.erase(bufferToWrite.begin()+2,bufferToWrite.end());
		}

		for(const auto& buffer:bufferToWrite)
		{
			outputFile.append(buffer->data(),buffer->length());
		}

		if(bufferToWrite.size()>2)
		{
			bufferToWrite.resize(2);
		}

		if(!newBuffer1)
		{
			assert(!bufferToWrite.empty());
			newBuffer1=std::move(bufferToWrite.back());
			bufferToWrite.pop_back();
			newBuffer1->reset();
		}

		if(!newBuffer2)
		{
			assert(!bufferToWrite.empty());
			newBuffer2=std::move(bufferToWrite.back());
			bufferToWrite.pop_back();
			newBuffer2->reset();
		}

		bufferToWrite.clear();
		outputFile.flush();
	}
	outputFile.flush();
}
