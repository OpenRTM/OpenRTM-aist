cmake_minimum_required(VERSION 3.16)

if(OpenRTM_aist_BINARY_DIR)
  set(srcs src/ConsoleOut.cpp include/ConsoleOut/ConsoleOut.h)
  examples_build(ConsoleOut SRCS "${srcs}" MAIN src/ConsoleOutComp.cpp
                 EXTERNAL_INCLUDE PRIVATE include/ConsoleOut)
  install(FILES consout.conf DESTINATION ${INSTALL_RTM_EXAMPLE_DIR} COMPONENT examples)
else(OpenRTM_aist_BINARY_DIR)

project(ConsoleOut)
string(TOLOWER ${PROJECT_NAME} PROJECT_NAME_LOWER)
include("${PROJECT_SOURCE_DIR}/cmake/utils.cmake")
set(PROJECT_VERSION 2.0.0 CACHE STRING "ConsoleOut version")
set(UPGRADE_GUID "")
DISSECT_VERSION()
set(PROJECT_SHORT_VER ${PROJECT_VERSION_MAJOR}${PROJECT_VERSION_MINOR}${PROJECT_VERSION_REVISION})
set(PROJECT_DESCRIPTION "Console output component")
set(PROJECT_VENDOR "AIST")
set(PROJECT_MAINTAINER "Noriaki Ando <n-ando@aist.go.jp>")
set(PROJECT_TYPE "c++/example")

find_package(OpenRTM REQUIRED)
set(RTM_VER ${OPENRTM_VERSION})
set(RTM_SHORT_VER ${OPENRTM_VERSION_MAJOR}${OPENRTM_VERSION_MINOR}${OPENRTM_VERSION_PATCH})

function(get_dist ARG0)
 if(NOT ${CMAKE_SYSTEM_NAME} MATCHES "Linux")
   set(${ARG0} ${CMAKE_SYSTEM_NAME} PARENT_SCOPE)
   return()
 endif()
 foreach(dist Debian Ubuntu RedHat Fedora CentOS Raspbian)
   execute_process(
     COMMAND grep ${dist} -s /etc/issue /etc/os-release /etc/redhat-release /etc/system-release
     OUTPUT_VARIABLE dist_name
     )
   if(${dist_name} MATCHES ${dist})
     set(${ARG0} ${dist} PARENT_SCOPE)
     return()
   endif()
 endforeach()
endfunction(get_dist)

function(get_pkgmgr ARG0)
 get_dist(DIST_NAME)
 if(${DIST_NAME} MATCHES "Debian" OR
     ${DIST_NAME} MATCHES "Ubuntu" OR
     ${DIST_NAME} MATCHES "Raspbian")
    set(${ARG0} "DEB" PARENT_SCOPE)
    return()
 endif()
 if(${DIST_NAME} MATCHES "RedHat" OR
    ${DIST_NAME} MATCHES "Fedora" OR
    ${DIST_NAME} MATCHES "CentOS")
    set(${ARG0} "RPM" PARENT_SCOPE)
    return()
 endif()
endfunction(get_pkgmgr)

get_dist(DIST_NAME)
MESSAGE(STATUS "Distribution is ${DIST_NAME}")

get_pkgmgr(PKGMGR)
if(PKGMGR AND NOT LINUX_PACKAGE_GENERATOR)
 set(LINUX_PACKAGE_GENERATOR ${PKGMGR})
 if(${PKGMGR} MATCHES "DEB")
   execute_process(COMMAND dpkg --print-architecture
     OUTPUT_VARIABLE CPACK_DEBIAN_PACKAGE_ARCHITECTURE
     OUTPUT_STRIP_TRAILING_WHITESPACE)
   message(STATUS "Package manager is ${PKGMGR}. Arch is ${CPACK_DEBIAN_PACKAGE_ARCHITECTURE}.")
 endif()
 if(${PKGMGR} MATCHES "RPM")
   execute_process(COMMAND uname "-m"
     OUTPUT_VARIABLE CPACK_RPM_PACKAGE_ARCHITECTURE
     OUTPUT_STRIP_TRAILING_WHITESPACE)
   message(STATUS "Package manager is ${PKGMGR}. Arch is ${CPACK_RPM_PACKAGE_ARCHITECTURE}.")
 endif()
