cmake_minimum_required (VERSION 3.16)

if(NOT CORBA MATCHES "RtORB")
	add_subdirectory(logical_time)
endif()

# RT_PREEMPT EC only supported on Linux
if (CMKAE_SYSTEM_NAME MATCHES "Linux")
	add_subdirectory(rtpreempt)
endif()
