#ifndef FF_HTTP_RESPONSE_H_
#define FF_HTTP_RESPONSE_H_

#include <map>

namespace firey{

class Bufferff;

class HttpResponseff{
	public:
		enum HttpStatusCode
		{
			kUnknown,
			k200Ok=200,
			k301MovedPermanently=301,
			k400BadRequest=400,
			k404NotFound=404,
			k500InternalError=500
		};
		
		/*enum Version
		{
			kUnknown,
			kHttp10,
			kHttp11,
			kHttp20
		};*/

		explicit HttpResponseff(bool close)
			:statusCode_(kUnknown),
			closeConnection_(close)
		{
		}
		
		/*
		void setVersion(Version v)
		{version_=v;}
		
		Version getVersion() const
		{return version_;}

		const char* versionToString() const;
		*/

		void setStatusCode(HttpStatusCode status)
		{statusCode_=status;}

		void setStatusMessage(const std::string& message)
		{statusMessage_=message;}

		void setCloseConnection(bool on)
		{closeConnection_=on;}

		bool closeConnection() const
		{return closeConnection_;}

		void setContentType(const std::string& type)
		{
			addHeader("Content-Type",type);
		}

		void addHeader(const std::string& field,const std::string& value)
		{
			headers_[field]=value;
		}

		void setBody(const std::string& body)
		{body_=body;}

		void appendToBuffer(Bufferff* buffer) const;

	private:
		//Version version_;
		HttpStatusCode statusCode_;
		std::string statusMessage_;
		bool closeConnection_;
		std::map<std::string,std::string> headers_;
		std::string body_;

};//class HttpResponseff

}//namespace firey

#endif //FF_HTTP_RESPONSE_H_
