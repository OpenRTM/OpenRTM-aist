cmake_minimum_required (VERSION 3.0.2)

project (ExtTrigger
	VERSION ${RTM_VERSION}
	LANGUAGES CXX)



set(srcs ConsoleIn.cpp ConsoleIn.h)
ComponentBuild(ConsoleInEx "${srcs}" ConsoleInComp.cpp "" "")


set(srcs ConsoleOut.cpp ConsoleOut.h)
ComponentBuild(ConsoleOutEx "${srcs}" ConsoleOutComp.cpp "" "")


set(target ConnectorCompEx)


set(srcs ConnectorComp.cpp)

link_directories(${ORB_LINK_DIR})
add_definitions(${ORB_C_FLAGS_LIST})
add_definitions(${COIL_C_FLAGS_LIST})


if(VXWORKS AND NOT RTP)

else()
	if(VXWORKS)
		set(libs ${RTM_PROJECT_NAME} ${ORB_LIBRARIES} ${COIL_PROJECT_NAME} ${RTCSKEL_PROJECT_NAME})
	else(VXWORKS)
		set(libs ${RTM_PROJECT_NAME} ${ORB_LIBRARIES} ${COIL_PROJECT_NAME})
	endif(VXWORKS)

	add_executable(${target} ${srcs})
	openrtm_common_set_compile_props(${target})
	openrtm_include_rtm(${target})
	target_link_libraries(${target} ${libs} ${RTM_LINKER_OPTION})

	install(TARGETS ${target} LIBRARY DESTINATION ${INSTALL_RTM_EXAMPLE_DIR}
				ARCHIVE DESTINATION ${INSTALL_RTM_EXAMPLE_DIR}
				RUNTIME DESTINATION ${INSTALL_RTM_EXAMPLE_DIR}
				COMPONENT examples)

	install(FILES ${srcs} DESTINATION ${INSTALL_RTM_EXAMPLE_DIR}/src/ExtTrigger COMPONENT sources)

endif()