endif()

# Add an "uninstall" target
CONFIGURE_FILE ("${PROJECT_SOURCE_DIR}/cmake/uninstall_target.cmake.in"
    "${PROJECT_BINARY_DIR}/uninstall_target.cmake" IMMEDIATE @ONLY)
ADD_CUSTOM_TARGET (${PROJECT_NAME}_uninstall "${CMAKE_COMMAND}" -P
    "${PROJECT_BINARY_DIR}/uninstall_target.cmake")

#option(BUILD_EXAMPLES "Build and install examples" OFF)
option(BUILD_DOCUMENTATION "Build the documentation" OFF)
option(BUILD_TESTS "Build the tests" OFF)
#option(BUILD_TOOLS "Build the tools" OFF)
option(BUILD_IDL "Build and install idl" ON)
option(BUILD_SOURCES "Build and install sources" OFF)

option(STATIC_LIBS "Build static libraries" OFF)
if(STATIC_LIBS)
    set(LIB_TYPE STATIC)
else(STATIC_LIBS)
    set(LIB_TYPE SHARED)
endif(STATIC_LIBS)

if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
   # Mac OS X specific code
   SET(CMAKE_CXX_COMPILER "g++")
   SET(CMAKE_MACOSX_RPATH 1)
endif (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")


# Set up installation directories
if(WIN32)
   set(RTM_COMPONENT_PREFIX "Components/${PROJECT_TYPE}")
   set(OPENRTM_SHARE_PREFIX "OpenRTM-aist/${RTM_VER}/${RTM_COMPONENT_PREFIX}")
   set(INSTALL_PREFIX ${PROJECT_NAME})
   if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
      set(CMAKE_INSTALL_PREFIX "${OPENRTM_DIR}/${RTM_COMPONENT_PREFIX}" CACHE PATH "..." FORCE)
   else()
      set(CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}/${OPENRTM_SHARE_PREFIX}")
   endif()
else(WIN32)
   set(OPENRTM_SHARE_PREFIX "share/openrtm-${OPENRTM_VERSION_MAJOR}.${OPENRTM_VERSION_MINOR}")
   set(INSTALL_PREFIX "${OPENRTM_SHARE_PREFIX}/components/${PROJECT_TYPE}/${PROJECT_NAME}")
endif(WIN32)

# Subdirectories
add_subdirectory(cmake)
if(BUILD_DOCUMENTATION)
    add_subdirectory(doc)
endif(BUILD_DOCUMENTATION)

#if(BUILD_EXAMPLES)
#    add_subdirectory(examples)
#endif(BUILD_EXAMPLES)

if(BUILD_IDL)
    add_subdirectory(idl)
endif(BUILD_IDL)

file(GLOB IDL_FILES "${CMAKE_CURRENT_SOURCE_DIR}/idl/*.idl")
if(IDL_FILES)
    install(FILES ${IDL_FILES} DESTINATION ${INSTALL_PREFIX}/idl
        COMPONENT component)
endif(IDL_FILES)

add_subdirectory(include)
MAP_ADD_STR(headers  "include/" comp_hdrs)
add_subdirectory(src)

if(BUILD_TESTS)
    # Universal settings
    enable_testing()
    add_subdirectory(test)
endif(BUILD_TESTS)

#if(BUILD_TOOLS)
#    add_subdirectory(tools)
#endif(BUILD_TOOLS)

if(BUILD_SOURCES)
    add_subdirectory(include)
    add_subdirectory(src)
endif(BUILD_SOURCES)

# Package creation
# By default, do not include the system runtime library.
#include(InstallRequiredSystemLibraries)
set(PROJECT_EXECUTABLES ${PROJECT_NAME}Comp
    "${PROJECT_NAME}Comp.exe")

set(cpack_options "${PROJECT_BINARY_DIR}/cpack_options.cmake")

configure_file("${PROJECT_SOURCE_DIR}/cmake/cpack_options.cmake.in"
    ${cpack_options} @ONLY)

set(CPACK_PROJECT_CONFIG_FILE ${cpack_options})
include(${CPACK_PROJECT_CONFIG_FILE})
include(CPack)

endif(OpenRTM_aist_BINARY_DIR)
