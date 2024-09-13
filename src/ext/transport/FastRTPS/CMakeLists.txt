cmake_minimum_required (VERSION 3.5.1)

project (FastRTPSTransport
	VERSION ${RTM_VERSION}
	LANGUAGES CXX)

find_package(fastcdr REQUIRED)
find_package(fastrtps REQUIRED)

link_directories(${ORB_LINK_DIR})
add_definitions(${ORB_C_FLAGS_LIST})

if(WIN32)
	add_definitions(-DRTM_SKEL_IMPORT_SYMBOL)
	add_definitions(-DNOGDI)
	add_definitions(-DNOMINMAX)
	add_definitions(-DTRANSPORT_PLUGIN)
endif()


set(target FastRTPSTransport)

set(srcs FastRTPSTransport.cpp FastRTPSTransport.h FastRTPSInPort.cpp FastRTPSInPort.h FastRTPSOutPort.cpp FastRTPSOutPort.h FastRTPSManager.cpp FastRTPSManager.h FastRTPSMessageInfo.cpp FastRTPSMessageInfo.h CORBACdrData.cpp CORBACdrData.h CORBACdrDataPubSubTypes.cpp CORBACdrDataPubSubTypes.h)


if(VXWORKS AND NOT RTP)
	set(libs ${RTCSKEL_PROJECT_NAME})

	add_executable(${target} ${srcs})
	openrtm_common_set_compile_props(${target})
	openrtm_set_link_props_shared(${target})
	openrtm_include_rtm(${target})
	target_link_libraries(${target} ${libs} fastcdr fastrtps)

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
	target_link_libraries(${target} PRIVATE ${libs} ${RTM_LINKER_OPTION} fastcdr fastrtps)
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


install(FILES FastRTPSMessageInfo.h DESTINATION ${INSTALL_RTM_INCLUDE_DIR}/rtm/ext/FastRTPS COMPONENT ext)



if(VXWORKS)
	if(RTP)
	else(RTP)	
		set_target_properties(${target} PROPERTIES SUFFIX ".out")
	endif(RTP)
endif(VXWORKS)

