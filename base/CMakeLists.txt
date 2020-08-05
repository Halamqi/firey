set(base_SRCS
	logger/AsyncLoggingff.cpp
	logger/FileUtilff.cpp
	logger/LogFileff.cpp
	logger/Loggingff.cpp
	logger/LogStreamff.cpp
	thread/CountDownLatchff.cpp
	thread/CurrentThreadff.cpp
	thread/Mutexff.cpp
	thread/Threadff.cpp
	thread/ThreadPoolff.cpp
		)

add_library(firey_base ${base_SRCS})
target_link_libraries(firey_base rt lpthread)

install(TARGETS firey_base DESTINATION lib)

set(HEADERS
	logger/AsyncLoggingff.h
	logger/FileUtilff.h
	logger/LogFileff.h
	logger/Loggingff.h
	logger/LogStreamff.h
	logger/Timestampff.h
	thread/Conditionff.h
	thread/CountDownLatchff.h
	thread/CurrentThreadff.h 
	thread/Mutexff.h
	thread/MutexGuardff.h
	thread/Threadff.h
	thread/ThreadPoolff.h
		)

install(FILES ${HEADERS} DESTINATION include)
