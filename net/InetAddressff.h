#ifndef FF_INETADDRESS_H_
#define FF_INETADDRESS_H_

#include <string>
#include <netinet/in.h>

namespace firey{

class InetAddressff{
	private:
		struct sockaddr_in addr_;

	public:

		explicit InetAddressff(const struct sockaddr_in& addr)
			:addr_(addr){}

		explicit InetAddressff(uint16_t port);

		InetAddressff(const std::string& ip,uint16_t port);


	public:
		std::string toIpPort() const;
		std::string toIp() const;
		uint16_t toPort() const;

		const struct sockaddr_in* getSockAddr() const {return &addr_;}
		void setSockAddr(const struct sockaddr_in& addr){addr_=addr;}

		uint16_t portNetEndian() const{return addr_.sin_port;}
		uint32_t ipNetEndian() const;
};

}//namespace firey

#endif //FF_INETADDRESS_H_
