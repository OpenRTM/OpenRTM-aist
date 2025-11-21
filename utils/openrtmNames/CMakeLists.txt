cmake_minimum_required (VERSION 3.16)

if(POLICY CMP0144)
  cmake_policy(SET CMP0144 NEW)
endif()

set(target openrtmNames)
set(targetDLL OpenrtmNamesPlugin)
project (${target}
	VERSION ${RTM_VERSION}
	LANGUAGES CXX)





link_directories(${ORB_LINK_DIR})
add_definitions(${ORB_C_FLAGS_LIST})
add_definitions(${COIL_C_FLAGS_LIST})
if(WIN32)
	add_definitions(-DRTM_SKEL_IMPORT_SYMBOL)
endif()

set(OPENRTMNAMES_FILES NamingContext.h NamingContext.cpp
		BindingIterator.h BindingIterator.cpp ObjectBinding.h ObjectBinding.cpp
		RTObjectBinding.h RTObjectBinding.cpp ManagerBinding.h ManagerBinding.cpp)

add_library(${target}_objlib OBJECT ${OPENRTMNAMES_FILES})
openrtm_common_set_compile_props(${target}_objlib)
openrtm_set_link_props_shared(${target}_objlib)
openrtm_include_rtm(${target}_objlib)

add_executable(${target} openrtmNames.cpp $<TARGET_OBJECTS:${target}_objlib>)
openrtm_common_set_compile_props(${target})
openrtm_set_link_props_shared(${target})
openrtm_include_rtm(${target})

add_library(${targetDLL} SHARED OpenrtmNamesPlugin.cpp OpenrtmNamesPlugin.h $<TARGET_OBJECTS:${target}_objlib>)
set_target_properties(${targetDLL} PROPERTIES PREFIX "")
openrtm_common_set_compile_props(${targetDLL})
openrtm_set_link_props_shared(${targetDLL})
openrtm_include_rtm(${targetDLL})

if(NOT WIN32)
	add_library(${targetDLL}-static STATIC OpenrtmNamesPlugin.cpp OpenrtmNamesPlugin.h $<TARGET_OBJECTS:${target}_objlib>)
	set_target_properties(${targetDLL}-static PROPERTIES OUTPUT_NAME ${targetDLL} CLEAN_DIRECT_OUTPUT 1)
	openrtm_common_set_compile_props(${targetDLL}-static)
	openrtm_set_link_props_shared(${targetDLL}-static)
	openrtm_include_rtm(${targetDLL}-static)
endif()


set(libs ${RTM_PROJECT_NAME} ${ORB_LIBRARIES} ${DATATYPE_FACTORIES})
if(CORBA STREQUAL "TAO")
	if(VXWORKS)
		set(libs ${libs} TAO_CosNaming_Skel)
	elseif(UNIX)
		set(libs ${libs} TAO_CosNaming_Skel)
	elseif(MSVC)
		set(libs ${libs} optimized;TAO_CosNaming_Skel;debug;TAO_CosNaming_Skeld)
	endif()
endif()

if(SSL_ENABLE)
	set(OPENSSL_ROOT ${OPENSSL_ROOT} CACHE PATH "set OPENSSL_ROOT")
	if(OPENSSL_ROOT)
		set(OPENSSL_ROOT_DIR ${OPENSSL_ROOT})
	endif(OPENSSL_ROOT)

	find_package(OpenSSL REQUIRED)

	target_compile_options(${target} PRIVATE -DSSL_ENABLE)
	target_include_directories(${target} SYSTEM
							PRIVATE ${OPENSSL_INCLUDE_DIR})
	set(libs ${libs} ${OPENSSL_LIBRARIES} ${ORBSSL_LIBRARIES})
	if(HTTP_ENABLE)
		target_compile_options(${target} PRIVATE -DHTTP_ENABLE)
		set(libs ${libs} ${ORBHTTP_LIBRARIES})
	endif(HTTP_ENABLE)
endif(SSL_ENABLE)

target_link_libraries(${target} ${libs} ${RTM_LINKER_OPTION})

