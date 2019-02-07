cmake_minimum_required (VERSION 2.6)
project (${COIL_PROJECT_NAME})






option (ACE_5_6_1_OR_EARLIER "ACE version 5.6.1 or earlier" OFF)
option (COIL_OS_DARWIN "OS is Max OS X" OFF)
option (COIL_OS_FREEBSD "OS is FreeBSD" OFF)
option (COIL_OS_HPUX "OS is HP-UX" OFF)
option (COIL_OS_IRIX "OS is IRIX" OFF)
option (COIL_OS_LINUX "OS is Linux" OFF)
option (COIL_OS_OSF1 "OS is OSF1" OFF)
option (COIL_OS_QNX "OS is QNX" OFF)
option (COIL_OS_SUNOS "OS is SunOS" OFF)
option (COIL_OS_VXWORKS "OS is VxWorks" OFF)
option (COIL_OS_WIN32 "OS is Windows" OFF)
option (HAVE_DLFCN_H "Define to 1 if you have the <dlfcn.h> header file." ON)
option (HAVE_INTTYPES_H "Define to 1 if you have the <inttypes.h> header file." ON)
option (HAVE_MEMORY_H "Define to 1 if you have the <memory.h> header file." ON)
option (HAVE_NLIST_H "Define to 1 if you have the <nlist.h> header file." OFF)
option (HAVE_STDINT_H "Define to 1 if you have the <stdint.h> header file." ON)
option (HAVE_STDLIB_H "Define to 1 if you have the <stdlib.h> header file." ON)
option (HAVE_STRINGS_H "Define to 1 if you have the <strings.h> header file." ON)
option (HAVE_STRING_H "Define to 1 if you have the <string.h> header file." ON)
option (HAVE_SYS_STAT_H "Define to 1 if you have the <sys/stat.h> header file." ON)
option (HAVE_SYS_TYPES_H "Define to 1 if you have the <sys/types.h> header file." ON)
option (HAVE_UNAME "Define to 1 if you have the `uname' function." ON)
option (HAVE_UNISTD_H "Define to 1 if you have the <unistd.h> header file." ON)
option (STDC_HEADERS "Define to 1 if you have the ANSI C header files." OFF)
option (WITH_ACE "compiled with ACE" OFF)
option (WITH_DMALLOC "Define if using the dmalloc debugging malloc package" OFF)
option (_REENTRANT "Multi Thread Support" ON)

set(COIL_MAJOR_VERSION "1.0" CACHE STRING "COIL major version")
set(COIL_MINOR_VERSION "1.0" CACHE STRING "COIL minor version")
set(COIL_PLATFORM "POSIX" CACHE STRING "coil platform is ACE")
set(COIL_REVISION_NUM "1.0" CACHE STRING "")
set(COIL_SHORT_VERSION "1.01.01.0" CACHE STRING "COIL version")
set(COIL_VERSION "1.0" CACHE STRING "COIL version")
set(PACKAGE "coil" CACHE STRING "Name of package")
set(PACKAGE_BUGREPORT "n-ando@aist.go.jp" CACHE STRING "Define to the address where bug reports for this package should be sent.")
set(PACKAGE_NAME "coil" CACHE STRING "Define to the full name of this package.")
set(PACKAGE_STRING "coil 1.0" CACHE STRING "Define to the full name and version of this package.")
set(PACKAGE_TARNAME "coil" CACHE STRING "Define to the one symbol short name of this package.")
set(PACKAGE_VERSION "1.0" CACHE STRING "Define to the version of this package.")




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

#if(CORBA MATCHES "TAO")
#	set (WITH_ACE ON)
#endif()

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config_coil_cmake.h.in ${PROJECT_BINARY_DIR}/config_coil.h)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config_coil_cmake.h.in ${CMAKE_CURRENT_SOURCE_DIR}/${COIL_OS_DIR}/coil/config_coil.h)


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



if(WIN32)
	set(DEF_FILE_NAME  ${COIL_PROJECT_NAME}.def)
	add_library(${COIL_PROJECT_NAME}_static STATIC ${coil_srcs})
	get_property(coil_static_location TARGET ${COIL_PROJECT_NAME}_static PROPERTY LOCATION)

	install(TARGETS ${COIL_PROJECT_NAME}_static LIBRARY DESTINATION ${INSTALL_RTM_LIB_DIR}
					 ARCHIVE DESTINATION ${INSTALL_RTM_LIB_DIR}
					RUNTIME DESTINATION ${INSTALL_RTM_LIB_DIR}
					COMPONENT coil)

	add_custom_command(OUTPUT ${DEF_FILE_NAME}
		COMMAND python ${MAKE_DEF_COMMAND} ${coil_static_location} ${COIL_PROJECT_NAME} ${RTM_SHORT_VERSION} ${DEF_FILE_NAME}
		
		WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
	)
	add_custom_target(${COIL_PROJECT_NAME}_TGT DEPENDS ${DEF_FILE_NAME})
	add_dependencies(${COIL_PROJECT_NAME}_TGT ${COIL_PROJECT_NAME}_static)
	set(coil_srcs ${coil_srcs} ${CMAKE_CURRENT_BINARY_DIR}/${DEF_FILE_NAME})
