cmake_minimum_required (VERSION 3.5.1)

if(UNIX)
	configure_file(${CMAKE_CURRENT_SOURCE_DIR}/rtc.conf.sample ${CMAKE_CURRENT_SOURCE_DIR}/rtc.conf.sample2 COPYONLY)
	install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/rtc.conf.sample2 DESTINATION ${INSTALL_RTM_ETC_DIR} COMPONENT ext)
else()
	install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/rtc.conf.sample DESTINATION ${INSTALL_RTM_ETC_DIR} COMPONENT ext)
endif()

if(CORBA STREQUAL "TAO")
	install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/tao_udp/svc.conf DESTINATION ${INSTALL_RTM_ETC_DIR}/tao_udp COMPONENT ext)
endif()
