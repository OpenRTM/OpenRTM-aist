cmake_minimum_required (VERSION 3.0.2)
project (${COIL_PROJECT_NAME}
	VERSION ${RTM_VERSION}
	LANGUAGES CXX)






set(ACE_5_6_1_OR_EARLIER OFF)
set(COIL_OS_DARWIN OFF)
set(COIL_OS_FREEBSD OFF)
set(COIL_OS_HPUX OFF)
set(COIL_OS_IRIX OFF)
set(COIL_OS_LINUX OFF)
set(COIL_OS_OSF1 OFF)
set(COIL_OS_QNX OFF)
set(COIL_OS_SUNOS OFF)
set(COIL_OS_VXWORKS OFF)
set(COIL_OS_WIN32 OFF)
set(HAVE_DLFCN_H ON)
set(HAVE_INTTYPES_H ON)
set(HAVE_MEMORY_H ON)
set(HAVE_NLIST_H OFF)
set(HAVE_STDINT_H ON)
set(HAVE_STDLIB_H ON)
set(HAVE_STRINGS_H ON)
set(HAVE_STRING_H ON)
set(HAVE_SYS_STAT_H ON)
set(HAVE_SYS_TYPES_H ON)
set(HAVE_UNAME ON)
set(HAVE_UNISTD_H ON)
set(STDC_HEADERS OFF)
set(WITH_ACE OFF)
set(WITH_DMALLOC OFF)
set(_REENTRANT ON)

set(COIL_MAJOR_VERSION ${${COIL_PROJECT_NAME}_VERSION_MAJOR})
set(COIL_MINOR_VERSION ${${COIL_PROJECT_NAME}_VERSION_MINOR})
set(COIL_PLATFORM "POSIX")
set(COIL_REVISION_NUM ${${COIL_PROJECT_NAME}_VERSION_PATCH})
set(COIL_SHORT_VERSION ${${COIL_PROJECT_NAME}_VERSION_MAJOR}.${${COIL_PROJECT_NAME}_VERSION_MINOR})
set(COIL_VERSION ${${COIL_PROJECT_NAME}_VERSION})
set(PACKAGE "coil")
set(PACKAGE_BUGREPORT "n-ando@aist.go.jp")
set(PACKAGE_NAME "coil")
set(PACKAGE_STRING "coil ${COIL_SHORT_VERSION}")
set(PACKAGE_TARNAME "coil")
set(PACKAGE_VERSION ${COIL_SHORT_VERSION})




if(VXWORKS)
	set (COIL_OS_VXWORKS ON)
elseif(UNIX)
	if(${CMAKE_SYSTEM_NAME}test MATCHES QNXtest)
		set (COIL_OS_QNX ON)
	else()
		set (COIL_OS_LINUX ON)
	endif()
elseif(WIN32)
	set (COIL_OS_WIN32 ON)
endif()



configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config_coil_cmake.h.in ${PROJECT_BINARY_DIR}/config_coil.h)


set(coil_headers
	common/coil/Allocator.h
	common/coil/Async.h
	common/coil/ClockManager.h
	common/coil/Factory.h
	common/coil/Guard.h
	common/coil/Listener.h
	common/coil/Logger.h
	common/coil/NonCopyable.h
	common/coil/PeriodicTask.h
	common/coil/PeriodicTaskBase.h
	common/coil/Properties.h
	common/coil/Singleton.h
	common/coil/TimeMeasure.h
	common/coil/TimeValue.h
	common/coil/Timer.h
	common/coil/crc.h
	common/coil/stringutil.h
	${COIL_OS_DIR}/coil/Condition.h
	${COIL_OS_DIR}/coil/DynamicLib.h
	${COIL_OS_DIR}/coil/Mutex.h
	${COIL_OS_DIR}/coil/OS.h
	${COIL_OS_DIR}/coil/Routing.h
	${COIL_OS_DIR}/coil/Process.h
	${COIL_OS_DIR}/coil/Signal.h
	${COIL_OS_DIR}/coil/Task.h
	${COIL_OS_DIR}/coil/Time.h
	${COIL_OS_DIR}/coil/UUID.h
	${COIL_OS_DIR}/coil/atomic.h
	${COIL_OS_DIR}/coil/SharedMemory.h
	${COIL_OS_DIR}/coil/Affinity.h
	${PROJECT_BINARY_DIR}/config_coil.h
)


