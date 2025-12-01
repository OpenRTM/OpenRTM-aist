cmake_minimum_required (VERSION 3.16)

set(ROS_ENABLE OFF CACHE BOOL "set ROS_ENABLE")

if(ROS_ENABLE)
	add_subdirectory(ROSTransport)
endif()


set(FASTRTPS_ENABLE OFF CACHE BOOL "set FASTRTPS_ENABLE")
set(ROS2_ENABLE OFF CACHE BOOL "set ROS2_ENABLE")

if(FASTRTPS_ENABLE)
	add_subdirectory(FastRTPS)
	if(ROS2_ENABLE)
		add_subdirectory(ROS2Transport)
	endif(ROS2_ENABLE)
endif()

set(OPENSPLICE_ENABLE OFF CACHE BOOL "set OPENSPLICE_ENABLE")

if(OPENSPLICE_ENABLE)
	add_subdirectory(OpenSplice)
endif()


if(UNIX)
	set(SSM_ENABLE OFF CACHE BOOL "set SSM_ENABLE")

	if(SSM_ENABLE)
		add_subdirectory(SSMTransport)
	endif()
endif()
