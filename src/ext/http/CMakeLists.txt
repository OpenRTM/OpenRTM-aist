cmake_minimum_required (VERSION 3.16)

if(POLICY CMP0144)
  cmake_policy(SET CMP0144 NEW)
endif()

project (HTTPTransport
	VERSION ${RTM_VERSION}
	LANGUAGES CXX)


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


set(target HTTPTransport)

set(srcs HTTPTransport.cpp )


set(libs ${RTM_PROJECT_NAME} ${ORB_LIBRARIES} ${OPENSSL_LIBRARIES} ${ORBSSL_LIBRARIES} ${ORBHTTP_LIBRARIES} ${DATATYPE_FACTORIES})

add_library(${target} SHARED ${srcs})
openrtm_set_link_props_shared(${target})
openrtm_common_set_compile_props(${target})
openrtm_include_rtm(${target})
target_include_directories(${target} SYSTEM
	PRIVATE ${OPENSSL_INCLUDE_DIR})
target_link_libraries(${target} PRIVATE ${libs} ${RTM_LINKER_OPTION})
set_target_properties(${target} PROPERTIES PREFIX "")


install(TARGETS ${target} LIBRARY DESTINATION ${INSTALL_RTM_EXT_DIR}/http
			RUNTIME DESTINATION ${INSTALL_RTM_EXT_DIR}/http
			COMPONENT ext)

set(INSTALL_HTTPTRANSPORT_DIR ${INSTALL_RTM_EXT_DIR}/http)

set(PROTOCOLS ${PROTOCOLS} http https ws wss)

if(BUILD_RTM_LINUX_PKGS)
	set(CMAKE_INSTALL_PREFIX "/usr")
endif()
foreach(PROTOCOL ${PROTOCOLS})
	configure_file(
		${CMAKE_CURRENT_SOURCE_DIR}/rtc.${PROTOCOL}.conf.in ${PROJECT_BINARY_DIR}/rtc.${PROTOCOL}.conf @ONLY)
	install(
		FILES ${PROJECT_BINARY_DIR}/rtc.${PROTOCOL}.conf
		DESTINATION ${INSTALL_RTM_ETC_DIR}/http
		COMPONENT ext)
endforeach()
