cmake_minimum_required (VERSION 3.5.1)
set(target openrtmNames)
project (${target}
	VERSION ${RTM_VERSION}
	LANGUAGES CXX)





link_directories(${ORB_LINK_DIR})
add_definitions(${ORB_C_FLAGS_LIST})
add_definitions(${COIL_C_FLAGS_LIST})
if(WIN32)
	add_definitions(-DRTM_SKEL_IMPORT_SYMBOL)
endif()

add_executable(${target} openrtmNames.cpp NamingContext.h NamingContext.cpp
				BindingIterator.h BindingIterator.cpp ObjectBinding.h ObjectBinding.cpp
				RTObjectBinding.h RTObjectBinding.cpp ManagerBinding.h ManagerBinding.cpp)

openrtm_common_set_compile_props(${target})
openrtm_set_link_props_shared(${target})
openrtm_include_rtm(${target})

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
else(WIN32)
	install(TARGETS ${target} LIBRARY DESTINATION ${INSTALL_RTM_LIB_DIR}
			ARCHIVE DESTINATION ${INSTALL_RTM_LIB_DIR}
			RUNTIME DESTINATION ${INSTALL_RTM_BIN_DIR}
			COMPONENT utils)
endif(WIN32)

if(SSL_ENABLE)

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

