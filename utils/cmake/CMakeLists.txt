cmake_minimum_required (VERSION 3.5.1)

project (OpenRTMCMake
	VERSION 2.0.2
	LANGUAGES CXX)

if(CORBA STREQUAL "ORBexpress")
elseif(CORBA STREQUAL "omniORB")
	set(OMNIORB_VERSION "${OMNI_VERSION1}.${OMNI_VERSION2}.${OMNI_MINOR}")
elseif(CORBA STREQUAL "TAO")
endif()

set(RTM_C_FLAGS_LIST ${ORB_C_FLAGS_LIST} ${COIL_C_FLAGS_LIST})




if(WIN32)
	set(RTM_C_FLAGS_LIST ${RTM_C_FLAGS_LIST} -DRTM_IMPORT_SYMBOL -DEXT_RTM_IMPORT_SYMBOL -DRTM_SKEL_IMPORT_SYMBOL)
	set(RTC ${RTM_PROJECT_NAME}${RTM_SHORT_VERSION}_${RTM_VC_VER}_${CMAKE_VS_PLATFORM_NAME})
	set(rtmCamera rtmCamera${RTM_SHORT_VERSION}_${RTM_VC_VER}_${CMAKE_VS_PLATFORM_NAME})
	set(rtmManipulator rtmManipulator${RTM_SHORT_VERSION}_${RTM_VC_VER}_${CMAKE_VS_PLATFORM_NAME})
	set(OPENRTM_LIBRARIES "optimized;${RTC};debug;${RTC}d;${RTM_LINKER_OPTION};optimized;${rtmCamera};debug;${rtmCamera}d;optimized;${rtmManipulator};debug;${rtmManipulator}d")
	set(OPENRTM_LOCAL_LIBRARIES "optimized;${RTC};debug;${RTC}d;${RTM_LINKER_OPTION}")
	foreach(data_type ${DATATYPE_FACTORIES})
		set(DATATYPE ${data_type}${RTM_SHORT_VERSION}_${RTM_VC_VER}_${CMAKE_VS_PLATFORM_NAME})
		set(OPENRTM_LIBRARIES ${OPENRTM_LIBRARIES};optimized;${DATATYPE};debug;${DATATYPE}d)
		set(OPENRTM_LOCAL_LIBRARIES ${OPENRTM_LOCAL_LIBRARIES};optimized;${DATATYPE};debug;${DATATYPE}d)
	endforeach()
else(WIN32)
	set(OPENRTM_LIBRARIES "${RTM_PROJECT_NAME};${ORB_LIBRARIES};rtmCamera2;rtmManipulator2")
endif(WIN32)

if(BUILD_RTM_LINUX_PKGS)
	set(CMAKE_INSTALL_PREFIX "/usr")
endif()


if(VXWORKS)
elseif(UNIX)
	if(CORBA STREQUAL "ORBexpress")
	elseif(CORBA STREQUAL "omniORB")
		configure_file(${CMAKE_CURRENT_SOURCE_DIR}/OpenRTMConfig_omniORB_Linux.cmake.in ${PROJECT_BINARY_DIR}/cmake/OpenRTMConfig.cmake @ONLY)
		install(FILES ${PROJECT_BINARY_DIR}/cmake/OpenRTMConfig.cmake DESTINATION ${INSTALL_RTM_CMAKE_DIR} COMPONENT cmakefiles)
	elseif(CORBA STREQUAL "TAO")
		configure_file(${CMAKE_CURRENT_SOURCE_DIR}/OpenRTMConfig_TAO_Linux.cmake.in ${PROJECT_BINARY_DIR}/cmake/OpenRTMConfig.cmake @ONLY)
		install(FILES ${PROJECT_BINARY_DIR}/cmake/OpenRTMConfig.cmake DESTINATION ${INSTALL_RTM_CMAKE_DIR} COMPONENT cmakefiles)
	elseif(CORBA STREQUAL "RtORB")
		foreach(data_type ${DATATYPE_FACTORIES})
			set(OPENRTM_LIBRARIES ${OPENRTM_LIBRARIES};${data_type})
		endforeach()
		configure_file(${CMAKE_CURRENT_SOURCE_DIR}/OpenRTMConfig_RtORB_Linux.cmake.in ${PROJECT_BINARY_DIR}/cmake/OpenRTMConfig.cmake @ONLY)
		install(FILES ${PROJECT_BINARY_DIR}/cmake/OpenRTMConfig.cmake DESTINATION ${INSTALL_RTM_CMAKE_DIR} COMPONENT cmakefiles)
	endif()
	
elseif(MSVC)
	configure_file(${CMAKE_CURRENT_SOURCE_DIR}/OpenRTMConfig.cmake.in ${PROJECT_BINARY_DIR}/OpenRTMConfig.cmake @ONLY)
	install(FILES  ${PROJECT_BINARY_DIR}/OpenRTMConfig.cmake DESTINATION ${INSTALL_RTM_CMAKE_DIR} COMPONENT cmakefiles)
	if(CORBA STREQUAL "ORBexpress")
	elseif(CORBA STREQUAL "omniORB")
		configure_file(${CMAKE_CURRENT_SOURCE_DIR}/OpenRTMConfig_omniORB.cmake.in ${PROJECT_BINARY_DIR}/cmake/OpenRTMConfig.cmake @ONLY)
		install(FILES  ${PROJECT_BINARY_DIR}/cmake/OpenRTMConfig.cmake DESTINATION ${INSTALL_RTM_BIN_DIR} COMPONENT cmakefiles)
	elseif(CORBA STREQUAL "TAO")
		configure_file(${CMAKE_CURRENT_SOURCE_DIR}/OpenRTMConfig_TAO.cmake.in ${PROJECT_BINARY_DIR}/cmake/OpenRTMConfig.cmake @ONLY)
		install(FILES  ${PROJECT_BINARY_DIR}/cmake/OpenRTMConfig.cmake DESTINATION ${INSTALL_RTM_BIN_DIR} COMPONENT cmakefiles)
	endif()
endif()


configure_file(${CMAKE_CURRENT_SOURCE_DIR}/OpenRTMConfigVersion.cmake.in ${PROJECT_BINARY_DIR}/cmake/OpenRTMConfigVersion.cmake @ONLY)
install(FILES ${PROJECT_BINARY_DIR}/cmake/OpenRTMConfigVersion.cmake DESTINATION ${INSTALL_RTM_CMAKE_DIR} COMPONENT cmakefiles)

