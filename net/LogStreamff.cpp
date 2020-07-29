#include "LogStreamff.h"

#include <algorithm>
#include <limits>
#include <type_traits>
#include <stdint.h>
#include <stdio.h>
#include <assert.h>

#ifndef __STDC_FORMAT_MACROS
#define __STDC_FORMAT_MACROS
#endif

#include <inttypes.h>

using namespace firey;

#if defined(__clang__)
#pragma clang diagnostic ignored "-Wtautological-compare"
#else
#pragma GCC diagnostic ignored "-Wtype-limits"
#endif

namespace firey{

	//details
	const char digits[]="9876543210123456789";
	const char* zero=digits+9;
	static_assert(sizeof(digits)==20,"wrong number of digits");

	const char digitHex[]="0123456789ABCDEF";
	static_assert(sizeof(digitHex)==17,"wrong number of digitsHex");

	//将整数转换成字符串
	template <typename T>
	size_t convert(char buf[],T val)
	{
		T i=value;
		char* p=buf;

		while(i!=0)
		{
			int lsd=static_cast<int>(i%10);
			i/=10;
			*p++=zero[lsd];
		}

		if(value<0)
		{
			*p++='-';
		}
		*p='\0';
		std::reverse(buf,p);
		return p-buf;
	}

	size_t convertHex(char buf[],uintptr_t value)
	{
		uintptr_t i=value;
		char* p=buf;

		while(i!=0)
		{
			int lsd=static_cast<int>(i%16);
			i/=16;
			*p++=digitHex[lsd];
		}
		
		*p='\0';
		std::reverse(buf,p);
		return p-buf;
	}

	template class FixedBufferff<kSmallBuffer>;
	template class FixedBufferff<kLargeBuufer>;

	//LogStreamff implementation
	void LogStreamff::staticCheck()
	{
		static_assert(kMaxNumericSize-10>std::numeric_limits<double>::digits10,
					  "kMaxNumericSize is large enough");
		static_assert(kmaxNumericSize-10>std::numeric_limits<long double>::digits10,
					 "kMaxNumericSize is large enough");
		static_assert(kmaxNumericSize-10>std::numeric_limits<long>::digits10,
					 "kMaxNumericSize is large enough");
		static_assert(kmaxNumericSize-10>std::numeric_limits<long long>::digits10,
					 "kMaxNumericSize is large enough");
	}

	template <typename T>
	void LogStreamff::formatInteger(T v)
	{
		if(buffer_.avail()>=kMaxNumericSize)
		{
			size_t len=convert(buffer_.curPos(),v);
			//维护buffer_的当前指针指向
			buffer_.add(len);
		}
	}

	LogStreamff& LogStreamff::operator<<(short v)
	{
		*this<<static_cast<int>(v);
		return *this;
	}

	LogStreamff& LogStreamff::operator<<(unsigned short v)
	{
		*this<<static_cast<unsigned int>(v);
		return *this;
	}

	LogStreamff& LogStreamff::operator<<(int v)
	{
		formatInteger(v);
		return *this;
	}

	LogStreamff& LogStreamff::operator<<(unsigned int v)
	{
		formatInteger(v);
		return *this;
	}

	LogStreamff& LogStreamff::operator<<(long v)
	{
		formatInteger(v);
		return *this;
	}

	LogStreamff& LogStreamff::operator<<(unsigned long v)
	{
		formatInteger(v);
		return *this;
	}
	
	LogStreamff& LogStreamff::operator<<(long long v)
	{
		formatInteger(v);
		return *this;
	}

	LogStreamff& LogStreamff::operator<<(unsigned long long v)
	{
		formatInteger(v);
		return *this;
	}


}
