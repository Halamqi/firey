# Install script for directory: /home/cdlsxx/firey/net

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
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "/home/cdlsxx/firey/lib/libfirey_net.a")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include" TYPE FILE FILES
    "/home/cdlsxx/firey/net/Timerff.h"
    "/home/cdlsxx/firey/net/InetAddressff.h"
    "/home/cdlsxx/firey/net/TimerIdff.h"
    "/home/cdlsxx/firey/net/Channelff.h"
    "/home/cdlsxx/firey/net/TimerQueueff.h"
    "/home/cdlsxx/firey/net/Callbacksff.h"
    "/home/cdlsxx/firey/net/EventLoopThreadff.h"
    "/home/cdlsxx/firey/net/Socketff.h"
    "/home/cdlsxx/firey/net/Bufferff.h"
    "/home/cdlsxx/firey/net/Acceptorff.h"
    "/home/cdlsxx/firey/net/Pollerff.h"
    "/home/cdlsxx/firey/net/TcpServerff.h"
    "/home/cdlsxx/firey/net/TcpConnectionff.h"
    "/home/cdlsxx/firey/net/EventLoopff.h"
    "/home/cdlsxx/firey/net/Socketopsff.h"
    "/home/cdlsxx/firey/net/EventLoopThreadPoolff.h"
    )
endif()

