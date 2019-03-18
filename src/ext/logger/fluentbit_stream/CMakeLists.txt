cmake_minimum_required (VERSION 3.0.2)

project (FluentBit
	VERSION ${RTM_VERSION}
	LANGUAGES CXX)



set(FLUENTBIT_ROOT ${FLUENTBIT_ROOT} CACHE PATH "set FLUENTBIT_ROOT")


if(NOT FLUENTBIT_ROOT)
	message(FATAL_ERROR "Please set FLUENTBIT_ROOT.")
endif()

set(FLUENTBIT_WORKDIR ${FLUENTBIT_WORKDIR} CACHE PATH "set FLUENTBIT_WORKDIR")

if(NOT FLUENTBIT_WORKDIR)
	message(FATAL_ERROR "Please set FLUENTBIT_WORKDIR.")
endif()

link_directories(${FLUENTBIT_WORKDIR}/lib)





link_directories(${ORB_LINK_DIR})
add_definitions(${ORB_C_FLAGS_LIST})
add_definitions(${COIL_C_FLAGS_LIST})
add_definitions(-Dtypeof=decltype)


set(target FluentBit)

set(srcs FluentBit.cpp FluentBit.h)


if(VXWORKS AND NOT RTP)
	set(libs ${RTCSKEL_PROJECT_NAME})

	add_executable(${target} ${srcs})
	openrtm_common_set_compile_props(${target})
	target_link_libraries(${target} ${libs} fluent-bit)

	install(TARGETS ${target} LIBRARY DESTINATION ${INSTALL_RTM_EXT_DIR}/logger
				ARCHIVE DESTINATION ${INSTALL_RTM_EXT_DIR}/logger
				RUNTIME DESTINATION ${INSTALL_RTM_EXT_DIR}/logger
				COMPONENT ext)
else()
	if(VXWORKS)
		set(libs ${RTM_PROJECT_NAME} ${ORB_LIBRARIES} ${COIL_PROJECT_NAME} ${RTCSKEL_PROJECT_NAME})
	else(VXWORKS)
		set(libs ${RTM_PROJECT_NAME} ${ORB_LIBRARIES} ${COIL_PROJECT_NAME})
	endif(VXWORKS)

	add_library(${target} SHARED ${srcs})
	openrtm_common_set_compile_props(${target})
	openrtm_include_rtm(${target})
	target_include_directories(SYSTEM
	PRIVATE ${FLUENTBIT_ROOT}/include
	PRIVATE ${FLUENTBIT_ROOT}/lib/monkey/include
	PRIVATE ${FLUENTBIT_ROOT}/lib/mbedtls-2.14.1/include
	PRIVATE ${FLUENTBIT_ROOT}/lib/msgpack-3.1.1/include
	PRIVATE ${FLUENTBIT_ROOT}/lib/flb_libco
	PRIVATE ${FLUENTBIT_ROOT}/lib/jemalloc-5.1.0/include
	PRIVATE ${FLUENTBIT_ROOT}/lib/jsmn
	PRIVATE ${FLUENTBIT_ROOT}/lib/onigmo
	PRIVATE ${FLUENTBIT_ROOT}/lib/sha1
	PRIVATE ${FLUENTBIT_ROOT}/lib/sqlite-amalgamation-3400000)
	target_link_libraries(${target} PRIVATE ${libs} ${RTM_LINKER_OPTION} fluent-bit)
	set_target_properties(${target} PROPERTIES PREFIX "")


	install(TARGETS ${target} LIBRARY DESTINATION ${INSTALL_RTM_EXT_DIR}/logger
				ARCHIVE DESTINATION ${INSTALL_RTM_EXT_DIR}/logger
				RUNTIME DESTINATION ${INSTALL_RTM_EXT_DIR}/logger
				COMPONENT ext)
endif()






if(VXWORKS)
	if(RTP)
	else(RTP)	
		set_target_properties(${target} PROPERTIES SUFFIX ".out")
	endif(RTP)
endif(VXWORKS)


