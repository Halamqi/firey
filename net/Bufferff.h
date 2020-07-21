#ifndef FF_BUFFER_H
#define FF_BUFFER_H

#include <algorithm>
#include <vector>

#include <assert.h>
#include <string.h>
#include <string>
#include <endian.h>

//#include <unistd.h>  // ssize_t

namespace firey
{

	//在Bufferff中的数据都是网络字节序

	/// A buffer class modeled after org.jboss.netty.buffer.ChannelBufferff
	///
	/// @code
	/// +-------------------+------------------+------------------+
	/// | prependable bytes |  readable bytes  |  writable bytes  |
	/// |                   |     (CONTENT)    |                  |
	/// +-------------------+------------------+------------------+
	/// |                   |                  |                  |
	/// 0      <=      readerIndex   <=   writerIndex    <=     size
	/// @endcode
	class Bufferff  
	{
		public:
			static const size_t kCheapPrepend = 8;
			static const size_t kInitialSize = 1024;

			explicit Bufferff(size_t initialSize = kInitialSize)
				: buffer_(kCheapPrepend + initialSize),
				readerIndex_(kCheapPrepend),
				writerIndex_(kCheapPrepend)
		{
			assert(readableBytes() == 0);
			assert(writableBytes() == initialSize);
			assert(prependableBytes() == kCheapPrepend);
		}

			// implicit copy-ctor, move-ctor, dtor and assignment are fine
			// NOTE: implicit move-ctor is added in g++ 4.6



			//隐式的拷贝构造函数，复制一个缓冲区对象
			void swap(Bufferff& rhs)
			{
				buffer_.swap(rhs.buffer_);
				std::swap(readerIndex_, rhs.readerIndex_);
				std::swap(writerIndex_, rhs.writerIndex_);
			}

			//获取可读缓冲区的长度
			size_t readableBytes() const
			{ return writerIndex_ - readerIndex_; }

			//获取可写缓冲区的长度
			size_t writableBytes() const
			{ return buffer_.size() - writerIndex_; }

			//获取缓冲区的前缀长度
			size_t prependableBytes() const
			{ return readerIndex_; }

			//可写缓冲区的开始的地址
			const char* peek() const
			{ return begin() + readerIndex_; }




			//在可读区寻找"\r\n"
			const char* findCRLF() const
			{
				// FIXME: replace with memmem()?
				const char* crlf = std::search(peek(), beginWrite(), kCRLF, kCRLF+2);
				return crlf == beginWrite() ? NULL : crlf;
			}

			//在可读区的start处开始寻找"\r\n"
			const char* findCRLF(const char* start) const
			{
				assert(peek() <= start);
				assert(start <= beginWrite());
				// FIXME: replace with memmem()?
				const char* crlf = std::search(start, beginWrite(), kCRLF, kCRLF+2);
				return crlf == beginWrite() ? NULL : crlf;
			}

			//在可读取区中寻找'\n'
			const char* findEOL() const
			{
				const void* eol = memchr(peek(), '\n', readableBytes());
				return static_cast<const char*>(eol);
			}
			//以start开始到writeindex寻找'\n'
			const char* findEOL(const char* start) const
			{
				assert(peek() <= start);
				assert(start <= beginWrite());
				const void* eol = memchr(start, '\n', beginWrite() - start);
				return static_cast<const char*>(eol);
			}


			/*可读区索引的维护*/
			// retrieve returns void, to prevent
			// string str(retrieve(readableBytes()), readableBytes());
			// the evaluation of two functions are unspecified
			//获取缓冲区中从peek开始的len个字节大的数据
			void retrieve(size_t len)
			{
				assert(len <= readableBytes());
				if (len < readableBytes())
				{
					readerIndex_ += len;
				}
				else
				{
					retrieveAll();
				}
			}

			//获取从可读缓冲区中peek()开始到end的数据
			void retrieveUntil(const char* end)
			{
				assert(peek() <= end);
				assert(end <= beginWrite());
				retrieve(end - peek());
			}

