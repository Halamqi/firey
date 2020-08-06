#include "Socketff.h"
#include "InetAddressff.h"
#include "Loggingff.h"

#include <netinet/tcp.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <string.h>

using namespace firey;

bool Socketff::getTcpInfo(struct tcp_info* tcpi) const
{
	socklen_t len=sizeof(*tcpi);
	memset(tcpi,0,len);
	return ::getsockopt(sockfd_,SOL_TCP,TCP_INFO,tcpi,&len)==0;
}

bool Socketff::getTcpInfoString(char* buf,int len) const
{
	struct tcp_info tcpi;
	bool ok=getTcpInfo(&tcpi);
	if(ok)
	{
		snprintf(buf, len, "unrecovered=%u "
				"rto=%u ato=%u snd_mss=%u rcv_mss=%u "
				"lost=%u retrans=%u rtt=%u rttvar=%u "
				"sshthresh=%u cwnd=%u total_retrans=%u",
				tcpi.tcpi_retransmits,  // Number of unrecovered [RTO] timeouts
				tcpi.tcpi_rto,          // Retransmit timeout in usec
				tcpi.tcpi_ato,          // Predicted tick of soft clock in usec
				tcpi.tcpi_snd_mss,
				tcpi.tcpi_rcv_mss,
				tcpi.tcpi_lost,         // Lost packets
				tcpi.tcpi_retrans,      // Retransmitted packets out
				tcpi.tcpi_rtt,          // Smoothed round trip time in usec
				tcpi.tcpi_rttvar,       // Medium deviation
				tcpi.tcpi_snd_ssthresh,
				tcpi.tcpi_snd_cwnd,
				tcpi.tcpi_total_retrans);  // Total retransmits for entire connection
	}
	return ok;
}

Socketff::~Socketff(){
	::close(sockfd_);
}

void Socketff::bindAddress(const InetAddressff& localAddr){
	Socket::bindOrDie(sockfd_,localAddr.getSockAddr());
}

void Socketff::listen(){
	Socket::listenOrDie(sockfd_);
}

int Socketff::accept(InetAddressff& peerAddr){
	struct sockaddr_in addr;
	memset(&addr,0,sizeof addr);
	int connfd=Socket::accept(sockfd_,&addr);
	//设置客户端的地址
	if(connfd>=0){
		peerAddr.setSockAddr(addr);
	}
	return connfd;
}

void Socketff::setTcpNoDelay(bool on){
#ifdef TCP_NODELAY
	int optval=on?1:0;
	int ret=::setsockopt(sockfd_,IPPROTO_TCP,TCP_NODELAY,&optval,static_cast<socklen_t>(sizeof optval));
	if(ret<0&&on)
	{
		LOG_SYSERR<<"TCP_NODELAY failed.";
	}
#else
	if(on)
	{
		LOG_ERROR<<"TCP_NODELAY is not supported.";
	}
#endif
}

void Socketff::setTcpReuseAddr(bool on){
#ifdef SO_REUSEADDR
	int optval=on?1:0;
	int ret=::setsockopt(sockfd_,SOL_SOCKET,SO_REUSEADDR,&optval,static_cast<socklen_t>(sizeof optval));
	if(ret<0&&on)
	{
		LOG_SYSERR<<"SO_REUSEADDR failed.";
	}
#else
	if(on)
	{
		LOG_ERROR<<"SO_REUSEADDR is not supported.";
	}
#endif
}

void Socketff::setTcpReusePort(bool on){
#ifdef SO_REUSEPORT
	int optval = on ?1:0;
	int ret=::setsockopt(sockfd_,SOL_SOCKET,SO_REUSEPORT,&optval,static_cast<socklen_t>(sizeof optval));
	if(ret<0&&on)
	{
		LOG_SYSERR<<"SO_REUSEPORT failed.";
	}
#else
	if(on)
	{
		LOG_ERROR<<"SO_REUSEPORT is not supported.";
	}
#endif
}

void Socketff::setTcpKeepAlive(bool on){
#ifdef SO_KEEPALIVE
	int optval=on?1:0;
	int ret=::setsockopt(sockfd_,SOL_SOCKET,SO_KEEPALIVE,&optval,static_cast<socklen_t>(sizeof optval));
	if(ret<0&&on)
	{
		LOG_SYSERR<<"SO_KEEPALIVE failed.";
	}
#else
	if(on)
	{
		LOG_ERROR<<"SO_KEEPALIVE is not supported.";
	}
#endif
}

void Socketff::shutdown(){
	Socket::shutdownWrite(sockfd_);
}