set(coil_srcs
	common/coil/Allocator.cpp
	common/coil/ClockManager.cpp
	common/coil/PeriodicTask.cpp
	common/coil/Properties.cpp
	common/coil/TimeMeasure.cpp
	common/coil/TimeValue.cpp
	common/coil/Timer.cpp
	common/coil/crc.cpp
	common/coil/stringutil.cpp
	common/coil/Logger.cpp
	${COIL_OS_DIR}/coil/Condition.cpp
	${COIL_OS_DIR}/coil/DynamicLib.cpp
	${COIL_OS_DIR}/coil/Mutex.cpp
	${COIL_OS_DIR}/coil/Routing.cpp
	${COIL_OS_DIR}/coil/Process.cpp
	${COIL_OS_DIR}/coil/Signal.cpp
	${COIL_OS_DIR}/coil/Task.cpp
	${COIL_OS_DIR}/coil/Time.cpp
	${COIL_OS_DIR}/coil/UUID.cpp
	${COIL_OS_DIR}/coil/SharedMemory.cpp
	${COIL_OS_DIR}/coil/Affinity.cpp
	${coil_headers}
)

add_definitions(${COIL_C_FLAGS_LIST})


if(UNIX)
	if(${CMAKE_SYSTEM_NAME}test MATCHES QNXtest)
		if(QNX7)
		else()
		include_directories(SYSTEM /usr/local/include)
		link_directories(/usr/qnx650/target/qnx6/x86/lib/gcc/4.4.2/ /usr/qnx650/target/qnx6/x86/lib/ /usr/local/lib)
		endif()
	endif()

	include(FindPkgConfig)
	pkg_check_modules(UUID REQUIRED uuid)

	link_directories(${UUID_LIBRARY_DIRS})
	add_definitions(${UUID_CFLAGS})
endif(UNIX)


if(VXWORKS)
	add_library(${COIL_PROJECT_NAME} STATIC ${coil_srcs} )
	openrtm_common_set_compile_props(${COIL_PROJECT_NAME})
	target_include_directories(${COIL_PROJECT_NAME}
		PUBLIC ${PROJECT_SOURCE_DIR}/common
		PUBLIC ${PROJECT_SOURCE_DIR}/${COIL_OS_DIR})
	target_include_directories(${COIL_PROJECT_NAME} SYSTEM
		PUBLIC ${CMAKE_BINARY_DIR}/src/lib)
	install(TARGETS ${COIL_PROJECT_NAME} LIBRARY DESTINATION ${INSTALL_RTM_LIB_DIR}
					ARCHIVE DESTINATION ${INSTALL_RTM_BIN_DIR}
					RUNTIME DESTINATION ${INSTALL_RTM_BIN_DIR}
					COMPONENT coil)
