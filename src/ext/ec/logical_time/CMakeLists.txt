cmake_minimum_required (VERSION 3.0.2)

project (LogicalTimeTriggeredEC
	VERSION ${RTM_VERSION}
	LANGUAGES CXX)

set(${PROJECT_NAME}_IDLS LogicalTimeTriggeredEC.idl)
set(${PROJECT_NAME}_SRCS LogicalTimeTriggeredEC.cpp)

openrtm_idl_compile_all(${PROJECT_NAME}
	"${${PROJECT_NAME}_IDLS}"
	"-I${CMAKE_CURRENT_SOURCE_DIR};-I${CMAKE_SOURCE_DIR}/src/lib/rtm/idl")

link_directories(${ORB_LINK_DIR})

add_definitions(${ORB_C_FLAGS_LIST})
add_definitions(${COIL_C_FLAGS_LIST})

if(VXWORKS AND NOT RTP)
	set(libs ${RTCSKEL_PROJECT_NAME})

	add_executable(${PROJECT_NAME} ${${PROJECT_NAME}_SRCS} ${${PROJECT_NAME}_IDLSKEL})
	openrtm_common_set_compile_props(${PROJECT_NAME})
	openrtm_include_rtm(${PROJECT_NAME})
	target_include_directories(${PROJECT_NAME}
		PUBLIC ${PROJECT_BINARY_DIR})
	target_link_libraries(${PROJECT_NAME} ${libs})
	add_dependencies(${PROJECT_NAME} ${PROJECT_NAME}_IDLTGT)

	install(TARGETS ${PROJECT_NAME} LIBRARY DESTINATION ${INSTALL_RTM_EXT_DIR}/ec
				ARCHIVE DESTINATION ${INSTALL_RTM_EXT_DIR}/ec
				RUNTIME DESTINATION ${INSTALL_RTM_EXT_DIR}/ec
				COMPONENT ext)
else()
	if(VXWORKS)
		set(libs ${RTM_PROJECT_NAME} ${ORB_LIBRARIES} ${COIL_PROJECT_NAME} ${RTCSKEL_PROJECT_NAME})
	else()
		set(libs ${RTM_PROJECT_NAME} ${ORB_LIBRARIES} ${COIL_PROJECT_NAME})
	endif()

	add_library(${PROJECT_NAME}_objlib OBJECT ${${PROJECT_NAME}_SRCS})
	openrtm_common_set_compile_props(${PROJECT_NAME}_objlib)
	openrtm_include_rtm(${PROJECT_NAME}_objlib)
	target_include_directories(${PROJECT_NAME}_objlib
		PUBLIC ${PROJECT_BINARY_DIR})
	add_dependencies(${PROJECT_NAME}_objlib ${PROJECT_NAME}_IDLTGT)

	add_library(${PROJECT_NAME}_idl_objlib OBJECT ${${PROJECT_NAME}_IDLSKEL})
	openrtm_gencode_set_compile_props(${PROJECT_NAME}_idl_objlib)
	openrtm_include_rtm(${PROJECT_NAME}_idl_objlib)
	target_include_directories(${PROJECT_NAME}_idl_objlib
		PUBLIC ${PROJECT_BINARY_DIR})
	add_dependencies(${PROJECT_NAME}_idl_objlib ${PROJECT_NAME}_IDLTGT)

	add_library(${PROJECT_NAME} SHARED
		$<TARGET_OBJECTS:${PROJECT_NAME}_objlib>
		$<TARGET_OBJECTS:${PROJECT_NAME}_idl_objlib>)
	target_link_libraries(${PROJECT_NAME} PRIVATE ${libs} ${RTM_LINKER_OPTION})
	set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "")

	install(TARGETS ${PROJECT_NAME} LIBRARY DESTINATION ${INSTALL_RTM_EXT_DIR}/ec
				ARCHIVE DESTINATION ${INSTALL_RTM_EXT_DIR}/ec
				RUNTIME DESTINATION ${INSTALL_RTM_EXT_DIR}/ec
				COMPONENT ext)
endif()

if(VXWORKS)
	if(RTP)
	else()
		set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".out")
	endif()
endif()
