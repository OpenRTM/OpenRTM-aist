cmake_minimum_required (VERSION 3.0.2)

project (FileNameservice
	VERSION ${RTM_VERSION}
	LANGUAGES CXX)

link_directories(${ORB_LINK_DIR}
		 ${Boost_LIBRARY_DIRS})
add_definitions(${ORB_C_FLAGS_LIST})
add_definitions(${COIL_C_FLAGS_LIST})


set(target FileNameservice)

set(srcs FileNameservice.cpp FileNameservice.h)


if(VXWORKS AND NOT RTP)
	set(libs ${RTCSKEL_PROJECT_NAME})

	add_executable(${target} ${srcs})
	openrtm_common_set_compile_props(${target})
	openrtm_include_rtm(${target})
	target_include_directories(${target} SYSTEM
		PRIVATE ${Boost_INCLUDE_DIRS})
	target_link_libraries(${target} ${libs})

	install(TARGETS ${target} LIBRARY DESTINATION ${INSTALL_RTM_EXT_DIR}/local_service
				ARCHIVE DESTINATION ${INSTALL_RTM_EXT_DIR}/local_service
				RUNTIME DESTINATION ${INSTALL_RTM_EXT_DIR}/local_service
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
	target_include_directories(${target} SYSTEM
		PRIVATE ${Boost_INCLUDE_DIRS})
	target_link_libraries(${target} PRIVATE ${libs} ${RTM_LINKER_OPTION})
	set_target_properties(${target} PROPERTIES PREFIX "")


	install(TARGETS ${target} LIBRARY DESTINATION ${INSTALL_RTM_EXT_DIR}/local_service
				ARCHIVE DESTINATION ${INSTALL_RTM_EXT_DIR}/local_service
				RUNTIME DESTINATION ${INSTALL_RTM_EXT_DIR}/local_service
				COMPONENT ext)
endif()






if(VXWORKS)
	if(RTP)
	else(RTP)	
		set_target_properties(${target} PROPERTIES SUFFIX ".out")
	endif(RTP)
endif(VXWORKS)