else(VXWORKS)
	add_library(${COIL_PROJECT_NAME}_objlib OBJECT ${coil_srcs})
	openrtm_common_set_compile_props(${COIL_PROJECT_NAME}_objlib)
	target_include_directories(${COIL_PROJECT_NAME}_objlib
		PUBLIC ${PROJECT_SOURCE_DIR}/common
		PUBLIC ${PROJECT_SOURCE_DIR}/${COIL_OS_DIR})
	target_include_directories(${COIL_PROJECT_NAME}_objlib SYSTEM
		PUBLIC ${CMAKE_BINARY_DIR}/src/lib
		PRIVATE ${UUID_INCLUDE_DIRS})
	add_library(${COIL_PROJECT_NAME}-static STATIC $<TARGET_OBJECTS:${COIL_PROJECT_NAME}_objlib>)

	if(WIN32)
		set(DEF_FILE_NAME  ${COIL_PROJECT_NAME}.def)

		add_custom_command(TARGET ${COIL_PROJECT_NAME}-static 
			POST_BUILD
			COMMAND python -B ${MAKE_DEF_COMMAND} $<TARGET_FILE:${COIL_PROJECT_NAME}-static> ${COIL_PROJECT_NAME} ${RTM_SHORT_VERSION} ${DEF_FILE_NAME}
			WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
		)
		
	endif(WIN32)

	add_library(${COIL_PROJECT_NAME} SHARED $<TARGET_OBJECTS:${COIL_PROJECT_NAME}_objlib> ${CMAKE_CURRENT_BINARY_DIR}/${DEF_FILE_NAME})
	set_target_properties(${COIL_PROJECT_NAME} PROPERTIES DEBUG_POSTFIX d)
	target_link_libraries(${COIL_PROJECT_NAME} PRIVATE ${RTM_LINKER_OPTION}  ${UUID_LIBRARIES})

	if(UNIX)
		set_target_properties(${COIL_PROJECT_NAME} PROPERTIES VERSION ${RTM_VERSION} SOVERSION ${COIL_SO_VERSION})
		install(TARGETS ${COIL_PROJECT_NAME} LIBRARY DESTINATION ${INSTALL_RTM_LIB_DIR}
						ARCHIVE DESTINATION ${INSTALL_RTM_LIB_DIR}
						RUNTIME DESTINATION ${INSTALL_RTM_BIN_DIR}
						COMPONENT coil)

		set_target_properties(${COIL_PROJECT_NAME}-static PROPERTIES DEBUG_POSTFIX d)
		set_target_properties(${COIL_PROJECT_NAME}-static PROPERTIES OUTPUT_NAME ${COIL_PROJECT_NAME} CLEAN_DIRECT_OUTPUT 1)
		install(TARGETS ${COIL_PROJECT_NAME}-static LIBRARY DESTINATION ${INSTALL_RTM_LIB_DIR}
						ARCHIVE DESTINATION ${INSTALL_RTM_LIB_DIR}
						RUNTIME DESTINATION ${INSTALL_RTM_BIN_DIR}
						COMPONENT coil)

	elseif(WIN32)
		set_source_files_properties(${DEF_FILE_NAME} PROPERTIES GENERATED 1)
		add_dependencies(${COIL_PROJECT_NAME} ${COIL_PROJECT_NAME}-static)
		install(TARGETS ${COIL_PROJECT_NAME} LIBRARY DESTINATION ${INSTALL_RTM_LIB_DIR}
						 ARCHIVE DESTINATION ${INSTALL_RTM_LIB_DIR}
						RUNTIME DESTINATION ${INSTALL_RTM_LIB_DIR}
						COMPONENT coil)

		install(TARGETS ${COIL_PROJECT_NAME}-static LIBRARY DESTINATION ${INSTALL_RTM_LIB_DIR}
						 ARCHIVE DESTINATION ${INSTALL_RTM_LIB_DIR}
						RUNTIME DESTINATION ${INSTALL_RTM_LIB_DIR}
						COMPONENT coil)
	endif()
endif(VXWORKS)
install(FILES  ${coil_headers} DESTINATION ${INSTALL_COIL_INCLUDE_DIR}/coil COMPONENT headers)



install(FILES ${CMAKE_SOURCE_DIR}/build/makedeffile.py DESTINATION ${INSTALL_RTM_SCRIPT_DIR} COMPONENT scripts)
install(FILES ${CMAKE_SOURCE_DIR}/build/setuptest.py DESTINATION ${INSTALL_RTM_SCRIPT_DIR} COMPONENT scripts)
install(FILES ${CMAKE_SOURCE_DIR}/build/slntool.py DESTINATION ${INSTALL_RTM_SCRIPT_DIR} COMPONENT scripts)
install(FILES ${CMAKE_SOURCE_DIR}/build/uuid.py DESTINATION ${INSTALL_RTM_SCRIPT_DIR} COMPONENT scripts)
install(FILES ${CMAKE_SOURCE_DIR}/build/vcprojtool.py DESTINATION ${INSTALL_RTM_SCRIPT_DIR} COMPONENT scripts)
install(FILES ${CMAKE_SOURCE_DIR}/build/vcxprojtool.py DESTINATION ${INSTALL_RTM_SCRIPT_DIR} COMPONENT scripts)
install(FILES ${CMAKE_SOURCE_DIR}/build/yat.py DESTINATION ${INSTALL_RTM_SCRIPT_DIR} COMPONENT scripts)

add_subdirectory(utils)
