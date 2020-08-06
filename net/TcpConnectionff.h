#ifndef FF_TCPCONNECTION_H_
#define FF_TCPCONNECTION_H_

#include "Callbacksff.h"
#include "InetAddressff.h"
#include "Bufferff.h"

#include <string>

struct tcp_info;

namespace firey{

class Channelff;
class EventLoopff;
class Socketff;

//derived from enable_shared_from_this for managing TcpConnection's lifetime
class TcpConnectionff:public std::enable_shared_from_this<TcpConnectionff>
{
	public:
		TcpConnectionff(EventLoopff* loop,
				const std::string& name,
				int sockfd,
				const InetAddressff& localAddr,
				const InetAddressff& peerAddr);
		~TcpConnectionff();
		TcpConnectionff(const TcpConnectionff&)=delete;
		TcpConnectionff& operator=(const TcpConnectionff&)=delete;

		EventLoopff* getLoop() const{return ownerLoop_;}
		const std::string name() const {return name_;}
		const InetAddressff& localAddr() const {return localAddr_;}
		const InetAddressff& peerAddr() const{return peerAddr_;}

		bool connected() const {return state_==kConnected;}
		bool disconnected() const {return state_==kDisconnected;}

		void send(const std::string& message);
		void send(const void* message,size_t len);
		void send(Bufferff* buffer);
			
		void shutdown();
		void forceClose();
		void forceCloseWithDelay(double);

		void startRead();
		void stopRead();
		bool isReading() const {return reading_;}

		bool getTcpInfo(struct tcp_info* tcpi) const;
		std::string getTcpInfoString() const;

		//don't use move ,beacuse other connection need tcpserver's callback
		void setConnectionCallback(const ConnectionCallback& cb){
			connectionCallback_=cb;
		}

		void setMessageCallback(const MessageCallback& cb){
			messageCallback_=cb;
		}

		void setWriteCompleteCallback(const WriteCompleteCallback& cb){
			writeCompleteCallback_=cb;
		}
		
		void setHighWaterCallback(const HighWaterCallback& cb,size_t highWaterMark){
			highWaterCallback_=cb;
			highWaterMark_=highWaterMark;
		}

		void setCloseCallback(const CloseCallback& cb){
			closeCallback_=cb;
		}

		void connectionEstablished();
		void connectionDestroy();

	private:

		enum ConnState{
			kDisconnected,
			kConnecting,
			kConnected,
			kDisconnecting
		};
		
		EventLoopff* ownerLoop_;
		const std::string name_;
		const InetAddressff localAddr_;
		const InetAddressff peerAddr_;

		ConnState state_;
		bool reading_;

		std::unique_ptr<Channelff> connChannel_;
		std::unique_ptr<Socketff> connSocket_;

		ConnectionCallback connectionCallback_;
		MessageCallback messageCallback_;

		WriteCompleteCallback writeCompleteCallback_;

		HighWaterCallback highWaterCallback_;
		size_t highWaterMark_;

		CloseCallback closeCallback_;

		void shutdownInLoop();
		void forceCloseInLoop();

		void handleRead(Timestampff reveiveTime);
		void handleWrite();
		void handleError();
		void handleClose();

//		void sendInLoop(const std::string& message);
		void sendInLoop(const void* message,size_t len);
		
		void startReadInLoop();
		void stopReadInLoop();

		void setState(ConnState s){state_=s;}
		const char* stateToString() const;

		Bufferff inputBuffer_;
		Bufferff outputBuffer_;

};//class TcpConnectionff

}//namespace firey

#endif//FF_TCPCONNECTION_H_
