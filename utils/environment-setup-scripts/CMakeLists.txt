cmake_minimum_required (VERSION 3.16)
set(target environment-setup-scripts)
project (${target}
	VERSION ${RTM_VERSION}
	LANGUAGES CXX)



function(generate_script_win filename)
	configure_file(
		${CMAKE_CURRENT_SOURCE_DIR}/${filename}.bat.in ${PROJECT_BINARY_DIR}/${filename}.${RTM_VC_VER}.bat @ONLY)
	install(
		FILES ${PROJECT_BINARY_DIR}/${filename}.${RTM_VC_VER}.bat
		DESTINATION ${INSTALL_RTM_ETC_DIR}
		COMPONENT utils)
endfunction()

function(generate_script_linux filename)
	configure_file(
		${CMAKE_CURRENT_SOURCE_DIR}/${filename}.sh.in ${PROJECT_BINARY_DIR}/${filename}.sh @ONLY)
	install(
		FILES ${PROJECT_BINARY_DIR}/${filename}.sh
		DESTINATION ${INSTALL_RTM_ETC_DIR}
		PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE 
		COMPONENT utils)
endfunction()


if(WIN32)
	if(CORBA STREQUAL "omniORB")
		generate_script_win(environment-setup.omniorb)
	elseif(CORBA STREQUAL "TAO")
		generate_script_win(environment-setup.tao)
	endif()
else(WIN32)
	generate_script_linux(environment-setup)
endif(WIN32)
