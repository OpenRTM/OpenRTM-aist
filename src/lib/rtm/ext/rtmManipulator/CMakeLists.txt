cmake_minimum_required (VERSION 2.6)




if(WIN32)
	set(target rtmManipulator${RTM_SHORT_VERSION}_${RTM_VC_VER})
else(WIN32)
	set(target rtmManipulator)
endif(WIN32)

project (${target})







macro(idl_compile idl_name)

	set(ALL_IDL_FILES ${ALL_IDL_FILES} ../${idl_name}.idl)
	add_custom_command(
		OUTPUT ${idl_name}Skel.cpp
		#COMMAND mkdir -p include/rtm/idl
		#COMMAND cd include/rtm/idl
		#COMMAND ${IDL_COMPILE_COMMAND} ${IDLCOMMAND_FLAGS} -I${PROJECT_SOURCE_DIR}/idl ${PROJECT_SOURCE_DIR}/ext/${idl_name}.idl
		COMMAND ${IDL_COMPILE_COMMAND} ${IDLCOMMAND_FLAGS} -I${PROJECT_SOURCE_DIR}/.. -I${PROJECT_SOURCE_DIR}/../../idl ${PROJECT_SOURCE_DIR}/../${idl_name}.idl
		COMMAND python ${SKELWRAPPER_COMMAND} --include-dir="" --skel-suffix=Skel --stub-suffix=Stub --idl-file=${idl_name}.idl
		#WORKING_DIRECTORY ${CURRENT_BINARY_DIR}/include/rtm/idl
		WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
	)
	

	add_custom_target(${idl_name}_TGT DEPENDS ${idl_name}Skel.cpp)
	#set(ALL_IDL_SRCS ${ALL_IDL_SRCS} idl/${idl_name}Skel.cpp)
	#set(ALL_IDL_SRCS ${ALL_IDL_SRCS} idl/${idl_name}SKel.h)
	set(ALL_IDL_SRCS ${ALL_IDL_SRCS} ${idl_name}Skel.cpp)
	set(ALL_IDL_SRCS ${ALL_IDL_SRCS} ${idl_name}Skel.h)
	#set(ALL_IDL_SRCS ${ALL_IDL_SRCS} ${idl_name}Stub.cpp)
	#set(ALL_IDL_SRCS ${ALL_IDL_SRCS} ${idl_name}Stub.h)

	set(ALL_IDL_HEADERS ${ALL_IDL_HEADERS} ${PROJECT_BINARY_DIR}/${idl_name}Skel.h)
	set(ALL_IDL_HEADERS ${ALL_IDL_HEADERS} ${PROJECT_BINARY_DIR}/${idl_name}Stub.h)
	#set(ALL_IDL_SRCS ${ALL_IDL_SRCS} ${idl_name}_c.cc)
	#set(ALL_IDL_SRCS ${ALL_IDL_SRCS} ${idl_name}_c.h)
	#set(ALL_IDL_SRCS ${ALL_IDL_SRCS} ${idl_name}.cc)
	#set(ALL_IDL_SRCS ${ALL_IDL_SRCS} ${idl_name}.h)

	if(NOT TARGET ALL_IDL_TGT)
		add_custom_target(ALL_IDL_TGT)
	endif(NOT TARGET ALL_IDL_TGT)
	add_dependencies(ALL_IDL_TGT ${idl_name}_TGT)
	#include_directories(${PROJECT_BINARY_DIR}/idl)
endmacro()



set(idls
	ManipulatorCommonInterface_Common
	ManipulatorCommonInterface_DataTypes
	ManipulatorCommonInterface_Middle
	)




foreach(idl ${idls})
	idl_compile(${idl})
endforeach(idl)



link_directories(${ORB_LINK_DIR})
include_directories(
		${ORB_INCLUDE_DIR}
		${PROJECT_BINARY_DIR}
		)
ADD_DEFINITIONS(${ORB_C_FLAGS_LIST})


link_directories(${ORB_LINK_DIR})
include_directories(${ORB_INCLUDE_DIR})
ADD_DEFINITIONS(${ORB_C_FLAGS_LIST})


#set(ALL_IDL_SRCS ${ALL_IDL_SRCS} PARENT_SCOPE)

set_source_files_properties(${ALL_IDL_SRCS} PROPERTIES GENERATED 1)




if(WIN32)
	set(DEF_FILE_NAME  ${target}.def)

	add_library(${target}_static STATIC ${ALL_IDL_SRCS} )
	add_dependencies(${target}_static ${RTCSKEL_PROJECT_NAME})
	set_source_files_properties(${ALL_IDL_SRCS_RTM} PROPERTIES GENERATED 1)
	get_property(rtm_static_location TARGET ${target}_static PROPERTY LOCATION)

	install(TARGETS ${target}_static LIBRARY DESTINATION ${INSTALL_RTM_LIB_DIR}
					ARCHIVE DESTINATION ${INSTALL_RTM_LIB_DIR}
					RUNTIME DESTINATION ${INSTALL_RTM_LIB_DIR}
					COMPONENT ext)

	add_custom_command(OUTPUT ${DEF_FILE_NAME}
		COMMAND python ${MAKE_DEF_COMMAND} ${rtm_static_location} ${target} ${RTM_SHORT_VERSION} ${DEF_FILE_NAME}
		
		WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
	)
	add_custom_target(${target}_TGT DEPENDS ${DEF_FILE_NAME})
	add_dependencies(${target}_TGT ${target}_static)
	set(ALL_IDL_SRCS ${ALL_IDL_SRCS} ${CMAKE_CURRENT_BINARY_DIR}/${DEF_FILE_NAME})
