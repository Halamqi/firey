#ifndef FF_CALLBACKS_H_
#define FF_CALLBACKS_H_

#include <functional>
#include <memory>

#include "Timestampff.h"

namespace firey{

using std::placeholders::_1;
using std::placeholders::_2;
using std::placeholders::_3;

class TcpConnectionff;
class Bufferff;

typedef std::shared_ptr<TcpConnectionff> TcpConnectionPtr;
typedef std::function<void(const TcpConnectionPtr&)> ConnectionCallback;
typedef std::function<void(const TcpConnectionPtr&)> CloseCallback;
typedef std::function<void(const TcpConnectionPtr&)> WriteCompleteCallback;
typedef std::function<void(const TcpConnectionPtr&)> HighWaterCallback;

typedef std::function<void(const TcpConnectionPtr&,Bufferff*,Timestampff)> MessageCallback;

void defaultConnectionCallback(const TcpConnectionPtr& conn);
void defaultMessageCallback(const TcpConnectionPtr& conn,Bufferff* buffer,Timestampff receiveTime);
}//namespace firey

#endif //FF_CALLBACKS_H_
