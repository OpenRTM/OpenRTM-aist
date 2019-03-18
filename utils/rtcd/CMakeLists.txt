cmake_minimum_required (VERSION 3.0.2)
set(target rtcd)
project (${target}
	VERSION ${RTM_VERSION}
	LANGUAGES CXX)


link_directories(${ORB_LINK_DIR})
add_definitions(${ORB_C_FLAGS_LIST})
add_definitions(${COIL_C_FLAGS_LIST})

if(VXWORKS)
	add_executable(${target} rtcd_vxorks.cpp)
else(VXWORKS)
	add_executable(${target} rtcd.cpp)
endif(VXWORKS)
openrtm_common_set_compile_props(${target})
openrtm_include_rtm(${target})

if(VXWORKS)
	set(libs ${RTM_PROJECT_NAME} ${ORB_LIBRARIES} ${COIL_PROJECT_NAME} ${RTCSKEL_PROJECT_NAME})
else(VXWORKS)
	set(libs ${RTM_PROJECT_NAME} ${ORB_LIBRARIES} ${COIL_PROJECT_NAME})
endif(VXWORKS)
target_link_libraries(${target} ${libs} ${RTM_LINKER_OPTION})

if(VXWORKS)
	if(RTP)
		set_target_properties(${target} PROPERTIES SUFFIX ".vxe")
	else(RTP)	
		set_target_properties(${target} PROPERTIES SUFFIX ".out")
	endif(RTP)
endif(VXWORKS)

if(WIN32)
install(TARGETS ${target} LIBRARY DESTINATION ${INSTALL_RTM_LIB_DIR}
			ARCHIVE DESTINATION ${INSTALL_RTM_LIB_DIR}
			RUNTIME DESTINATION ${INSTALL_RTM_LIB_DIR}
			COMPONENT utils)
else(WIN32)
install(TARGETS ${target} LIBRARY DESTINATION ${INSTALL_RTM_LIB_DIR}
			ARCHIVE DESTINATION ${INSTALL_RTM_LIB_DIR}
			RUNTIME DESTINATION ${INSTALL_RTM_BIN_DIR}
			COMPONENT utils)
endif(WIN32)