endif(WIN32)





if(VXWORKS)
	add_library(${target} SHARED ${ALL_IDL_SRCS})
	install(TARGETS ${RTM_PROJECT_NAME} LIBRARY DESTINATION ${INSTALL_RTM_BIN_DIR}
						ARCHIVE DESTINATION ${INSTALL_RTM_BIN_DIR}
						RUNTIME DESTINATION ${INSTALL_RTM_BIN_DIR}
						COMPONENT ext)
else(VXWORKS)
	add_library(${target} SHARED ${ALL_IDL_SRCS})
	set_target_properties(${target} PROPERTIES DEBUG_POSTFIX d)
	target_link_libraries(${target} ${ORB_LIBRARIES} ${RTCSKEL_PROJECT_NAME})
	if(UNIX)
		install(TARGETS ${target} LIBRARY DESTINATION ${INSTALL_RTM_LIB_DIR}
						ARCHIVE DESTINATION ${INSTALL_RTM_LIB_DIR}
						RUNTIME DESTINATION ${INSTALL_RTM_BIN_DIR}
						COMPONENT ext)
		add_library(${target}-static STATIC ${ALL_IDL_SRCS})
		set_target_properties(${target}-static PROPERTIES DEBUG_POSTFIX d)
		set_target_properties(${target}-static PROPERTIES OUTPUT_NAME ${target} CLEAN_DIRECT_OUTPUT 1)
		install(TARGETS ${target}-static LIBRARY DESTINATION ${INSTALL_RTM_LIB_DIR}
						ARCHIVE DESTINATION ${INSTALL_RTM_LIB_DIR}
						RUNTIME DESTINATION ${INSTALL_RTM_BIN_DIR}
						COMPONENT ext)
	elseif(WIN32)
		set_source_files_properties(${DEF_FILE_NAME} PROPERTIES GENERATED 1)
		add_dependencies(${target} ${target}_TGT)
		add_dependencies(${target} ${target}_static)
		install(TARGETS ${target} LIBRARY DESTINATION ${INSTALL_RTM_LIB_DIR}
						ARCHIVE DESTINATION ${INSTALL_RTM_LIB_DIR}
						RUNTIME DESTINATION ${INSTALL_RTM_LIB_DIR}
						COMPONENT ext)
	endif()
endif(VXWORKS)







if(NOT TARGET ALL_IDL_TGT)
  add_custom_target(ALL_IDL_TGT)

endif(NOT TARGET ALL_IDL_TGT)
add_dependencies(${target} ALL_IDL_TGT)
add_dependencies(${target} ${RTCSKEL_PROJECT_NAME})








install(FILES ${ALL_IDL_FILES} ${ALL_IDL_HEADERS} DESTINATION ${INSTALL_RTM_INCLUDE_DIR}/rtm/ext COMPONENT ext)
 


foreach(idl ${idls})
	if(CORBA STREQUAL "ORBexpress")
		set(SKEL_STUB_HEADER_FILES ${SKEL_STUB_HEADER_FILES} ${PROJECT_BINARY_DIR}/${idl}.h ${PROJECT_BINARY_DIR}/${idl}_c.h)
		set(SKEL_STUB_SOURCE_FILES ${SKEL_STUB_SOURCE_FILES} ${PROJECT_BINARY_DIR}/${idl}.cc ${PROJECT_BINARY_DIR}/${idl}_c.cc)
	elseif(CORBA STREQUAL "omniORB")
		set(SKEL_STUB_HEADER_FILES ${SKEL_STUB_HEADER_FILES} ${PROJECT_BINARY_DIR}/${idl}.hh)
		set(SKEL_STUB_SOURCE_FILES ${SKEL_STUB_SOURCE_FILES} ${PROJECT_BINARY_DIR}/${idl}SK.cc ${PROJECT_BINARY_DIR}/${idl}DynSK.cc)
	elseif(CORBA STREQUAL "TAO")
		set(SKEL_STUB_HEADER_FILES ${SKEL_STUB_HEADER_FILES} ${PROJECT_BINARY_DIR}/${idl}S.h ${PROJECT_BINARY_DIR}/${idl}C.h ${PROJECT_BINARY_DIR}/${idl}C.inl)
		set(SKEL_STUB_SOURCE_FILES ${SKEL_STUB_SOURCE_FILES} ${PROJECT_BINARY_DIR}/${idl}S.cpp ${PROJECT_BINARY_DIR}/${idl}C.cpp)
	endif()
endforeach(idl)

install(FILES ${SKEL_STUB_HEADER_FILES} DESTINATION ${INSTALL_RTM_INCLUDE_DIR}/rtm/ext COMPONENT ext)