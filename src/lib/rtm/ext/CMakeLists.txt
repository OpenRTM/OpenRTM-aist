cmake_minimum_required (VERSION 3.16)

if(NOT CORBA MATCHES "RtORB")
	add_subdirectory(rtmCamera)
	add_subdirectory(rtmManipulator)
endif()