#	add_custom_command(OUTPUT ${COIL_PROJECT_NAME}.def
#		COMMAND lib -out:"coil_static.lib" "$(IntDir)Skel*.obj"
#		COMMAND python ${MAKE_DEF_COMMAND} coil_static.lib ${COIL_PROJECT_NAME} ${RTM_SHORT_VERSION} ${COIL_PROJECT_NAME}.def
#		PRE_LINK
#		WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
#		DEPENDS ${COIL_PROJECT_NAME}
#	)
#	set(coil_srcs ${coil_srcs} ${CMAKE_CURRENT_BINARY_DIR}/${COIL_PROJECT_NAME}.def)
	
else(WIN32)
endif()


include_directories(
		${PROJECT_SOURCE_DIR}/common 
		${PROJECT_SOURCE_DIR}/${COIL_OS_DIR}
		${PROJECT_BINARY_DIR}/..
		)
if(UNIX)
	if(${CMAKE_SYSTEM_NAME}test MATCHES QNXtest)
		if(QNX7)
		else()
		include_directories(/usr/local/include)
		link_directories(/usr/qnx650/target/qnx6/x86/lib/gcc/4.4.2/ /usr/qnx650/target/qnx6/x86/lib/ /usr/local/lib)
		endif()
	endif()

	include(FindPkgConfig)
	pkg_check_modules(UUID REQUIRED uuid)

	include_directories(${UUID_INCLUDE_DIRS})
	link_directories(${UUID_LIBRARY_DIRS})
	add_definitions(${UUID_CFLAGS})
endif(UNIX)
#message(STATUS ${CMAKE_INCLUDE_DIRECTORIES_BEFORE})

if(VXWORKS)
	add_library(${COIL_PROJECT_NAME} STATIC ${coil_srcs} )
	install(TARGETS ${COIL_PROJECT_NAME} LIBRARY DESTINATION ${INSTALL_RTM_LIB_DIR}
					ARCHIVE DESTINATION ${INSTALL_RTM_BIN_DIR}
					RUNTIME DESTINATION ${INSTALL_RTM_BIN_DIR}
					COMPONENT coil)
else(VXWORKS)
	add_library(${COIL_PROJECT_NAME} SHARED ${coil_srcs})
	set_target_properties(${COIL_PROJECT_NAME} PROPERTIES DEBUG_POSTFIX d)
	target_link_libraries(${COIL_PROJECT_NAME} ${RTM_LINKER_OPTION}  ${UUID_LIBRARIES})
	if(UNIX)
		install(TARGETS ${COIL_PROJECT_NAME} LIBRARY DESTINATION ${INSTALL_RTM_LIB_DIR}
						ARCHIVE DESTINATION ${INSTALL_RTM_LIB_DIR}
						RUNTIME DESTINATION ${INSTALL_RTM_BIN_DIR}
						COMPONENT coil)
		add_library(${COIL_PROJECT_NAME}-static STATIC ${coil_srcs})
		set_target_properties(${COIL_PROJECT_NAME}-static PROPERTIES DEBUG_POSTFIX d)
		set_target_properties(${COIL_PROJECT_NAME}-static PROPERTIES OUTPUT_NAME ${COIL_PROJECT_NAME} CLEAN_DIRECT_OUTPUT 1)
		install(TARGETS ${COIL_PROJECT_NAME}-static LIBRARY DESTINATION ${INSTALL_RTM_LIB_DIR}
						ARCHIVE DESTINATION ${INSTALL_RTM_LIB_DIR}
						RUNTIME DESTINATION ${INSTALL_RTM_BIN_DIR}
						COMPONENT coil)

	elseif(WIN32)
		set_source_files_properties(${DEF_FILE_NAME} PROPERTIES GENERATED 1)
		add_dependencies(${COIL_PROJECT_NAME} ${COIL_PROJECT_NAME}_TGT)
		add_dependencies(${COIL_PROJECT_NAME} ${COIL_PROJECT_NAME}_static)
		install(TARGETS ${COIL_PROJECT_NAME} LIBRARY DESTINATION ${INSTALL_RTM_LIB_DIR}
						 ARCHIVE DESTINATION ${INSTALL_RTM_LIB_DIR}
						RUNTIME DESTINATION ${INSTALL_RTM_LIB_DIR}
						COMPONENT coil)
	endif()
endif(VXWORKS)
install(FILES  ${coil_headers} DESTINATION ${INSTALL_COIL_INCLUDE_DIR}/coil COMPONENT headers)
#add_executable(coil  ${coil_srcs} )


install(FILES ${CMAKE_SOURCE_DIR}/build/makedeffile.py DESTINATION ${INSTALL_RTM_SCRIPT_DIR} COMPONENT scripts)
install(FILES ${CMAKE_SOURCE_DIR}/build/setuptest.py DESTINATION ${INSTALL_RTM_SCRIPT_DIR} COMPONENT scripts)
install(FILES ${CMAKE_SOURCE_DIR}/build/slntool.py DESTINATION ${INSTALL_RTM_SCRIPT_DIR} COMPONENT scripts)
install(FILES ${CMAKE_SOURCE_DIR}/build/uuid.py DESTINATION ${INSTALL_RTM_SCRIPT_DIR} COMPONENT scripts)
install(FILES ${CMAKE_SOURCE_DIR}/build/vcprojtool.py DESTINATION ${INSTALL_RTM_SCRIPT_DIR} COMPONENT scripts)
install(FILES ${CMAKE_SOURCE_DIR}/build/vcxprojtool.py DESTINATION ${INSTALL_RTM_SCRIPT_DIR} COMPONENT scripts)
install(FILES ${CMAKE_SOURCE_DIR}/build/yat.py DESTINATION ${INSTALL_RTM_SCRIPT_DIR} COMPONENT scripts)

add_subdirectory(utils)