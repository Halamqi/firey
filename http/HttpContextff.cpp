#include "HttpContextff.h"

#include "Bufferff.h"

using namespace firey;

bool HttpContextff::processRequestLine(const char* begin,const char* end)
{
	bool succeed=false;
	const char* start=begin;
	const char* space=std::find(start,end,' ');
	if(space!=end&&request_.setMethod(start,space))
	{
		start=space+1;
		space=std::find(start,end,' ');
		if(space!=end)
		{
			const char* question=std::find(start,space,'?');
			if(question!=space)
			{
				request_.setPath(start,question);
				request_.setQuery(question,space);
			}
			else
			{
				request_.setPath(start,space);
			}

			start=space+1;

			succeed=end-start==8&&std::equal(start,end-3,"HTTP/");
			if(succeed)
			{
				if(std::equal(end-3,end-1,"1.0"))
				{
					request_.setVersion(HttpRequestff::kHttp10);
				}
				else if(std::equal(end-3,end-1,"1.1"))
				{
					request_.setVersion(HttpRequestff::kHttp11);
				}
				else if(std::equal(end-3,end-1,"2.0"))
				{
					request_.setVersion(HttpRequestff::kHttp20);
				}
				else succeed=false;
			}
		}
	}
	return succeed;
}

bool HttpContextff::parseRequest(Bufferff* buffer,Timestampff receiveTime)
{
	bool ok=true;
	bool hasMore=true;
	while(hasMore)
	{
		if(state_==kExpectRequestLine)
		{
			//在buffer中找到请求行，提取出来
			const char* crlf=buffer->findCRLF();
			if(crlf)
			{
				//处理请求行
				ok=processRequestLine(buffer->peek(),crlf);
				if(ok)
				{
					request_.setReceiveTime(receiveTime);
					buffer->retrieveUntil(crlf+2);
					state_=kExpectHeaders;
				}
				else hasMore=false;
			}
			else
			{
				hasMore=false;
			}
		}

		else if(state_==kExpectHeaders)
		{
			const char* crlf=buffer->findCRLF();
			if(crlf)
			{
				const char* colon=std::find(buffer->peek(),crlf,':');
				if(colon!=crlf)
				{
					request_.addHeader(buffer->peek(),colon,crlf);
				}
				else
				{
					//TODO
					state_=kGotAll;
					hasMore=false;
				}
				buffer->retrieveUntil(crlf+2);
			}
			else 
				hasMore=false;
		}

		else if(state_==kExpectBody)
		{
			//TODO
		}
	}
	return ok;
}
