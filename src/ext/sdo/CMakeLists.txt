cmake_minimum_required (VERSION 3.16)

add_subdirectory(logger)
add_subdirectory(extended_fsm)
if(OBSERVER_ENABLE)
	add_subdirectory(observer)
endif(OBSERVER_ENABLE)
add_subdirectory(fsm4rtc_observer)
