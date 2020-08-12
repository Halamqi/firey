set(http_SCRS
	HttpContextff.cpp
	HttpResponseff.cpp
	HttpServerff.cpp)

add_library(firey_http ${http_SCRS})
target_link_libraries(firey_http firey_net)

install(TARGETS firey_http DESTINATION lib)

set(HEADERS
	HttpContextff.h
	HttpServerff.h
	HttpRequestff.h
	HttpResponseff.h)
install(FILES ${HEADERS} DESTINATION include)

add_executable(httpserver_test httpserver_test.cpp)
target_link_libraries(httpserver_test firey_http)