			void retrieveInt64()
			{
				retrieve(sizeof(int64_t));
			}

			void retrieveInt32()
			{
				retrieve(sizeof(int32_t));
			}

			void retrieveInt16()
			{
				retrieve(sizeof(int16_t));
			}

			void retrieveInt8()
			{
				retrieve(sizeof(int8_t));
			}

			//获取读缓冲区中的所有数据
			void retrieveAll()
			{
				readerIndex_ = kCheapPrepend;
				writerIndex_ = kCheapPrepend;
			}

			//以字符串的形式获取读缓冲区中的数据
			std::string retrieveAllAsString()
			{
				return retrieveAsString(readableBytes());
			}

			//在可读区中peek()开始以字符串的形式读取len长度的数组
			std::string retrieveAsString(size_t len)
			{
				assert(len <= readableBytes());
				std::string result(peek(), len);
				retrieve(len);
				return result;
			}

			//将可读区的数据转换成StringPiece类
			std::string toString() const
			{
				return std::string(peek(), static_cast<int>(readableBytes()));
			}



			/*可写区添加数据*/

			//将data指向的len长度的数据拷贝到可写区
			void append(const char* /*restrict*/ data, size_t len)
			{
				//确保可写区长度大于等于len
				ensureWritableBytes(len);
				std::copy(data, data + len, beginWrite());
				hasWritten(len);
			}

			//将StringPiece对象的数据拷贝到可写区，调用append(const char*,size_t)
			void append(const std::string& str)
			{
				append(str.data(), str.size());
			}

			//将任意类型的数据拷贝到可写区
			void append(const void* /*restrict*/ data, size_t len)
			{
				append(static_cast<const char*>(data), len);
			}

			//确保可写区的长度大于等于len
			void ensureWritableBytes(size_t len)
			{
				//如果可写区的长度小于len，则扩容
				if (writableBytes() < len)
				{
					makeSpace(len);
				}
				assert(writableBytes() >= len);
			}

			//返回可写区的首地址
			char* beginWrite()
			{ return begin() + writerIndex_; }

			const char* beginWrite() const
			{ return begin() + writerIndex_; }

			//往可写区中写入数据之后，更新可写区的首地址
			void hasWritten(size_t len)
			{
				assert(len <= writableBytes());
				writerIndex_ += len;
			}

			//将可写区中len长的数据抹除
			void unwrite(size_t len)
			{
				assert(len <= readableBytes());
				writerIndex_ -= len;
			}



			/*往可写区中添加整数（网络字节序）*/
			///
			/// Append int64_t using network endian
			///

			//void append(const void* /*restrict*/ data, size_t len)
			//调用如上的append函数，将一个整型数据添加到可写区中
			void appendInt64(int64_t x)
			{
				int64_t be64 = htobe64(x);
				append(&be64, sizeof be64);
			}

			///
			/// Append int32_t using network endian
			///
			void appendInt32(int32_t x)
			{
				int32_t be32 = htobe32(x);
				append(&be32, sizeof be32);
			}

			void appendInt16(int16_t x)
			{
				int16_t be16 = htobe16(x);
				append(&be16, sizeof be16);
			}

			void appendInt8(int8_t x)
			{
				append(&x, sizeof x);
			}




			/*读取可读区中的整数（主机字节序），并未获索引（真正的取出来）*/
			///
			/// Read int64_t from network endian
			///
			/// Require: buf->readableBytes() >= sizeof(int32_t)
			//从可读区中读取整数，先调用peekInt获取整数，然后取出
			int64_t readInt64()
			{
				int64_t result = peekInt64();
				retrieveInt64();
				return result;
			}
			///
			/// Read int32_t from network endian
			///
			/// Require: buf->readableBytes() >= sizeof(int32_t)
			int32_t readInt32()
			{
				int32_t result = peekInt32();
				retrieveInt32();
				return result;
			}

