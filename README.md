项目的文件树：
./firey
├── base
│   ├── CMakeLists.txt
│   ├── logger
│   │   ├── AsyncLoggingff.cpp
│   │   ├── AsyncLoggingff.h
│   │   ├── FileUtilff.cpp
│   │   ├── FileUtilff.h
│   │   ├── LogFileff.cpp
│   │   ├── LogFileff.h
│   │   ├── Loggingff.cpp
│   │   ├── Loggingff.h
│   │   ├── LogStreamff.cpp
│   │   ├── LogStreamff.h
│   │   ├── Timestampff.cpp
│   │   └── Timestampff.h
│   └── thread
│       ├── Conditionff.h
│       ├── CountDownLatchff.cpp
│       ├── CountDownLatchff.h
│       ├── CurrentThreadff.cpp
│       ├── CurrentThreadff.h
│       ├── Mutexff.cpp
│       ├── Mutexff.h
│       ├── MutexGuardff.h
│       ├── Threadff.cpp
│       ├── Threadff.h
│       ├── ThreadPoolff.cpp
│       └── ThreadPoolff.h
├── build.sh
├── CMakeLists.txt
├── net
│   ├── Acceptorff.cpp
│   ├── Acceptorff.h
│   ├── Bufferff.cpp
│   ├── Bufferff.h
│   ├── Callbacksff.h
│   ├── Channelff.cpp
│   ├── Channelff.h
│   ├── CMakeLists.txt
│   ├── EventLoopff.cpp
│   ├── EventLoopff.h
│   ├── EventLoopThreadff.cpp
│   ├── EventLoopThreadff.h
│   ├── EventLoopThreadPoolff.cpp
│   ├── EventLoopThreadPoolff.h
│   ├── InetAddressff.cpp
│   ├── InetAddressff.h
│   ├── Pollerff.cpp
│   ├── Pollerff.h
│   ├── Socketff.cpp
│   ├── Socketff.h
│   ├── Socketopsff.cpp
│   ├── Socketopsff.h
│   ├── TcpConnectionff.cpp
│   ├── TcpConnectionff.h
│   ├── TcpServerff.cpp
│   ├── TcpServerff.h
│   ├── Timerff.cpp
│   ├── Timerff.h
│   ├── TimerIdff.h
│   ├── TimerQueueff.cpp
│   └── TimerQueueff.h
├── README.md
└── test_demos
    ├── test_acceptor.cpp
    ├── test_conn.cpp
    ├── test_curThr.cpp
    ├── test_eventLoop.cpp
    ├── test_evlp.cpp
    ├── test_log.cpp
    ├── test_thread.cpp
    ├── test_time.cpp
    ├── test_timers.cpp
    ├── test_timestamp.cpp
    └── test_tpool.cpp
