add_executable(test_acceptor 
		test_acceptor.cpp)
target_link_libraries(test_acceptor firey_net)

add_executable(test_conn 
		test_conn.cpp)
target_link_libraries(test_conn firey_net)

add_executable(test_curThr 
		test_curThr.cpp)
target_link_libraries(test_curThr firey_base)

add_executable(test_eventloopthreadpool 
		test_eventloopthreadpool.cpp )
target_link_libraries(test_eventloopthreadpool firey_net)

add_executable(test_log 
		test_log.cpp)
target_link_libraries(test_log firey_base)

add_executable(test_thread 
		test_thread.cpp)
target_link_libraries(test_thread firey_base)

add_executable(test_time 
		test_time.cpp)
target_link_libraries(test_time firey_base)

add_executable(test_timers 
		test_timers.cpp)
target_link_libraries(test_timers firey_net)

add_executable(test_timestamp
		test_timestamp.cpp)
target_link_libraries(test_timestamp firey_net)

add_executable(test_tpool
		test_tpool.cpp)
target_link_libraries(test_tpool firey_base)
