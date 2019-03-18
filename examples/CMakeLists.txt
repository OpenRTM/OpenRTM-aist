cmake_minimum_required (VERSION 3.0.2)


function(ComponentBuild target srcs mainfile dependencies vxaddfile)
	link_directories(${ORB_LINK_DIR})
	add_definitions(${ORB_C_FLAGS_LIST})
	add_definitions(${COIL_C_FLAGS_LIST})

	if(VXWORKS AND NOT RTP)
		set(standalone_srcs ${srcs} ${mainfile} ${CMAKE_SOURCE_DIR}/src/lib/rtm/DataFlowComponentBase.cpp ${vxaddfile})
		set(libs ${RTCSKEL_PROJECT_NAME})

		add_executable(${target} ${standalone_srcs})
		openrtm_common_set_compile_props(${target})
		openrtm_include_rtm(${target})
		target_link_libraries(${target} ${libs})
		if(dependencies)
			add_dependencies(${target} ${dependencies})
		endif(dependencies)

		install(TARGETS ${target} LIBRARY DESTINATION ${INSTALL_RTM_EXAMPLE_DIR}
					ARCHIVE DESTINATION ${INSTALL_RTM_EXAMPLE_DIR}
					RUNTIME DESTINATION ${INSTALL_RTM_EXAMPLE_DIR}
					COMPONENT examples)
	else()
		if(VXWORKS)
			set(libs ${RTM_PROJECT_NAME} ${ORB_LIBRARIES} ${COIL_PROJECT_NAME} ${RTCSKEL_PROJECT_NAME})
		else(VXWORKS)
			set(libs ${RTM_PROJECT_NAME} ${ORB_LIBRARIES} ${COIL_PROJECT_NAME} ${DATATYPE_FACTORIES})
		endif(VXWORKS)

		add_library(${target}_objlib OBJECT ${srcs})
		openrtm_common_set_compile_props(${target}_objlib)
		openrtm_include_rtm(${target}_objlib)
		add_executable(${target}Comp $<TARGET_OBJECTS:${target}_objlib> ${mainfile})
		openrtm_common_set_compile_props(${target}Comp)
		openrtm_include_rtm(${target}Comp)
		target_link_libraries(${target}Comp ${libs} ${RTM_LINKER_OPTION})
		if(DATATYPE_FACTORIES)
			add_dependencies(${target}Comp ${DATATYPE_FACTORIES})
		endif(DATATYPE_FACTORIES)

		add_library(${target} SHARED $<TARGET_OBJECTS:${target}_objlib>)
		target_link_libraries(${target} PRIVATE ${libs} ${RTM_LINKER_OPTION})
		if(DATATYPE_FACTORIES)
			add_dependencies(${target} ${DATATYPE_FACTORIES})
		endif(DATATYPE_FACTORIES)
		set_target_properties(${target} PROPERTIES PREFIX "")

		if(dependencies)
			add_dependencies(${target}_objlib ${dependencies})
			add_dependencies(${target} ${dependencies})
			add_dependencies(${target}Comp ${dependencies})
		endif(dependencies)

		install(TARGETS ${target}Comp LIBRARY DESTINATION ${INSTALL_RTM_EXAMPLE_DIR}
					ARCHIVE DESTINATION ${INSTALL_RTM_EXAMPLE_DIR}
					RUNTIME DESTINATION ${INSTALL_RTM_EXAMPLE_DIR}
					COMPONENT examples)

		install(TARGETS ${target} LIBRARY DESTINATION ${INSTALL_RTM_EXAMPLE_SHAREDLIB_DIR}
					ARCHIVE DESTINATION ${INSTALL_RTM_EXAMPLE_SHAREDLIB_DIR}
					RUNTIME DESTINATION ${INSTALL_RTM_EXAMPLE_SHAREDLIB_DIR}
					COMPONENT examples)

		install(FILES ${standalone_srcs} DESTINATION ${INSTALL_RTM_EXAMPLE_DIR}/src/Analyzer COMPONENT sources)
	endif()


	if(VXWORKS)
		if(RTP)
			set_target_properties(${target}Comp PROPERTIES SUFFIX ".vxe")
		else(RTP)	
			set_target_properties(${target} PROPERTIES SUFFIX ".out")
		endif(RTP)
	endif(VXWORKS)

endfunction()



add_subdirectory(SimpleIO)
add_subdirectory(SeqIO)
#add_subdirectory(AutoTest)
#add_subdirectory(HelloWorld)
add_subdirectory(ExtTrigger)
add_subdirectory(SimpleService)
#add_subdirectory(StringIO)
add_subdirectory(ConfigSample)
add_subdirectory(Composite)
add_subdirectory(Analyzer)
add_subdirectory(Throughput)

add_subdirectory(StaticFsm)

add_subdirectory(Templates)