			int16_t readInt16()
			{
				int16_t result = peekInt16();
				retrieveInt16();
				return result;
			}

			int8_t readInt8()
			{
				int8_t result = peekInt8();
				retrieveInt8();
				return result;
			}



			/*获取可读区中的整数（主机字节序），从peek()开始，只是获取并没有取出*/
			///
			/// Peek int64_t from network endian
			///
			/// Require: buf->readableBytes() >= sizeof(int64_t)
			int64_t peekInt64() const
			{
				assert(readableBytes() >= sizeof(int64_t));
				int64_t be64 = 0;
				::memcpy(&be64, peek(), sizeof be64);
				return be64toh(be64);
			}

			///
			/// Peek int32_t from network endian
			///
			/// Require: buf->readableBytes() >= sizeof(int32_t)
			int32_t peekInt32() const
			{
				assert(readableBytes() >= sizeof(int32_t));
				int32_t be32 = 0;
				::memcpy(&be32, peek(), sizeof be32);
				return be32toh(be32);
			}

			int16_t peekInt16() const
			{
				assert(readableBytes() >= sizeof(int16_t));
				int16_t be16 = 0;
				::memcpy(&be16, peek(), sizeof be16);
				return be16toh(be16);
			}

			int8_t peekInt8() const
			{
				assert(readableBytes() >= sizeof(int8_t));
				int8_t x = *peek();
				return x;
			}





			//往预分配区中添加data开始的长度为len的数据
			void prepend(const void* /*restrict*/ data, size_t len)
			{
				assert(len <= prependableBytes());
				readerIndex_ -= len;
				const char* d = static_cast<const char*>(data);
				std::copy(d, d + len, begin() + readerIndex_);

			}

			//往预分配区中添加整数，（以网络字节序添加）
			///
			/// Prepend int64_t using network endian
			///
			void prependInt64(int64_t x)
			{
				int64_t be64 = htobe64(x);
				prepend(&be64, sizeof be64);
			}

			///
			/// Prepend int32_t using network endian
			///
			void prependInt32(int32_t x)
			{
				int32_t be32 = htobe32(x);
				prepend(&be32, sizeof be32);
			}

			void prependInt16(int16_t x)
			{
				int16_t be16 = htobe16(x);
				prepend(&be16, sizeof be16);
			}

			void prependInt8(int8_t x)
			{
				prepend(&x, sizeof x);
			}



			//收缩Bufferff
			void shrink(size_t reserve)
			{
				// FIXME: use vector::shrink_to_fit() in C++ 11 if possible.
				Bufferff other;
				other.ensureWritableBytes(readableBytes()+reserve);
				other.append(toString());
				swap(other);
			}

			size_t internalCapacity() const
			{
				return buffer_.capacity();
			}

			/// Read data directly into buffer.
			///
			/// It may implement with readv(2)
			/// @return result of read(2), @c errno is saved
			ssize_t readFd(int fd, int* savedErrno);

		private:

			char* begin()
			{ return &*buffer_.begin(); }

			const char* begin() const
			{ return &*buffer_.begin(); }

			void makeSpace(size_t len)
			{
				if (writableBytes() + prependableBytes() < len + kCheapPrepend)
				{
					// FIXME: move readable data
					buffer_.resize(writerIndex_+len);
				}
				else
				{
					// move readable data to the front, make space inside buffer
					assert(kCheapPrepend < readerIndex_);
					size_t readable = readableBytes();
					std::copy(begin()+readerIndex_,
							begin()+writerIndex_,
							begin()+kCheapPrepend);
					readerIndex_ = kCheapPrepend;
					writerIndex_ = readerIndex_ + readable;
					assert(readable == readableBytes());
				}
			}

		private:
			std::vector<char> buffer_;
			size_t readerIndex_;
			size_t writerIndex_;

			static const char kCRLF[];
	};

}  // namespace firey

#endif  // FF_BUFFER_H
