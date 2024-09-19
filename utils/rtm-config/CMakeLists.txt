cmake_minimum_required (VERSION 3.5.1)

# Variable definitions
set(INSTALL_INCLUDE_DIR include)
set(INSTALL_RTM_IDL_DIR ${INSTALL_RTM_INCLUDE_DIR}/rtm/idl)
set(INSTALL_DATA_DIR openrtm-${RTM_MAJOR_VERSION}.${RTM_MINOR_VERSION})
set(INSTALL_RTM_IDL_DIR ${INSTALL_RTM_INCLUDE_DIR}/rtm/idl)
set(INSTALL_RTC_DIR ${INSTALL_RTM_EXT_DIR}/rtc)
set(INSTALL_EC_DIR ${INSTALL_RTM_EXT_DIR}/ec)
set(INSTALL_RTM_DIR ${INSTALL_RTM_EXT_DIR}/rtm)
set(INSTALL_SVC_DIR ${INSTALL_RTM_EXT_DIR}/rtm)
set(PACKAGE OpenRTM-aist)
set(RTM_DESCRIPTION An OMG RTC compliant middleware for robots.)
set(RTM_LIBS ${RTM_PROJECT_NAME})
if(CORBA STREQUAL "ORBexpress")
elseif(CORBA STREQUAL "omniORB")
	set(IDLCOMMAND_FLAGS "-bcxx -Wba -nf -Wbshortcut")
elseif(CORBA STREQUAL "TAO")
	if(WIN32)
		set(IDLCOMMAND_FLAGS -as -DTAO_IDL -I${OPENRTM_DIR}/ACE/${RTM_VC_VER}/TAO)
	else()
		set(IDLCOMMAND_FLAGS -as -DTAO_IDL -I${ORB_ROOT}/TAO)
	endif()
endif()

string(REPLACE ";" " " IDL_FLAGS "${IDLCOMMAND_FLAGS}")

# Other definitions
set(TMPLIBS -lpthread ${RTM_LINKER_OPTION})
foreach(library ${ORB_LIBRARIES})
	set(TMPLIBS ${TMPLIBS} -l${library})
endforeach()
set(TMPLIBS ${TMPLIBS} -l${RTM_PROJECT_NAME} -lrtmCamera -lrtmManipulator)
string(REPLACE ";" " " LIBS "${TMPLIBS}")

#------------------------------
# Configure rtm-config
if(BUILD_RTM_LINUX_PKGS)
  set(CMAKE_INSTALL_PREFIX "/usr")
endif()

configure_file(
	${CMAKE_CURRENT_SOURCE_DIR}/rtm-config.in
	${PROJECT_BINARY_DIR}/rtm2-config @ONLY)

if(NOT WIN32)
	install(
		FILES ${PROJECT_BINARY_DIR}/rtm2-config
		DESTINATION ${INSTALL_RTM_BIN_DIR}
		COMPONENT utils
		PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ GROUP_EXECUTE GROUP_READ  WORLD_READ WORLD_EXECUTE)
else()
	install(
		FILES ${PROJECT_BINARY_DIR}/rtm2-config
		DESTINATION ${INSTALL_RTM_BIN_DIR} COMPONENT utils)
endif()

#------------------------------
# Configure openrtm-aist.pc
configure_file(
	${CMAKE_CURRENT_SOURCE_DIR}/openrtm-aist.pc.in ${PROJECT_BINARY_DIR}/openrtm2.pc @ONLY)
install(
	FILES ${PROJECT_BINARY_DIR}/openrtm2.pc
	DESTINATION ${INSTALL_RTM_LIB_DIR}/pkgconfig
	COMPONENT utils)
