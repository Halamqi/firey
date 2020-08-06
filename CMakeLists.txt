cmake_minimum_required(VERSION 2.6)


project(firey C CXX)

enable_testing()

if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE "Debug")
endif()

# only build examples if this is the main project
#只有这个是主项目时，才编译例子
if(CMAKE_PROJECT_NAME STREQUAL "firey")
	option(FIREY_BUILD_EXAMPLES "Build firey examples" ON)
endif()

set(CXX_FLAGS
	-g
#   -DVALGRIND
	-DCHECK_PTHREAD_RETURN_VALUE
	-D_FILE_OFFSET_BITS=64
	-Wall
	-Wextra
	-Werror
	-Wconversion
	-Wno-unused-parameter
	-Wold-style-cast
	-Woverloaded-virtual
	-Wpointer-arith
	-Wshadow
	-Wwrite-strings
	-march=native
# 	-MMD
	-std=c++11
	-rdynamic
	)

if(CMAKE_BUILD_BITS EQUAL 32)
	list(APPEND CXX_FLAGS "-m32")
endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
	list(APPEND CXX_FLAGS "-Wno-null-dereference")
	list(APPEND CXX_FLAGS "-Wno-sign-conversion")
	list(APPEND CXX_FLAGS "-Wno-unused-local-typedef")
	list(APPEND CXX_FLAGS "-Wthread-safety")
	list(REMOVE_ITEM CXX_FLAGS "-rdynamic")
endif()

string(REPLACE ";" " " CMAKE_CXX_FLAGS "${CXX_FLAGS}")

set(CMAKE_CXX_FLAGS_DEBUG "-O0")
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG")
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

include_directories(${PROJECT_SOURCE_DIR}/base/logger ${PROJECT_SOURCE_DIR}/base/thread ${PROJECT_SOURCE_DIR}/net)

string(TOUPPER ${CMAKE_BUILD_TYPE} BUILD_TYPE)
message(STATUS "CXX_FLAGS = " ${CMAKE_CXX_FLAGS} " " ${CMAKE_CXX_FLAGS_${BUILD_TYPE}})

#当有FIREY_BUILD_EXAMPLES时，才编译例子
if(FIREY_BUILD_EXAMPLES)
	add_subdirectory(./test_demos)
endif()

add_subdirectory(./base)
add_subdirectory(./net)

