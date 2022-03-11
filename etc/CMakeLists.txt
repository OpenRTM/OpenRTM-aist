cmake_minimum_required (VERSION 3.5.1)



install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/rtc.conf.sample DESTINATION ${INSTALL_RTM_ETC_DIR} COMPONENT ext)

if(CORBA STREQUAL "TAO")
	install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/tao_diop/svc.conf DESTINATION ${INSTALL_RTM_ETC_DIR}/tao_diop COMPONENT ext)

	configure_file(
		${CMAKE_CURRENT_SOURCE_DIR}/tao_diop/rtc.diop.conf.in ${PROJECT_BINARY_DIR}/rtc.diop.conf @ONLY)
	install(
		FILES ${PROJECT_BINARY_DIR}/rtc.diop.conf
		DESTINATION ${INSTALL_RTM_ETC_DIR}/tao_diop
		COMPONENT ext)



	install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/tao_htiop/svc.conf DESTINATION ${INSTALL_RTM_ETC_DIR}/tao_htiop COMPONENT ext)
	install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/tao_htiop/HT_Config.conf DESTINATION ${INSTALL_RTM_ETC_DIR}/tao_htiop COMPONENT ext)

	configure_file(
		${CMAKE_CURRENT_SOURCE_DIR}/tao_htiop/rtc.htiop.conf.in ${PROJECT_BINARY_DIR}/rtc.htiop.conf @ONLY)
	install(
		FILES ${PROJECT_BINARY_DIR}/rtc.htiop.conf
		DESTINATION ${INSTALL_RTM_ETC_DIR}/tao_htiop
		COMPONENT ext)



	configure_file(
		${CMAKE_CURRENT_SOURCE_DIR}/tao_ssliop/rtc.ssliop.conf.in ${PROJECT_BINARY_DIR}/rtc.ssliop.conf @ONLY)
	install(
		FILES ${PROJECT_BINARY_DIR}/rtc.ssliop.conf
		DESTINATION ${INSTALL_RTM_ETC_DIR}/tao_ssliop
		COMPONENT ext)

	configure_file(
		${CMAKE_CURRENT_SOURCE_DIR}/tao_ssliop/svc.conf.in ${PROJECT_BINARY_DIR}/svc.conf @ONLY)
	install(
		FILES ${PROJECT_BINARY_DIR}/svc.conf
		DESTINATION ${INSTALL_RTM_ETC_DIR}/tao_ssliop
		COMPONENT ext)


	install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/tao_shmiop/svc.conf DESTINATION ${INSTALL_RTM_ETC_DIR}/tao_shmiop COMPONENT ext)

	configure_file(
		${CMAKE_CURRENT_SOURCE_DIR}/tao_shmiop/rtc.shmiop.conf.in ${PROJECT_BINARY_DIR}/rtc.shmiop.conf @ONLY)
	install(
		FILES ${PROJECT_BINARY_DIR}/rtc.shmiop.conf
		DESTINATION ${INSTALL_RTM_ETC_DIR}/tao_shmiop
		COMPONENT ext)

endif()
