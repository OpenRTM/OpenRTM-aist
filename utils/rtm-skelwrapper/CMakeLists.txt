cmake_minimum_required (VERSION 3.5.1)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/rtm-skelwrapper.in ${OpenRTM_aist_BINARY_DIR}/rtm-skelwrapper @ONLY)
configure_file(skel_wrapper.py ${OpenRTM_aist_BINARY_DIR}/skel_wrapper.py COPYONLY)
configure_file(yat.py ${OpenRTM_aist_BINARY_DIR}/yat.py COPYONLY)

if(UNIX)
	install(FILES ${OpenRTM_aist_BINARY_DIR}/rtm-skelwrapper DESTINATION ${INSTALL_RTM_BIN_DIR} COMPONENT utils PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ GROUP_EXECUTE GROUP_READ WORLD_READ WORLD_EXECUTE)
	install(FILES skel_wrapper.py DESTINATION ${INSTALL_RTM_SCRIPT_DIR} COMPONENT utils)
	install(FILES yat.py DESTINATION ${INSTALL_RTM_SCRIPT_DIR} COMPONENT utils)
elseif(MSVC)
	configure_file(${OpenRTM_aist_BINARY_DIR}/rtm-skelwrapper ${OpenRTM_aist_BINARY_DIR}/rtm-skelwrapper.py COPYONLY)
	install(FILES ${OpenRTM_aist_BINARY_DIR}/rtm-skelwrapper.py DESTINATION ${INSTALL_RTM_SCRIPT_DIR} COMPONENT utils)
	install(FILES skel_wrapper.py DESTINATION ${INSTALL_RTM_SCRIPT_DIR} COMPONENT utils)
	install(FILES yat.py DESTINATION ${INSTALL_RTM_SCRIPT_DIR} COMPONENT utils)
endif(UNIX)

