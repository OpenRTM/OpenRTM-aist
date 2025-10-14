cmake_minimum_required (VERSION 3.16)
project (${COIL_PROJECT_NAME}
	VERSION ${RTM_VERSION}
	LANGUAGES CXX)

if(WIN32)
	set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()

set(COIL_OS_DARWIN OFF)
set(COIL_OS_FREEBSD OFF)
set(COIL_OS_LINUX OFF)
set(COIL_OS_QNX OFF)
set(COIL_OS_VXWORKS OFF)
set(COIL_OS_WIN32 OFF)

if (UNIX)
	if     (CMAKE_SYSTEM_NAME MATCHES "Darwin")
		set (COIL_OS_DARWIN ON)
	elseif (CMAKE_SYSTEM_NAME MATCHES "FreeBSD")
		set (COIL_OS_FREEBSD ON)
	elseif (CMAKE_SYSTEM_NAME MATCHES "Linux")
		set (COIL_OS_LINUX ON)
	elseif (CMAKE_SYSTEM_NAME MATCHES "QNX") 
		set (COIL_OS_QNX ON)
	else ()
		set (COIL_OS_LINUX ON)
	endif ()
elseif (WIN32)
	set (COIL_OS_WIN32 ON)
elseif (VXWORKS)
	set (COIL_OS_VXWORKS ON)
endif ()

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config_coil_cmake.h.in ${PROJECT_BINARY_DIR}/config_coil.h)

set(coil_headers
	common/coil/Async.h
	common/coil/ClockManager.h
	common/coil/Factory.h
	common/coil/Logger.h
	common/coil/PeriodicTask.h
	common/coil/PeriodicTaskBase.h
	common/coil/Properties.h
	common/coil/Singleton.h
	common/coil/Task.h
	common/coil/TimeMeasure.h
	common/coil/Timer.h
	common/coil/crc.h
	common/coil/stringutil.h
	${COIL_OS_DIR}/coil/DynamicLib.h
	${COIL_OS_DIR}/coil/OS.h
	${COIL_OS_DIR}/coil/Routing.h
	${COIL_OS_DIR}/coil/Process.h
	${COIL_OS_DIR}/coil/Signal.h
	${COIL_OS_DIR}/coil/UUID.h
	${COIL_OS_DIR}/coil/SharedMemory.h
	${COIL_OS_DIR}/coil/Affinity.h
	${PROJECT_BINARY_DIR}/config_coil.h
)

set(coil_srcs
	common/coil/Async.cpp
	common/coil/ClockManager.cpp
	common/coil/PeriodicTask.cpp
	common/coil/Properties.cpp
	common/coil/Task.cpp
	common/coil/TimeMeasure.cpp
	common/coil/Timer.cpp
	common/coil/crc.cpp
	common/coil/stringutil.cpp
	common/coil/Logger.cpp
	${COIL_OS_DIR}/coil/DynamicLib.cpp
	${COIL_OS_DIR}/coil/Routing.cpp
	${COIL_OS_DIR}/coil/Process.cpp
	${COIL_OS_DIR}/coil/Signal.cpp
	${COIL_OS_DIR}/coil/UUID.cpp
	${COIL_OS_DIR}/coil/SharedMemory.cpp
	${COIL_OS_DIR}/coil/Affinity.cpp
	${COIL_OS_DIR}/coil/OS.cpp
	${COIL_OS_DIR}/coil/File.cpp
	${coil_headers}
)

if(UNIX)
	if(${CMAKE_SYSTEM_NAME}test MATCHES QNXtest)
		if(QNX7)
		else()
		include_directories(SYSTEM /usr/local/include)
		link_directories(/usr/qnx650/target/qnx6/x86/lib/gcc/4.4.2/ /usr/qnx650/target/qnx6/x86/lib/ /usr/local/lib)
		endif()
	endif()

	include(FindPkgConfig)
	pkg_check_modules(UUID REQUIRED uuid)
	add_definitions(${UUID_CFLAGS})
endif(UNIX)

add_library(${PROJECT_NAME}_objlib OBJECT ${coil_srcs})
openrtm_common_set_compile_props(${PROJECT_NAME}_objlib)
target_compile_definitions(${PROJECT_NAME}_objlib PRIVATE
	${COIL_C_FLAGS_LIST}
	$<$<PLATFORM_ID:Windows>:LIBRARY_EXPORTS>)
target_include_directories(${PROJECT_NAME}_objlib
	PUBLIC ${PROJECT_SOURCE_DIR}/common
	PUBLIC ${PROJECT_SOURCE_DIR}/${COIL_OS_DIR})
target_include_directories(${PROJECT_NAME}_objlib SYSTEM
	PUBLIC ${CMAKE_BINARY_DIR}/src/lib
	PRIVATE ${UUID_INCLUDE_DIRS})

install(FILES  ${coil_headers} DESTINATION ${INSTALL_COIL_INCLUDE_DIR}/coil COMPONENT headers)
