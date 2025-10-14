cmake_minimum_required (VERSION 3.16)


set(FLUENTBIT_ENABLE FALSE CACHE BOOL "set FLUENTBIT_ENABLE")
if(FLUENTBIT_ENABLE)
	add_subdirectory(fluentbit_stream)
endif(FLUENTBIT_ENABLE)


