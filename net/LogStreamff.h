#ifndef FF_LOGSTREAM_H_
#define FF_LOGSTREAM_H_

#include <string>
#include <string.h>

namespace firey{

const int kSmallBuffer=4000;
const int kLargeBuffer=4000*1000;

template <int SIZE>
class FixedBufferff
{

	public:
		FixedBufferff(const FixedBufferff&)=delete;
		FixedBufferff& operator=(const FixedBufferff&)=delete;

		FixedBufferff()
			:cur_(data_)
		{}

		~FixedBufferff()
		{}

		int avail() const {return static_cast<int>(end()-cur_);}
		char* curPos() {return cur_;}
		const char* data() const {return data_;}
		int length() const {return static_cast<int>(cur_-data_);}
		void add(size_t len) {cur_+=len;}
		
		void reset() {cur_=data_;}
		void bZero() {memset(data_,0,sizeof data_);}

		std::string toString() const {return std::string(data_,length());}

		void append(const char* buf,size_t len)
		{
			if(static_cast<size_t> (avail())<len) 
				len=avail();
			memcpy(cur_,buf,len);
			add(len);
		}
	private:
		char data_[SIZE];
		char* cur_;
		const char* end() const {return data_+sizeof(data_);}

};//class FixedBuffer

class LogStreamff
{
	
	typedef LogStreamff self;
	public:
		LogStreamff()=default;

		LogStreamff(const LogStreamff&)=delete;
		LogStreamff& operator=(const LogStreamff&)=delete;

		typedef FixedBufferff<kSmallBuffer> Buffer;
		
		self& operator<<(bool v)
		{
			buffer_.append(v?"1":"0",1);
			return *this;
		}

		self& operator<<(short);
		self& operator<<(unsigned short);
		self& operator<<(int);
		self& operator<<(unsigned int);
		self& operator<<(long);
		self& operator<<(unsigned long);
		self& operator<<(long long);
		self& operator<<(unsigned long long);

		self& operator<<(const void*);

		self& operator<<(float v)
		{
			*this<<static_cast<double>(v);
			return *this;
		}
		self& operator<<(double);

		self& operator<<(char v)
		{
			buffer_.append(&v,1);
			return *this;
		}

		self& operator<<(const char* str)
		{
			if(str)
				buffer_.append(str,strlen(str));
			else
				buffer_.append("(null)",6);
			return *this;
		}

		self& operator<<(const unsigned char* str)
		{
			return operator<<(reinterpret_cast<const char*>(str));
		}

		self& operator<<(const std::string& str)
		{
			buffer_.append(str.data(),str.size());
			return *this;
		}

		self& operator<<(const Buffer& v)
		{
			//return operator<<(v.toString());
			*this<<(v.toString());
			return *this;
		}

		void append(const char* data,size_t len)
		{
			buffer_.append(data,len);
		}
		
		const Buffer& buffer() const {return buffer_;}
		void resetBuffer() {buffer_.reset();}

	private:
		
		template <typename T>
		void formatInteger(T);
		
		//用来临时存放每次产生的日志
		Buffer buffer_;
		
		static const int kMaxNumericSize=32;

		void staticCheck();

};//class LogStreamff

//构造是将val按fmt格式打印到buf_中
class Fmt
{
	public:
		template <typename T>
		Fmt(const char* fmt,T val);

		const char* data() const {return buf_;}
		size_t length() const {return length_;}

	private:
		char buf_[32];
		size_t length_;
};//class Fmt

inline LogStreamff& operator<<(LogStreamff& s,const Fmt& fmt)
{
	s.append(fmt.data(),fmt.length());
	return s;
}

}//namespace firey

#endif //FF_LOGSTREAM_H_
