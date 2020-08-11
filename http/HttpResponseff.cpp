#include "HttpResponseff.h"

#include "Bufferff.h"

#include <stdio.h>

using namespace firey;

/*const char* versionToString() const
{
	const char* res="UNKNOWN";
	switch(version_)
	{
		case kHttp10:
			res="HTTP/1.0";
			break;
		case kHttp11:
			res="HTTP/1.1";
			break;
		case kHttp20:
			res="HTTP/2.0";
			break;
		default:
			break;
	}
	return res;
}*/

void HttpResponseff::appendToBuffer(Bufferff* output) const
{
	char buf[32]={0};
	snprintf(buf,sizeof buf,"HTTP/1.1 %d ",statusCode_);
	output->append(buf);
	output->append(statusMessage_);
	output->append("\r\n");

	if(closeConnection_)
	{
		output->append("Connection: close\r\n");
	}
	else
	{
		snprintf(buf,sizeof buf,"Content-Length: %zd\r\n",body_size());
		output->append(buf);
		output->append("Connection: Keep-Alive\r\n");
	}

	for(auto& header:headers_)
	{
		output->append(header.first);
		output->append(": ");
		output->append(header.second);
		output->append("\r\n");
	}

	output->append("\r\n");
	output->append(body_);
}
