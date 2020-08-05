set(net_SRCS
	Acceptorff.cpp
	Bufferff.cpp
	Channelff.cpp
	EventLoopff.cpp
	EventLoopThreadff.cpp
	EventLoopThreadPoolff.cpp
	InetAddressff.cpp
	Pollerff.cpp
	Socketff.cpp
	Socketopsff.cpp
	TcpConnectionff.cpp
	TcpServerff.cpp
	)

add_library(firey_net ${net_SRCS})
target_link_libraries(firey_net firey_base)

install(TARGETS firey_net DESTINATION lib)

file(GLOB HEADERS "*.h")
install(FILES ${HEADERS} DESTINATION include)

