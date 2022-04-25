cmake_minimum_required (VERSION 3.5.1)

project (SSMTransport
	VERSION ${RTM_VERSION}
	LANGUAGES CXX)


set(SSM_ROOT ${SSM_ROOT} CACHE PATH "set SSM_ROOT")



link_directories(${ORB_LINK_DIR})
if(SSM_ROOT)
	link_directories(${SSM_ROOT}/lib ${SSM_ROOT}/lib64)
endif()
add_definitions(${ORB_C_FLAGS_LIST})

if(WIN32)
	add_definitions(-DRTM_SKEL_IMPORT_SYMBOL)
	add_definitions(-DNOGDI)
	add_definitions(-DNOMINMAX)
	add_definitions(-DTRANSPORT_PLUGIN)
endif()


set(target SSMTransport)

set(srcs SSMTransport.cpp SSMTransport.h SSMInPort.cpp SSMInPort.h SSMOutPort.cpp SSMOutPort.h)


if(VXWORKS AND NOT RTP)
	set(libs ${RTCSKEL_PROJECT_NAME})

	add_executable(${target} ${srcs})
	openrtm_common_set_compile_props(${target})
	openrtm_set_link_props_shared(${target})
	openrtm_include_rtm(${target})
	target_link_libraries(${target} ${libs} ssm)
	if(SSM_ROOT)
		target_include_directories(${target} SYSTEM PUBLIC ${SSM_ROOT}/include)									
	endif()

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
	target_link_libraries(${target} PRIVATE ${libs} ${RTM_LINKER_OPTION} ssm)
	if(SSM_ROOT)
		target_include_directories(${target} SYSTEM PUBLIC ${SSM_ROOT}/include)									
	endif()
	set_target_properties(${target} PROPERTIES PREFIX "")

	set_target_properties(${target} PROPERTIES
				CXX_STANDARD 11
				CXX_STANDARD_REQUIRED YES
				CXX_EXTENSIONS NO
				)

	install(TARGETS ${target} LIBRARY DESTINATION ${INSTALL_RTM_EXT_DIR}/transport
				ARCHIVE DESTINATION ${INSTALL_RTM_EXT_DIR}/transport
				RUNTIME DESTINATION ${INSTALL_RTM_EXT_DIR}/transport
				COMPONENT ext)
endif()


if(VXWORKS)
	if(RTP)
	else(RTP)	
		set_target_properties(${target} PROPERTIES SUFFIX ".out")
	endif(RTP)
endif(VXWORKS)

set(INSTALL_SSMTRANSPORT_DIR ${INSTALL_RTM_EXT_DIR}/transport)
configure_file(
	${CMAKE_CURRENT_SOURCE_DIR}/rtc.ssm.conf.in ${PROJECT_BINARY_DIR}/rtc.ssm.conf @ONLY)
install(
	FILES ${PROJECT_BINARY_DIR}/rtc.ssm.conf
	DESTINATION ${INSTALL_RTM_ETC_DIR}/transport
	COMPONENT ext)
