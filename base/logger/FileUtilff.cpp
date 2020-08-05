#include "FileUtilff.h"
#include "Loggingff.h"

#include <stdio.h>
#include <assert.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

using namespace firey;

AppendFileff::AppendFileff(std::string filename)
	:fp_(::fopen(filename.c_str(),"ae")),
	writtenBytes_(0)
{
	assert(fp_);
	::setbuffer(fp_,buffer_,sizeof buffer_);
}

AppendFileff::~AppendFileff()
{
	::fclose(fp_);
}

void AppendFileff::append(const char* logline,size_t len)
{
	size_t n=write(logline,len);
	size_t remain=len-n;
	while(remain>0)
	{
		size_t m=write(logline+n,remain);
		if(m==0)
		{
			int err=ferror(fp_);
			if(err)
			{
				fprintf(stderr,"AppendFileff::append() failed %s\n",strerror_tl(err));
			}
			break;
		}
		n+=m;
		remain-=m;
	}
	writtenBytes_+=len;
}

void AppendFileff::flush()
{
	::fflush(fp_);
}

size_t AppendFileff::write(const char* logline,size_t len)
{
	//日志文件只有日志线程可以操作，故不用加锁，提高写入文件的效率
	return ::fwrite_unlocked(logline,1,len,fp_);
}

/*ReadSmallFileff implementation*/
ReadSmallFileff::ReadSmallFileff(std::string filename)
	:fd_(::open(filename.c_str(),O_RDONLY|O_CLOEXEC)),
	err_(0)
{
	buffer_[0]='\0';
	if(fd_<0)
	{
		err_=errno;
	}
}

ReadSmallFileff::~ReadSmallFileff()
{
	if(fd_>=0)
	{
		::close(fd_);
	}
}

	template <typename String>
int ReadSmallFileff::readToString(int maxSize,
		String* content,
		int64_t* fileSize,
		int64_t* modifyTime,
		int64_t* createTime)
{
	static_assert(sizeof(off_t)==8,"_FILE_OFFSET_BIT=64");
	assert(content!=nullptr);
	int err=err_;
	if(fd_>=0)
	{
		content->clear();

		if(fileSize)
		{
			struct stat statbuf;
			if(::fstat(fd_,&statbuf)==0)
			{
				//fd_指向的文件是普通文件
				if(S_ISREG(statbuf.st_mode))
				{
					*fileSize=statbuf.st_size;//获取读取文件的大小
					content->reserve(static_cast<int>(std::min(static_cast<int64_t>(maxSize),*fileSize)));
				}
				//fd_指向的文件是目录
				else if(S_ISDIR(statbuf.st_mode))
				{
					err=EISDIR;
				}

				if(modifyTime)
				{
					*modifyTime=statbuf.st_mtime;
				}
				if(createTime)
				{
					*createTime=statbuf.st_ctime;
				}
			}
			else
				err=errno;
		}

		//开始读取文件
		while(content->size()<static_cast<size_t>(maxSize))
		{
			size_t toRead=std::min(static_cast<size_t>(maxSize)-content->size(),sizeof(buffer_));
			ssize_t n=::read(fd_,buffer_,toRead);
			if(n>0)
			{
				content->append(buffer_,n);
			}
			else
			{
				if(n<0) err=errno;
				break;
			}
		}
	}
	return err;
}

int ReadSmallFileff::readToBuffer(int* size)
{
	int err=err_;
	if(fd_>=0)
	{
		ssize_t n=::pread(fd_,buffer_,sizeof(buffer_)-1,0);
		if(n>=0)
		{
			if(size) 
				*size=static_cast<int>(n);
			buffer_[n]='\0';
		}
		else
			err=errno;
	}
	return err;
}

//??
template int firey::readFile(std::string filename,
		 	  	  	  int maxSize,
					  std::string* content,
					  int64_t*,int64_t*,int64_t*);

template int ReadSmallFileff::readToString(
		 	  	  	  int maxSize,
					  std::string* content,
					  int64_t*,int64_t*,int64_t*);
