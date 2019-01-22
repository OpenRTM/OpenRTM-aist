cmake_minimum_required (VERSION 2.6)

project (FluentBit)



set(FLUENTBIT_ROOT ${FLUENTBIT_ROOT} CACHE PATH "set FLUENTBIT_ROOT")


if(NOT FLUENTBIT_ROOT)
	message(FATAL_ERROR "Please set FLUENTBIT_ROOT.")
endif()

set(FLUENTBIT_WORKDIR ${FLUENTBIT_WORKDIR} CACHE PATH "set FLUENTBIT_WORKDIR")

if(NOT FLUENTBIT_WORKDIR)
	message(FATAL_ERROR "Please set FLUENTBIT_WORKDIR.")
endif()





include_directories(${FLUENTBIT_ROOT}/include)
include_directories(${FLUENTBIT_ROOT}/lib/monkey/include)
include_directories(${FLUENTBIT_ROOT}/lib/mbedtls-2.14.1/include)
include_directories(${FLUENTBIT_ROOT}/lib/msgpack-3.1.1/include)
include_directories(${FLUENTBIT_ROOT}/lib/flb_libco)
include_directories(${FLUENTBIT_ROOT}/lib/jemalloc-5.1.0/include)
include_directories(${FLUENTBIT_ROOT}/lib/jsmn)
include_directories(${FLUENTBIT_ROOT}/lib/onigmo)
include_directories(${FLUENTBIT_ROOT}/lib/sha1)
include_directories(${FLUENTBIT_ROOT}/lib/sqlite-amalgamation-3400000)


link_directories(${FLUENTBIT_WORKDIR}/lib)





link_directories(${ORB_LINK_DIR})
include_directories(
		${RTM_INCLUDE_DIR}
		${ORB_INCLUDE_DIR}
		)

ADD_DEFINITIONS(${ORB_C_FLAGS_LIST})


set(target FluentBit)

set(srcs FluentBit.cpp FluentBit.h)


if(VXWORKS AND NOT RTP)
	set(libs ${RTCSKEL_PROJECT_NAME})

	add_executable(${target} ${srcs})
	target_link_libraries(${target} ${libs} fluent-bit)
	add_dependencies(${target} ${RTM_PROJECT_NAME})

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
	target_link_libraries(${target} ${libs} ${RTM_LINKER_OPTION} fluent-bit)
	add_dependencies(${target} ${RTM_PROJECT_NAME})
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


