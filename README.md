- firey项目背景
  - 该项目主要是个人的一些学习，总结。在编写网络编程时，如果频繁的调用Sockets API将是一件非常繁琐的事情。
  - 于是，对网络编程进行了一个总结，整理出网络编程中整体的框架，并参考了其它的一些开源网络框架，借鉴了其中的很多思想,编写出的网络库。

- 关键技术
  - 基于C+11，采用one loop per thread + Reactor的服务器编程模型，主线程负责监听连接，接收连接之后分发至io线程池中交由io线程处理。
  - Reactor采用epoll来实现事件分发，并配合非阻塞IO来进行数据读写。
  - 使用STL的set容器和Linux的timerfd实现了一个简单的定时器。
  - 使用Linux下的eventfd实现了线程间的消息队列，来实现线程间任务的调度，满足线程安全。
  - 采用双缓冲技术，配合C++11的移动语义实现了一个简单且高效的异步日志系统。

- 安装
  - 将源码下载至用户家目录下
  - 下载源码后，在源码目录下`~/firey`下执行`./build.sh`即可安装成功
  - 安装后的库文件和头文件存放在目录`~/firey-build/debug-install`中

- 使用
  在使用时，除了包含在`~/firey-build/debug-install/include`中的头文件之外。在编译时，添加链接库路径`~/firey-build/debug-install/lib`，并链接库`-lfirey_net`即可。
