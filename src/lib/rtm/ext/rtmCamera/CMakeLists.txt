cmake_minimum_required (VERSION 3.0.2)

if(WIN32)
	set(target rtmCamera${RTM_SHORT_VERSION}_${RTM_VC_VER})
else()
	set(target rtmCamera)
endif()

project (${target}
	VERSION ${RTM_VERSION}
	LANGUAGES CXX)

set(${PROJECT_NAME}_IDLS CameraCommonInterface.idl)

openrtm_idl_compile_all(${PROJECT_NAME}
	"${${PROJECT_NAME}_IDLS}"
	"-I${PROJECT_SOURCE_DIR}/..;-I${PROJECT_SOURCE_DIR}/../../idl")

link_directories(${ORB_LINK_DIR})
add_definitions(${ORB_C_FLAGS_LIST})
add_definitions(${COIL_C_FLAGS_LIST})

if(VXWORKS)
	add_library(${PROJECT_NAME} SHARED ${${PROJECT_NAME}_IDLSKEL})
	openrtm_common_set_compile_props(${PROJECT_NAME})
	openrtm_include_rtm(${PROJECT_NAME})
	install(PROJECT_NAMES ${PROJECT_NAME} LIBRARY DESTINATION ${INSTALL_RTM_LIB_DIR}
						ARCHIVE DESTINATION ${INSTALL_RTM_BIN_DIR}
						RUNTIME DESTINATION ${INSTALL_RTM_BIN_DIR}
						COMPONENT ext)
else()
	if(WIN32)
		set(additional_srcs rtmCameraFactory.cpp)
	endif()
	add_library(${PROJECT_NAME}_objlib OBJECT
		${${PROJECT_NAME}_IDLSKEL}
		${additional_srcs})
	openrtm_gencode_set_compile_props(${PROJECT_NAME}_objlib)
	openrtm_include_rtm(${PROJECT_NAME}_objlib)

	add_library(${PROJECT_NAME}-static STATIC $<TARGET_OBJECTS:${PROJECT_NAME}_objlib>)

	if(WIN32)
		set(DEF_FILE_NAME  ${PROJECT_NAME}.def)
		add_custom_command(TARGET ${PROJECT_NAME}-static
			POST_BUILD
			COMMAND python -B ${MAKE_DEF_COMMAND} $<TARGET_FILE:${PROJECT_NAME}-static> ${PROJECT_NAME} ${RTM_SHORT_VERSION} ${DEF_FILE_NAME}
			WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
		)
	endif()

	add_library(${PROJECT_NAME} SHARED
		$<TARGET_OBJECTS:${PROJECT_NAME}_objlib>
		${CMAKE_CURRENT_BINARY_DIR}/${DEF_FILE_NAME})
	set_target_properties(${PROJECT_NAME} PROPERTIES DEBUG_POSTFIX d)
	if(WIN32)
		target_link_libraries(${PROJECT_NAME} PRIVATE
			${ORB_LIBRARIES} ${COIL_PROJECT_NAME} ${RTM_LINKER_OPTION} ${RTM_PROJECT_NAME})
	else()
		target_link_libraries(${PROJECT_NAME} PRIVATE ${ORB_LIBRARIES})
	endif()

	if(UNIX)
		install(TARGETS ${PROJECT_NAME} LIBRARY DESTINATION ${INSTALL_RTM_LIB_DIR}
						ARCHIVE DESTINATION ${INSTALL_RTM_LIB_DIR}
						RUNTIME DESTINATION ${INSTALL_RTM_BIN_DIR}
						COMPONENT ext)

		set_target_properties(${PROJECT_NAME}-static PROPERTIES DEBUG_POSTFIX d)
		set_target_properties(${PROJECT_NAME}-static PROPERTIES OUTPUT_NAME ${PROJECT_NAME} CLEAN_DIRECT_OUTPUT 1)
		install(TARGETS ${PROJECT_NAME}-static LIBRARY DESTINATION ${INSTALL_RTM_LIB_DIR}
						ARCHIVE DESTINATION ${INSTALL_RTM_LIB_DIR}
						RUNTIME DESTINATION ${INSTALL_RTM_BIN_DIR}
						COMPONENT ext)
	elseif(WIN32)
		set_source_files_properties(${DEF_FILE_NAME} PROPERTIES GENERATED 1)
		add_dependencies(${PROJECT_NAME} ${PROJECT_NAME}-static)
		install(TARGETS ${PROJECT_NAME} LIBRARY DESTINATION ${INSTALL_RTM_LIB_DIR}
						ARCHIVE DESTINATION ${INSTALL_RTM_LIB_DIR}
						RUNTIME DESTINATION ${INSTALL_RTM_LIB_DIR}
						COMPONENT ext)

		install(TARGETS ${PROJECT_NAME}-static LIBRARY DESTINATION ${INSTALL_RTM_LIB_DIR}
						ARCHIVE DESTINATION ${INSTALL_RTM_LIB_DIR}
						RUNTIME DESTINATION ${INSTALL_RTM_LIB_DIR}
						COMPONENT ext)
	endif()
endif()

install(FILES ${${PROJECT_NAME}_IDLS} DESTINATION ${INSTALL_RTM_INCLUDE_DIR}/rtm/ext COMPONENT ext)
foreach(hdr ${${PROJECT_NAME}_IDLHDR})
	install(FILES "${PROJECT_BINARY_DIR}/${hdr}" DESTINATION ${INSTALL_RTM_INCLUDE_DIR}/rtm/ext COMPONENT ext)
endforeach()
