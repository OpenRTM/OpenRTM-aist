cmake_minimum_required (VERSION 3.16)

project (FSMComponentObserverConsumer
	VERSION ${RTM_VERSION}
	LANGUAGES CXX)

set(${PROJECT_NAME}_IDLS ComponentObserver.idl)
set(${PROJECT_NAME}_SRCS ComponentObserverConsumer.cpp)

openrtm_idl_compile_all(${PROJECT_NAME}
	"${${PROJECT_NAME}_IDLS}"
	"-I${CMAKE_CURRENT_SOURCE_DIR};-I${CMAKE_SOURCE_DIR}/src/lib/rtm/idl"
	${PROJECT_NAME} "${RTM_INCLUDE_IDLS}")

link_directories(${ORB_LINK_DIR})
add_definitions(${ORB_C_FLAGS_LIST})
add_definitions(${COIL_C_FLAGS_LIST})
if(WIN32)
	add_definitions(-DRTM_SKEL_IMPORT_SYMBOL)
endif()

if(VXWORKS AND NOT RTP)
	set(libs ${RTCSKEL_PROJECT_NAME})

	add_executable(${PROJECT_NAME} ${${PROJECT_NAME}_SRCS} ${${PROJECT_NAME}_IDLSTUB})
	openrtm_common_set_compile_props(${PROJECT_NAME})
	openrtm_set_link_props_shared(${PROJECT_NAME})
	openrtm_include_rtm(${PROJECT_NAME})
	target_include_directories(${PROJECT_NAME} SYSTEM
		PUBLIC ${PROJECT_BINARY_DIR})
	target_link_libraries(${PROJECT_NAME} ${libs})
	add_dependencies(${PROJECT_NAME} ${RTM_PROJECT_NAME})
	add_dependencies(${PROJECT_NAME} ${PROJECT_NAME}_IDLTGT)

	install(TARGETS ${PROJECT_NAME} LIBRARY DESTINATION ${INSTALL_RTM_EXT_DIR}/sdo
				ARCHIVE DESTINATION ${INSTALL_RTM_EXT_DIR}/sdo
				RUNTIME DESTINATION ${INSTALL_RTM_EXT_DIR}/sdo
				COMPONENT ext)

else()
	set(libs ${RTM_PROJECT_NAME} ${ORB_LIBRARIES} ${DATATYPE_FACTORIES})

	add_library(${PROJECT_NAME}_objlib OBJECT ${${PROJECT_NAME}_SRCS})
	openrtm_common_set_compile_props(${PROJECT_NAME}_objlib)
	openrtm_include_rtm(${PROJECT_NAME}_objlib)
	target_include_directories(${PROJECT_NAME}_objlib SYSTEM
		PUBLIC ${PROJECT_BINARY_DIR})
add_dependencies(${PROJECT_NAME}_objlib ${PROJECT_NAME}_IDLTGT)

	add_library(${PROJECT_NAME}_idl_objlib OBJECT ${${PROJECT_NAME}_IDLSTUB})
	openrtm_gencode_set_compile_props(${PROJECT_NAME}_idl_objlib)
	openrtm_include_rtm(${PROJECT_NAME}_idl_objlib)
	target_include_directories(${PROJECT_NAME}_idl_objlib SYSTEM
		PUBLIC ${PROJECT_BINARY_DIR})
	add_dependencies(${PROJECT_NAME}_idl_objlib ${PROJECT_NAME}_IDLTGT)

	add_library(${PROJECT_NAME} SHARED
		$<TARGET_OBJECTS:${PROJECT_NAME}_objlib>
		$<TARGET_OBJECTS:${PROJECT_NAME}_idl_objlib>)
	openrtm_set_link_props_shared(${PROJECT_NAME})
	target_link_libraries(${PROJECT_NAME} PRIVATE ${libs} ${RTM_LINKER_OPTION})
	set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "")
	set_target_properties(${PROJECT_NAME} PROPERTIES
		OUTPUT_NAME ComponentObserverConsumer
		CLEAN_DIRECT_OUTPUT 1)

	install(TARGETS ${PROJECT_NAME} LIBRARY DESTINATION ${INSTALL_RTM_EXT_DIR}/sdo
				RUNTIME DESTINATION ${INSTALL_RTM_EXT_DIR}/sdo
				COMPONENT ext)
endif()

set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME ComponentObserverConsumer CLEAN_DIRECT_OUTPUT 1)

if(VXWORKS)
	if(RTP)
	else()
		set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".out")
	endif()
endif()
