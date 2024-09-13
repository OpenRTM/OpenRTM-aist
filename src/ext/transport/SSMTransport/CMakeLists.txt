cmake_minimum_required (VERSION 3.5.1)

project (SSMTransport
	VERSION ${RTM_VERSION}
	LANGUAGES CXX)

set(target SSMTransport)
set(srcs SSMTransport.cpp SSMTransport.h SSMInPort.cpp SSMInPort.h SSMOutPort.cpp SSMOutPort.h)


if(OpenRTM_aist_BINARY_DIR)

	link_directories(${ORB_LINK_DIR})
	add_definitions(${ORB_C_FLAGS_LIST})

	if(WIN32)
		add_definitions(-DRTM_SKEL_IMPORT_SYMBOL)
		add_definitions(-DNOGDI)
		add_definitions(-DNOMINMAX)
		add_definitions(-DTRANSPORT_PLUGIN)
	endif()


	if(VXWORKS AND NOT RTP)
		set(libs ${RTCSKEL_PROJECT_NAME})

		add_executable(${target} ${srcs})
		openrtm_common_set_compile_props(${target})
		openrtm_set_link_props_shared(${target})
		openrtm_include_rtm(${target})
		target_link_libraries(${target} ${libs} ssm)

		install(TARGETS ${target} LIBRARY DESTINATION ${INSTALL_RTM_EXT_DIR}/transport
					ARCHIVE DESTINATION ${INSTALL_RTM_EXT_DIR}/transport
					RUNTIME DESTINATION ${INSTALL_RTM_EXT_DIR}/transport
					COMPONENT ext)
	else()
		set(libs ${RTM_PROJECT_NAME} ${ORB_LIBRARIES} ${DATATYPE_FACTORIES})


		add_library(${target} SHARED ${srcs})
		openrtm_common_set_compile_props(${target})
		openrtm_include_rtm(${target})
		openrtm_set_link_props_shared(${target})
		target_link_libraries(${target} PRIVATE ${libs} ${RTM_LINKER_OPTION} ssm)
		set_target_properties(${target} PROPERTIES PREFIX "")

		set_target_properties(${target} PROPERTIES
					CXX_STANDARD 11
					CXX_STANDARD_REQUIRED YES
					CXX_EXTENSIONS NO
					)

		install(TARGETS ${target} LIBRARY DESTINATION ${INSTALL_RTM_EXT_DIR}/transport
					ARCHIVE DESTINATION ${INSTALL_RTM_EXT_DIR}/transport
					RUNTIME DESTINATION ${INSTALL_RTM_EXT_DIR}/transport
					COMPONENT ext)
	endif()

else(OpenRTM_aist_BINARY_DIR)

	find_package(OpenRTM REQUIRED)

	if(${OPENRTM_VERSION_MAJOR} LESS 2)
		set(OPENRTM_CFLAGS ${OPENRTM_CFLAGS} ${OMNIORB_CFLAGS})
		set(OPENRTM_INCLUDE_DIRS ${OPENRTM_INCLUDE_DIRS} ${OMNIORB_INCLUDE_DIRS})
		set(OPENRTM_LIBRARY_DIRS ${OPENRTM_LIBRARY_DIRS} ${OMNIORB_LIBRARY_DIRS})
	else()
		set(CMAKE_CXX_STANDARD 11)
	endif()

	if (DEFINED OPENRTM_INCLUDE_DIRS)
		string(REGEX REPLACE "-I" ";"
			OPENRTM_INCLUDE_DIRS "${OPENRTM_INCLUDE_DIRS}")
		string(REGEX REPLACE " ;" ";"
			OPENRTM_INCLUDE_DIRS "${OPENRTM_INCLUDE_DIRS}")
	endif (DEFINED OPENRTM_INCLUDE_DIRS)

	if (DEFINED OPENRTM_LIBRARY_DIRS)
		string(REGEX REPLACE "-L" ";"
			OPENRTM_LIBRARY_DIRS "${OPENRTM_LIBRARY_DIRS}")
		string(REGEX REPLACE " ;" ";"
		OPENRTM_LIBRARY_DIRS "${OPENRTM_LIBRARY_DIRS}")
	endif (DEFINED OPENRTM_LIBRARY_DIRS)

	if (DEFINED OPENRTM_LIBRARIES)
		string(REGEX REPLACE "-l" ";"
			OPENRTM_LIBRARIES "${OPENRTM_LIBRARIES}")
		string(REGEX REPLACE " ;" ";"
			OPENRTM_LIBRARIES "${OPENRTM_LIBRARIES}")
	endif (DEFINED OPENRTM_LIBRARIES)


	if(WIN32)
		add_definitions(-DRTM_SKEL_IMPORT_SYMBOL)
		add_definitions(-DNOGDI)
		add_definitions(-DNOMINMAX)
		add_definitions(-DTRANSPORT_PLUGIN)
	endif()

	include_directories(${OPENRTM_INCLUDE_DIRS})
	add_definitions(${OPENRTM_CFLAGS})
	link_directories(${OPENRTM_LIBRARY_DIRS})

	if(VXWORKS AND NOT RTP)
		set(libs ${RTCSKEL_PROJECT_NAME})

		add_executable(${target} ${srcs})
		target_link_libraries(${target} ${libs} ssm ${OPENRTM_LIBRARIES})

		set(SSM_TRANSPORT_INSTALL_DIR lib/openrtm-${OPENRTM_VERSION_MAJOR}.${OPENRTM_VERSION_MINOR}/transport)

		install(TARGETS ${target} LIBRARY DESTINATION ${SSM_TRANSPORT_INSTALL_DIR}
					ARCHIVE DESTINATION ${SSM_TRANSPORT_INSTALL_DIR}
					RUNTIME DESTINATION ${SSM_TRANSPORT_INSTALL_DIR}
					COMPONENT ext)
	else()
		set(libs ${RTM_PROJECT_NAME} ${ORB_LIBRARIES} ${DATATYPE_FACTORIES})


		add_library(${target} SHARED ${srcs})
		target_link_libraries(${target} PRIVATE ${libs} ${RTM_LINKER_OPTION} ssm ${OPENRTM_LIBRARIES})
		set_target_properties(${target} PROPERTIES PREFIX "")

		set_target_properties(${target} PROPERTIES
					CXX_STANDARD 11
					CXX_STANDARD_REQUIRED YES
					CXX_EXTENSIONS NO
					)
		if(WIN32)
			set(SSM_TRANSPORT_INSTALL_DIR ${OPENRTM_DIR}/ext/${RTM_VC_VER}/transport)
		else(WIN32)
			include(GNUInstallDirs)
			set(CMAKE_INSTALL_LIBDIR ${CMAKE_INSTALL_LIBDIR}/${CMAKE_LIBRARY_ARCHITECTURE})
			set(SSM_TRANSPORT_INSTALL_DIR ${CMAKE_INSTALL_LIBDIR}/openrtm-${OPENRTM_VERSION_MAJOR}.${OPENRTM_VERSION_MINOR}/transport)
		endif(WIN32)
		install(TARGETS ${target} LIBRARY DESTINATION ${SSM_TRANSPORT_INSTALL_DIR}
				ARCHIVE DESTINATION ${SSM_TRANSPORT_INSTALL_DIR}
				RUNTIME DESTINATION ${SSM_TRANSPORT_INSTALL_DIR}
				COMPONENT ext)
	endif()

endif(OpenRTM_aist_BINARY_DIR)

if(VXWORKS)
	if(RTP)
	else(RTP)	
		set_target_properties(${target} PROPERTIES SUFFIX ".out")
	endif(RTP)
endif(VXWORKS)

