cmake_minimum_required (VERSION 3.5.1)

add_subdirectory(rtcd)
add_subdirectory(rtcprof)
add_subdirectory(cmake)
add_subdirectory(rtm-skelwrapper)
add_subdirectory(rtm-naming)
add_subdirectory(openrtmNames)

if(UNIX)
	add_subdirectory(rtm-config)
endif()

add_subdirectory(environment-setup-scripts)