cmake_minimum_required (VERSION 3.16)

project (ExtendedFsmServiceProvider
	VERSION ${RTM_VERSION}
	LANGUAGES CXX)

link_directories(${ORB_LINK_DIR})
add_definitions(${ORB_C_FLAGS_LIST})
add_definitions(${COIL_C_FLAGS_LIST})
if(WIN32)
	add_definitions(-DRTM_SKEL_IMPORT_SYMBOL)
endif()


set(target ExtendedFsmServiceProvider)

set(srcs ExtendedFsmServiceProvider.cpp ExtendedFsmServiceProvider.h)


if(VXWORKS AND NOT RTP)
	set(libs ${RTCSKEL_PROJECT_NAME})

	add_executable(${target} ${srcs})
	openrtm_common_set_compile_props(${target})
	openrtm_set_link_props_shared(${target})
	openrtm_include_rtm(${target})
	target_link_libraries(${target} ${libs})

	install(TARGETS ${target} LIBRARY DESTINATION ${INSTALL_RTM_EXT_DIR}/sdo
				ARCHIVE DESTINATION ${INSTALL_RTM_EXT_DIR}/sdo
				RUNTIME DESTINATION ${INSTALL_RTM_EXT_DIR}/sdo
				COMPONENT ext)
else()
	set(libs ${RTM_PROJECT_NAME} ${ORB_LIBRARIES} ${DATATYPE_FACTORIES})


	add_library(${target} SHARED ${srcs})
	openrtm_set_link_props_shared(${target})
	openrtm_common_set_compile_props(${target})
	openrtm_include_rtm(${target})
	target_link_libraries(${target} PRIVATE ${libs} ${RTM_LINKER_OPTION})
	set_target_properties(${target} PROPERTIES PREFIX "")


	install(TARGETS ${target} LIBRARY DESTINATION ${INSTALL_RTM_EXT_DIR}/sdo
				RUNTIME DESTINATION ${INSTALL_RTM_EXT_DIR}/sdo
				COMPONENT ext)
endif()






if(VXWORKS)
	if(RTP)
	else(RTP)	
		set_target_properties(${target} PROPERTIES SUFFIX ".out")
	endif(RTP)
endif(VXWORKS)



