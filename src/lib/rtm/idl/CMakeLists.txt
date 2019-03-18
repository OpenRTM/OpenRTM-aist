cmake_minimum_required (VERSION 3.0.2)

project (${RTCSKEL_PROJECT_NAME}
	VERSION ${RTM_VERSION}
	LANGUAGES CXX)

set(${PROJECT_NAME}_IDLS
	BasicDataType.idl
	DataPort.idl
	DataPort_OpenRTM.idl
	ExtendedDataTypes.idl
	InterfaceDataTypes.idl
	Manager.idl
	OpenRTM.idl
	RTC.idl
	SDOPackage.idl
	SharedMemory.idl
	ExtendedFsmService.idl)

if(CORBA MATCHES "TAO")
	set(${PROJECT_NAME}_IDLS ${${PROJECT_NAME}_IDLS} DataPortUDP.idl)
endif()

# .idl -> .cpp
openrtm_idl_compile_all(${PROJECT_NAME} "${${PROJECT_NAME}_IDLS}" "-I${PROJECT_SOURCE_DIR}")

# .cpp -> .o .so
link_directories(${ORB_LINK_DIR})

add_library(${PROJECT_NAME} STATIC ${${PROJECT_NAME}_IDLSKEL})
openrtm_gencode_set_compile_props(${PROJECT_NAME})
target_include_directories(${PROJECT_NAME} SYSTEM
	PUBLIC ${PROJECT_BINARY_DIR}
	PRIVATE ${CMAKE_BINARY_DIR}/src/lib)
add_dependencies(${PROJECT_NAME} ${PROJECT_NAME}_IDLTGT)

add_library(IDL_OBJLIB OBJECT ${${PROJECT_NAME}_IDLGEN})
openrtm_gencode_set_compile_props(IDL_OBJLIB)
target_include_directories(IDL_OBJLIB SYSTEM
	PUBLIC ${PROJECT_BINARY_DIR})
add_dependencies(IDL_OBJLIB ${PROJECT_NAME}_IDLTGT)

# install .idl .h .so
if(UNIX)
	install(TARGETS ${PROJECT_NAME} LIBRARY DESTINATION ${INSTALL_RTM_LIB_DIR}
					ARCHIVE DESTINATION ${INSTALL_RTM_LIB_DIR}
					RUNTIME DESTINATION ${INSTALL_RTM_BIN_DIR}
					COMPONENT openrtm)
endif()
install(FILES ${${PROJECT_NAME}_IDLS} DESTINATION ${INSTALL_RTM_INCLUDE_DIR}/rtm/idl COMPONENT idl)
foreach(hdr ${${PROJECT_NAME}_IDLHDR})
	install(FILES "${PROJECT_BINARY_DIR}/${hdr}" DESTINATION ${INSTALL_RTM_INCLUDE_DIR}/rtm/idl COMPONENT headers)
endforeach()

add_subdirectory(device_interfaces)
