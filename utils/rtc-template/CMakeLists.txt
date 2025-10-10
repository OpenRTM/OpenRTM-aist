cmake_minimum_required (VERSION 3.16)


if(UNIX)
    install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/rtc-template DESTINATION ${INSTALL_RTM_BIN_DIR} COMPONENT utils)
elseif(MSVC)
    install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/rtc-template DESTINATION ${INSTALL_RTM_SCRIPT_DIR} COMPONENT utils)
endif(UNIX)