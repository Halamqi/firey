# Install script for directory: /home/cdlsxx/firey/base

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Debug")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "/home/cdlsxx/firey/lib/libfirey_base.a")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include" TYPE FILE FILES
    "/home/cdlsxx/firey/base/logger/AsyncLoggingff.h"
    "/home/cdlsxx/firey/base/logger/FileUtilff.h"
    "/home/cdlsxx/firey/base/logger/LogFileff.h"
    "/home/cdlsxx/firey/base/logger/Loggingff.h"
    "/home/cdlsxx/firey/base/logger/LogStreamff.h"
    "/home/cdlsxx/firey/base/logger/Timestampff.h"
    "/home/cdlsxx/firey/base/thread/Conditionff.h"
    "/home/cdlsxx/firey/base/thread/CountDownLatchff.h"
    "/home/cdlsxx/firey/base/thread/CurrentThreadff.h"
    "/home/cdlsxx/firey/base/thread/Mutexff.h"
    "/home/cdlsxx/firey/base/thread/MutexGuardff.h"
    "/home/cdlsxx/firey/base/thread/Threadff.h"
    "/home/cdlsxx/firey/base/thread/ThreadPoolff.h"
    )
endif()

