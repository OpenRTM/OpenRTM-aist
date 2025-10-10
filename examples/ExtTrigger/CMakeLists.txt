cmake_minimum_required (VERSION 3.16)

project (ExtTrigger
	VERSION ${RTM_VERSION}
	LANGUAGES CXX)



set(srcs ConsoleIn.cpp ConsoleIn.h)
examples_build(ConsoleInEx SRCS "${srcs}" MAIN ConsoleInComp.cpp)


set(srcs ConsoleOut.cpp ConsoleOut.h)
examples_build(ConsoleOutEx SRCS "${srcs}" MAIN ConsoleOutComp.cpp)


set(target ConnectorCompEx)


set(srcs ConnectorComp.cpp)

link_directories(${ORB_LINK_DIR})
add_definitions(${ORB_C_FLAGS_LIST})
add_definitions(${COIL_C_FLAGS_LIST})
if(WIN32)
	add_definitions(-DRTM_SKEL_IMPORT_SYMBOL)
endif()


if(VXWORKS AND NOT RTP)

else()
	set(libs ${RTM_PROJECT_NAME} ${ORB_LIBRARIES} ${DATATYPE_FACTORIES})

	add_executable(${target} ${srcs})
	openrtm_common_set_compile_props(${target})
	openrtm_set_link_props_shared(${target})
	openrtm_include_rtm(${target})
	target_link_libraries(${target} ${libs} ${RTM_LINKER_OPTION})

	install(TARGETS ${target} LIBRARY DESTINATION ${INSTALL_RTM_EXAMPLE_DIR}
				ARCHIVE DESTINATION ${INSTALL_RTM_EXAMPLE_DIR}
				RUNTIME DESTINATION ${INSTALL_RTM_EXAMPLE_DIR}
				COMPONENT examples)

endif()


