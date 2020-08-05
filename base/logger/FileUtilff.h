#ifndef FF_FILEUTIL_H_
#define FF_FILEUTIL_H_

#include <string>
#include <sys/types.h>

namespace firey{

class AppendFileff{

	public:
		AppendFileff(std::string filename);
		~AppendFileff();

		AppendFileff(const AppendFileff&)=delete;
		AppendFileff& operator=(const AppendFileff&)=delete;
		//往缓冲区中写入日志
		void append(const char* logline,size_t len);
		//刷新缓冲区
		void flush();

		off_t writtenBytes() const{return writtenBytes_;}

	private:
		size_t write(const char* logline,size_t len);
		
		FILE* fp_;//文件指针
		char buffer_[64*1024];//文件缓冲区
		off_t writtenBytes_;//已经写入文件的数量
};//class AppendFileff

class ReadSmallFileff{
	public:
		ReadSmallFileff(std::string filename);
		~ReadSmallFileff();
		
		ReadSmallFileff(const ReadSmallFileff&)=delete;
		ReadSmallFileff& operator=(const ReadSmallFileff&)=delete;

		//将文件内容读到String中
		template<typename String>
		int readToString(int maxSize,
				 	 	 String* content,
						 int64_t* fileSize,
						 int64_t* modifyTime,
						 int64_t* createTime);

		//将文件内容读到buffer_中
		int readToBuffer(int* size);

		const char* buffer() const{return buffer_;}

		static const int kBufferSize=64*1024;

	private:
		int fd_;
		int err_;//存放errno
		char buffer_[kBufferSize];//缓冲区
};//class ReadSmallFileff

template <typename String>
int readFile(std::string filename,
			 int maxSize,
			 String* content,
			 int64_t* fileSize,
			 int64_t* modifyTime,
			 int64_t* createTime)
{
	ReadSmallFileff file(filename);
	return file.readToString(maxSize,content,fileSize,modifyTime,createTime);
}

}//namespace firey

#endif //FF_FILEUTIL_H_
