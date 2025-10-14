cmake_minimum_required (VERSION 3.16)

if(POLICY CMP0144)
  cmake_policy(SET CMP0144 NEW)
endif()

project (SSLTransport
	VERSION ${RTM_VERSION}
	LANGUAGES CXX)


set(SSL_FILES ${CMAKE_CURRENT_SOURCE_DIR}/root.crt ${CMAKE_CURRENT_SOURCE_DIR}/server.pem)
install(FILES ${SSL_FILES} DESTINATION ${INSTALL_RTM_ETC_DIR}/ssl COMPONENT ext)

if(NOT CORBA STREQUAL "omniORB")
  return()
endif()


set(OPENSSL_ROOT ${OPENSSL_ROOT} CACHE PATH "set OPENSSL_ROOT")

if(OPENSSL_ROOT)
	set(OPENSSL_ROOT_DIR ${OPENSSL_ROOT})
endif(OPENSSL_ROOT)

find_package(OpenSSL REQUIRED)

link_directories(${ORB_LINK_DIR} ${SSL_LINK_DIR})
add_definitions(${ORB_C_FLAGS_LIST})
add_definitions(${COIL_C_FLAGS_LIST})
if(WIN32)
	add_definitions(-DRTM_SKEL_IMPORT_SYMBOL)
endif()


set(target SSLTransport)

set(srcs SSLTransport.cpp)


set(libs ${RTM_PROJECT_NAME} ${ORB_LIBRARIES} ${OPENSSL_LIBRARIES} ${ORBSSL_LIBRARIES} ${DATATYPE_FACTORIES})

add_library(${target} SHARED ${srcs})
openrtm_set_link_props_shared(${target})
openrtm_common_set_compile_props(${target})
openrtm_include_rtm(${target})
target_include_directories(${target} SYSTEM
	PRIVATE ${OPENSSL_INCLUDE_DIR})
target_link_libraries(${target} PRIVATE ${libs} ${RTM_LINKER_OPTION})
set_target_properties(${target} PROPERTIES PREFIX "")


install(TARGETS ${target} LIBRARY DESTINATION ${INSTALL_RTM_EXT_DIR}/ssl
			RUNTIME DESTINATION ${INSTALL_RTM_EXT_DIR}/ssl
			COMPONENT ext)

if(WIN32)
	if(OPENSSL_ROOT_DIR)
		file(GLOB OPENSSL_DLLS1 "${OPENSSL_ROOT_DIR}/bin/libcrypto*.dll")
		file(GLOB OPENSSL_DLLS2 "${OPENSSL_ROOT_DIR}/bin/libssl*.dll")
		install(PROGRAMS ${OPENSSL_ROOT_DIR}/bin/openssl.exe DESTINATION ${INSTALL_RTM_EXT_DIR}/ssl COMPONENT ssllibs)
	endif(OPENSSL_ROOT_DIR)
	foreach(library ${OPENSSL_DLLS1};${OPENSSL_DLLS2})
		install(PROGRAMS ${library} DESTINATION ${INSTALL_RTM_BIN_DIR} COMPONENT ssllibs)
	endforeach()
	
else(WIN32)
endif(WIN32)


set(INSTALL_SSLTRANSPORT_DIR ${INSTALL_RTM_EXT_DIR}/ssl)
if(BUILD_RTM_LINUX_PKGS)
	set(CMAKE_INSTALL_PREFIX "/usr")
endif()
configure_file(
	${CMAKE_CURRENT_SOURCE_DIR}/rtc.ssl.conf.in ${PROJECT_BINARY_DIR}/rtc.ssl.conf @ONLY)
install(
	FILES ${PROJECT_BINARY_DIR}/rtc.ssl.conf
	DESTINATION ${INSTALL_RTM_ETC_DIR}/ssl
	COMPONENT ext)
