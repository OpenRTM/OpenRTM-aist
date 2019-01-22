cmake_minimum_required (VERSION 2.6)

if(UNIX)
	set(FLUENTBIT_ENABLE ${FLUENTBIT_ENABLE} CACHE BOOL "set FLUENTBIT_ENABLE")
	if(FLUENTBIT_ENABLE)
		add_subdirectory(fluentbit_stream)
	endif(FLUENTBIT_ENABLE)
endif(UNIX)