target_link_libraries(${targetDLL} ${libs} ${RTM_LINKER_OPTION})



if(VXWORKS)
	if(RTP)
		set_target_properties(${target} PROPERTIES SUFFIX ".vxe")
	else(RTP)	
		set_target_properties(${target} PROPERTIES SUFFIX ".out")
	endif(RTP)
endif(VXWORKS)

if(WIN32)
	install(TARGETS ${target} LIBRARY DESTINATION ${INSTALL_RTM_LIB_DIR}
			ARCHIVE DESTINATION ${INSTALL_RTM_LIB_DIR}
			RUNTIME DESTINATION ${INSTALL_RTM_LIB_DIR}
			COMPONENT utils)
	install(TARGETS ${targetDLL} LIBRARY DESTINATION ${INSTALL_RTM_EXT_DIR}/naming
			ARCHIVE DESTINATION ${INSTALL_RTM_EXT_DIR}/naming
			RUNTIME DESTINATION ${INSTALL_RTM_EXT_DIR}/naming
			COMPONENT utils)
else(WIN32)
	install(TARGETS ${target} LIBRARY DESTINATION ${INSTALL_RTM_LIB_DIR}
			ARCHIVE DESTINATION ${INSTALL_RTM_LIB_DIR}
			RUNTIME DESTINATION ${INSTALL_RTM_BIN_DIR}
			COMPONENT utils)

	install(TARGETS ${targetDLL} LIBRARY DESTINATION ${INSTALL_RTM_EXT_DIR}/naming
			ARCHIVE DESTINATION ${INSTALL_RTM_EXT_DIR}/naming
			RUNTIME DESTINATION ${INSTALL_RTM_EXT_DIR}/naming
			COMPONENT utils)

	install(TARGETS ${targetDLL}-static LIBRARY DESTINATION ${INSTALL_RTM_EXT_DIR}/naming
			ARCHIVE DESTINATION ${INSTALL_RTM_EXT_DIR}/naming
			RUNTIME DESTINATION ${INSTALL_RTM_EXT_DIR}/naming
			COMPONENT utils)
	install(FILES  OpenrtmNamesPlugin.h DESTINATION ${INSTALL_RTM_EXT_DIR}/naming COMPONENT headers)
endif(WIN32)

if(SSL_ENABLE)
	if(BUILD_RTM_LINUX_PKGS)
		set(CMAKE_INSTALL_PREFIX "/usr")
	endif()
	if(CORBA STREQUAL "omniORB")
		set(PROTOCOLS tcp ssl)

		if(HTTP_ENABLE)
			set(PROTOCOLS ${PROTOCOLS} http https ws wss)
		endif()

		foreach(PROTOCOL ${PROTOCOLS})
			configure_file(
				${CMAKE_CURRENT_SOURCE_DIR}/rtc.names.${PROTOCOL}.omniorb.conf.in ${PROJECT_BINARY_DIR}/rtc.names.${PROTOCOL}.conf @ONLY)
			install(
				FILES ${PROJECT_BINARY_DIR}/rtc.names.${PROTOCOL}.conf
				DESTINATION ${INSTALL_RTM_ETC_DIR}
				COMPONENT utils)
		endforeach()
	elseif(CORBA STREQUAL "TAO")
		set(PROTOCOLS tcp http shm)

		if(SSL_ENABLE)
			set(PROTOCOLS ${PROTOCOLS} ssl)
		endif()

		foreach(PROTOCOL ${PROTOCOLS})
			configure_file(
				${CMAKE_CURRENT_SOURCE_DIR}/rtc.names.${PROTOCOL}.tao.conf.in ${PROJECT_BINARY_DIR}/rtc.names.${PROTOCOL}.conf @ONLY)
			install(
				FILES ${PROJECT_BINARY_DIR}/rtc.names.${PROTOCOL}.conf
				DESTINATION ${INSTALL_RTM_ETC_DIR}
				COMPONENT utils)
		endforeach()
	endif()
endif()

