#ifndef FF_HTTP_REQUEST_H_
#define FF_HTTP_REQUEST_H_

#include "Timestampff.h"

#include <map>
#include <string>
#include <assert.h>

namespace firey{

class HttpRequestff{
	public:
		enum Method
		{
			kInvalid,
			kGet,
			kPost,
			kHead,
			kPut,
			kDelete
		};

		enum Version
		{
			kUnknown,
			kHttp10,
			kHttp11,
			kHttp20
		};

		HttpRequestff()
			:method_(kInvalid),
			version_(kUnknown)
		{
		}

		void setVersion(Version v)
		{
			version_(v);
		}

		Version getVersion() const
		{return version_;}

		bool setMethod(const char* start,const char* end)
		{
			assert(method_==kInvalid);
			std::string method(start,end);
			if(method=="GET")
				method_=kGet;
			else if(method=="POST")
				method_=kPost;
			else if(method=="HEAD")
				method_=kHead;
			else if(method=="PUT")
				method_=kPut;
			else if(method=="DELETE")
				method_=kDelete;
			else method_=kInvalid;
		}

		Method getMethod()
		{return method_;}

		const char* methodToString() const
		{
			const char* res="UNKONWN";
			switch (method_)
			{
				case kGet:
					res="GET";
					break;
				case kPost:
					res="POST";
					break;
				case kHead:
					res="HEAD";
					break;
				case kPut:
					res="PUT";
					break;
				case kDelete:
					res="DELETE";
					break;
				default:
					break;
			}
			return res;
		}

		void setPath(const char* start,const char* end)
		{
			path_.assign(start,end);
		}

		const std::string& path() const
		{
			return path_;
		}

		void setQuery(const char* start,const char* end)
		{
			query_.assign(start,end);
		}

		const std::string& query() const
		{return query_;}

		void setReceiveTime(Timestampff time)
		{receiveTime_=time;}

		Timestampff receiveTime() const
		{return receiveTime_;}

		void addHeader(const char* start,const char* colon,const char* end)
		{
			std::string field(start,colon);
			++colon;
			while(colon<end&&isspace(*colon))
			{
				++colon;
			}

			string value(colon,end);
			while(!value.empty()&&isspace(value[value.size()-1]))
			{
				value.resize(value.size()-1);
			}
			headers_[field]=value;
		}

		std::string getHeader(const std::string& field) const
		{
			std::string res;
			auto it=headers_.find(field);
			if(it!=headers_.end())
			{
				res=it->second;
			}
			return res;
		}

		const std::map<std::string,std::string>& headers() const
		{return headers_;}

		void swap(HttpRequest& that)
		{
			std::swap(method_,that.method_);
			std::swap(version_,that.version_);
			path_.swap(that.path_);
			query_.swap(that.query_);
			receiveTime_.swap(that.receiveTime_);
			headers_.swap(that.headers_);
		}

	private:
		//http请求的方法
		Method method_;
		//http请求的版本号
		Version version_;
		//http中url表示的资源的路径
		std::string path_;
		//url中的数据
		std::string query_;
		//http报文接收时间
		Timestampff receiveTime_;
		//头部字段
		std::map<std::string,std::string> headers_;

	};//class HttpRequestff
}//namespace firey
#endif //FF_HTTP_REQUEST_H_
