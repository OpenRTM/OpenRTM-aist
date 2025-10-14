cmake_minimum_required (VERSION 3.16)

function(examples_build target)
	cmake_parse_arguments(EB
		""
		"MAIN;VXADDFILE"
		"SRCS;GENSRCS;EXTERNAL_INCLUDE;DEPS"
		${ARGN})

	# if empty, target_include_directories() is failed.
	if(NOT EB_EXTERNAL_INCLUDE)
		set(EB_EXTERNAL_INCLUDE PRIVATE)
	endif()

	link_directories(${ORB_LINK_DIR})
	add_definitions(${ORB_C_FLAGS_LIST})
	add_definitions(${COIL_C_FLAGS_LIST})
	if(WIN32)
		add_definitions(-DRTM_SKEL_IMPORT_SYMBOL)
	endif()

	if(VXWORKS AND NOT RTP)
		set(standalone_srcs ${EB_SRCS} ${EB_MAIN}
			${CMAKE_SOURCE_DIR}/src/lib/rtm/DataFlowComponentBase.cpp
			${EB_VXADDFILE})
		set(libs ${RTCSKEL_PROJECT_NAME})

		add_executable(${target} ${standalone_srcs})
		openrtm_common_set_compile_props(${target})
		openrtm_set_link_props_shared(${target})
		openrtm_include_rtm(${target})
		target_include_directories(${target} SYSTEM ${EB_EXTERNAL_INCLUDE})
		target_link_libraries(${target} ${libs})
		if(EB_DEPS)
			add_dependencies(${target} ${EB_DEPS})
		endif()

		install(TARGETS ${target} LIBRARY DESTINATION ${INSTALL_RTM_EXAMPLE_DIR}
					ARCHIVE DESTINATION ${INSTALL_RTM_EXAMPLE_DIR}
					RUNTIME DESTINATION ${INSTALL_RTM_EXAMPLE_DIR}
					COMPONENT examples)
	else()
		set(libs ${RTM_PROJECT_NAME} ${ORB_LIBRARIES} ${DATATYPE_FACTORIES})

		# .cpp -> .o
		add_library(${target}_objlib OBJECT ${EB_SRCS})
		openrtm_common_set_compile_props(${target}_objlib)
		openrtm_include_rtm(${target}_objlib)
		target_include_directories(${target}_objlib SYSTEM ${EB_EXTERNAL_INCLUDE})
		if(EB_DEPS)
			add_dependencies(${target}_objlib ${EB_DEPS})
		endif()

		# generated .cpp -> .o
		if(EB_GENSRCS)
			add_library(${target}_gen_objlib OBJECT ${EB_GENSRCS})
			openrtm_gencode_set_compile_props(${target}_gen_objlib)
			openrtm_include_rtm(${target}_gen_objlib)
			target_include_directories(${target}_gen_objlib SYSTEM ${EB_EXTERNAL_INCLUDE})
			if(EB_DEPS)
				add_dependencies(${target}_gen_objlib ${EB_DEPS})
			endif()
			set(EB_GENOBJLIB $<TARGET_OBJECTS:${target}_gen_objlib>)
		endif()

		# .o, main() -> executable
		add_executable(${target}Comp
			$<TARGET_OBJECTS:${target}_objlib>
			${EB_GENOBJLIB}
			${EB_MAIN})
		openrtm_common_set_compile_props(${target}Comp)
		openrtm_set_link_props_shared(${target}Comp)
		openrtm_include_rtm(${target}Comp)
		target_include_directories(${target}Comp SYSTEM ${EB_EXTERNAL_INCLUDE})
		target_link_libraries(${target}Comp ${libs} ${RTM_LINKER_OPTION})
		if(EB_DEPS)
			add_dependencies(${target}Comp ${EB_DEPS})
		endif()

		# .o -> .s
		add_library(${target} SHARED
			$<TARGET_OBJECTS:${target}_objlib>
			${EB_GENOBJLIB})
		openrtm_set_link_props_shared(${target})
		target_link_libraries(${target} PRIVATE ${libs} ${RTM_LINKER_OPTION})
		set_target_properties(${target} PROPERTIES PREFIX "")

		install(TARGETS ${target}Comp LIBRARY
			DESTINATION ${INSTALL_RTM_EXAMPLE_DIR}
			RUNTIME DESTINATION ${INSTALL_RTM_EXAMPLE_DIR}
			COMPONENT examples)

		install(TARGETS ${target} 
			LIBRARY DESTINATION ${INSTALL_RTM_EXAMPLE_SHAREDLIB_DIR}
			RUNTIME DESTINATION ${INSTALL_RTM_EXAMPLE_SHAREDLIB_DIR}
			COMPONENT examples)
	endif()

	if(VXWORKS)
		if(RTP)
			set_target_properties(${target}Comp PROPERTIES SUFFIX ".vxe")
		else()
			set_target_properties(${target} PROPERTIES SUFFIX ".out")
		endif()
	endif()
endfunction()

add_subdirectory(ConsoleIn)
add_subdirectory(ConsoleOut)
add_subdirectory(SeqIn)
add_subdirectory(SeqOut)
add_subdirectory(MyServiceConsumer)
add_subdirectory(MyServiceProvider)
add_subdirectory(ConfigSample)
add_subdirectory(Composite)
add_subdirectory(Controller)
add_subdirectory(Motor)
add_subdirectory(Sensor)
if(NOT WINDOWS_MSM_BUILD)
  #add_subdirectory(AutoTest)
  #add_subdirectory(HelloWorld)
  add_subdirectory(ExtTrigger)
  #add_subdirectory(StringIO)
  add_subdirectory(Analyzer)
  add_subdirectory(Throughput)
  add_subdirectory(StaticFsm)
  add_subdirectory(Templates)
  add_subdirectory(Serializer)
endif()
