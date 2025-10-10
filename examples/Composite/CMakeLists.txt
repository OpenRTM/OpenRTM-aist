cmake_minimum_required (VERSION 3.16)

project (Composite
	VERSION ${RTM_VERSION}
	LANGUAGES CXX)


link_directories(${ORB_LINK_DIR})
add_definitions(${ORB_C_FLAGS_LIST})
add_definitions(${COIL_C_FLAGS_LIST})
if(WIN32)
	add_definitions(-DRTM_SKEL_IMPORT_SYMBOL)
endif()


set(target CompositeComp)

set(srcs Composite.cpp)
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


if(VXWORKS)
	if(RTP)
		set_target_properties(${target} PROPERTIES SUFFIX ".vxe")
	else(RTP)	
		set_target_properties(${target} PROPERTIES SUFFIX ".out")
	endif(RTP)
endif(VXWORKS)


install(FILES composite.conf DESTINATION ${INSTALL_RTM_EXAMPLE_DIR} COMPONENT examples)
if(UNIX)
	install(FILES rtc.composite.linux.conf DESTINATION ${INSTALL_RTM_EXAMPLE_DIR} COMPONENT examples)
elseif(WIN32)
	install(FILES rtc_win32.conf DESTINATION ${INSTALL_RTM_EXAMPLE_DIR} COMPONENT examples)
endif()
