#include "Bufferff.h"
#include "Socketopsff.h"

#include <sys/uio.h>

using namespace firey;

const char Bufferff::kCRLF[]="\r\n";

const size_t Bufferff::kCheapPrepend;
const size_t Bufferff::kInitialSize;

ssize_t Bufferff::readFd(int fd,int* savedErrno){
	char extrabuf[65536];
	struct iovec vec[2];
	const size_t writable=writableBytes();
	vec[0].iov_base=begin()+writerIndex_;
	vec[0].iov_len=writable;
	vec[1].iov_base=extrabuf;
	vec[1].iov_len=sizeof(extrabuf);

	const int iovcnt=(writable<sizeof extrabuf)?2:1;

	const ssize_t n=Socket::readv(fd,vec,iovcnt);

	if(n<0){
		*savedErrno=errno;
	}
	else if(static_cast<size_t>(n)<=writable){
		writerIndex_+=n;
	}
	else{
		writerIndex_=buffer_.size();
		append(extrabuf,n-writable);
	}
	return n;
}
