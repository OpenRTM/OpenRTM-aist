cmake_minimum_required (VERSION 3.5.1)

project (ROSTransport
	VERSION ${RTM_VERSION}
	LANGUAGES CXX)


if(NOT WIN32)
  set_directory_properties(PROPERTIES COMPILE_OPTIONS "-Wall;-Wextra")
endif()

if(UNIX)
	include(CheckCXXCompilerFlag)
	CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
	CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
	if(COMPILER_SUPPORTS_CXX11)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
	elseif(COMPILER_SUPPORTS_CXX0X)
	 	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
	else()
	 	message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
	endif()
endif()

if(WIN32)
	find_package(roscpp REQUIRED
	)
else()
	find_package(PkgConfig REQUIRED)
	pkg_check_modules(roscpp roscpp)
endif()

if(NOT roscpp_FOUND)
	message(FATAL_ERROR "can not find roscpp.")
endif()


find_package(Boost 1.69 COMPONENTS chrono filesystem system)
if(NOT Boost_FOUND)
	if(WIN32)
		find_package(Boost REQUIRED COMPONENTS chrono filesystem signals system thread)
	else()
		find_package(Boost REQUIRED COMPONENTS chrono filesystem signals system)
	endif()
endif()


link_directories(${ORB_LINK_DIR})
add_definitions(${ORB_C_FLAGS_LIST})
add_definitions(${COIL_C_FLAGS_LIST})
if(WIN32)
	add_definitions(-DRTM_SKEL_IMPORT_SYMBOL)
	add_definitions(-DNOGDI)
	add_definitions(-DTRANSPORT_PLUGIN)
	add_definitions(-D_RTLDLL)
	add_definitions(-DBOOST_ALL_DYN_LINK)

	configure_file(${CMAKE_CURRENT_SOURCE_DIR}/install_manifest.txt.in ${PROJECT_BINARY_DIR}/install_manifest.txt @ONLY)
endif()


set(target ROSTransport)

set(srcs ROSTransport.cpp ROSTransport.h ROSInPort.cpp ROSInPort.h ROSOutPort.cpp ROSOutPort.h ROSMessageInfo.cpp ROSMessageInfo.h ROSTopicManager.cpp ROSTopicManager.h ROSSerializer.cpp ROSSerializer.h SubscriberLink.cpp SubscriberLink.h PublisherLink.cpp PublisherLink.h)


if(VXWORKS AND NOT RTP)
	set(libs ${RTCSKEL_PROJECT_NAME})

	add_executable(${target} ${srcs})
	openrtm_common_set_compile_props(${target})
	openrtm_set_link_props_shared(${target})
	openrtm_include_rtm(${target})
	target_include_directories(${target} SYSTEM
	  PRIVATE${roscpp_INCLUDE_DIRS}
	  PRIVATE${Boost_INCLUDE_DIRS})
	target_link_libraries(${target} ${libs} ${roscpp_LINK_LIBRARIES} ${Boost_LIBRARIES})
	
	install(TARGETS ${target} LIBRARY DESTINATION ${INSTALL_RTM_EXT_DIR}/transport
				ARCHIVE DESTINATION ${INSTALL_RTM_EXT_DIR}/transport
				RUNTIME DESTINATION ${INSTALL_RTM_EXT_DIR}/transport
				COMPONENT ext)
else()
	set(libs ${RTM_PROJECT_NAME} ${ORB_LIBRARIES} ${DATATYPE_FACTORIES})

	add_library(${target} SHARED ${srcs})
	openrtm_common_set_compile_props(${target})
	openrtm_include_rtm(${target})
	openrtm_set_link_props_shared(${target})
	target_include_directories(${target} SYSTEM
		PRIVATE ${roscpp_INCLUDE_DIRS}
		PRIVATE ${Boost_INCLUDE_DIRS})
	if(WIN32)
		target_link_libraries(${target} PRIVATE ${libs} ${RTM_LINKER_OPTION} ${roscpp_LIBRARIES} ${Boost_LIBRARIES})
	else()
		target_link_libraries(${target} PRIVATE ${libs} ${RTM_LINKER_OPTION} ${roscpp_LINK_LIBRARIES} ${Boost_LIBRARIES})
	endif()
	set_target_properties(${target} PROPERTIES PREFIX "")


	install(TARGETS ${target} LIBRARY DESTINATION ${INSTALL_RTM_EXT_DIR}/transport
				ARCHIVE DESTINATION ${INSTALL_RTM_EXT_DIR}/transport
				RUNTIME DESTINATION ${INSTALL_RTM_EXT_DIR}/transport
				COMPONENT ext)
endif()


install(FILES ROSMessageInfo.h DESTINATION ${INSTALL_RTM_INCLUDE_DIR}/rtm/ext COMPONENT ext)



if(VXWORKS)
	if(RTP)
	else(RTP)	
		set_target_properties(${target} PROPERTIES SUFFIX ".out")
	endif(RTP)
endif(VXWORKS)


file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/devel/include/ros)

set(INSTALL_ROSTRANSPORT_DIR ${INSTALL_RTM_EXT_DIR}/transport)
configure_file(
	${CMAKE_CURRENT_SOURCE_DIR}/rtc.ros.conf.in ${PROJECT_BINARY_DIR}/rtc.ros.conf @ONLY)
install(
	FILES ${PROJECT_BINARY_DIR}/rtc.ros.conf
	DESTINATION ${INSTALL_RTM_ETC_DIR}/transport
	COMPONENT ext)