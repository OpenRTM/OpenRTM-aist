cmake_minimum_required (VERSION 3.5.1)

project (OpenRTM_aist
	VERSION 2.0.2
	LANGUAGES CXX)

set(CORBA omniORB CACHE STRING "set CORBA ")
set(ORB_ROOT ${ORB_ROOT} CACHE PATH "set ORB_ROOT")

if(${CMAKE_VERSION} VERSION_GREATER 3.11.99)
	find_package (Python3 REQUIRED COMPONENTS Interpreter)
else()
	set(Python3_EXECUTABLE python3)
endif()

find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
   set(CMAKE_CXX_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")
endif()

if(NOT ORB_ROOT)
	if(CORBA STREQUAL "omniORB")
		if(UNIX)
			include(FindPkgConfig)
			pkg_check_modules(OMNIORB REQUIRED omniORB4)
			if(NOT OMNIORB_DIR)
			  if(OMNIORB_FOUND)
			    set(OMNIORB_DIR )
			  endif()
			  set(OMNIORB_DIR CACHE PATH omniORB root directory)
			endif()
		else(UNIX)
			message(FATAL_ERROR "Please set ORB_ROOT.")
		endif(UNIX)

	elseif(CORBA STREQUAL "TAO")
		if(UNIX)
			include(FindPkgConfig)
			pkg_check_modules(TAO_Svc_Utils REQUIRED TAO_Svc_Utils)
			pkg_check_modules(TAO_CosNaming REQUIRED TAO_CosNaming)
			pkg_check_modules(TAO_IORTable REQUIRED TAO_IORTable)
			pkg_check_modules(TAO_Utils REQUIRED TAO_Utils)
			pkg_check_modules(TAO_Strategies REQUIRED TAO_Strategies)
			set(ACE_TAO_LIBRARIES ${TAO_Svc_Utils_LIBRARIES} ${TAO_CosNaming_LIBRARIES} ${TAO_IORTable_LIBRARIES} ${TAO_Utils_LIBRARIES} ${TAO_Strategies_LIBRARIES})
			set(ACE_TAO_LIBRARY_DIRS ${TAO_Svc_Utils_LIBRARY_DIRS} ${TAO_CosNaming_LIBRARY_DIRS} ${TAO_IORTable_LIBRARY_DIRS} ${TAO_Utils_LIBRARY_DIRS} ${TAO_Strategies_LIBRARY_DIRS})
			set(ACE_TAO_INCLUDE_DIRS ${TAO_Svc_Utils_INCLUDE_DIRS} ${TAO_CosNaming_INCLUDE_DIRS} ${TAO_IORTable_INCLUDE_DIRS} ${TAO_Utils_INCLUDE_DIRS} ${TAO_Strategies_INCLUDE_DIRS})
			set(ACE_TAO_CFLAGS ${TAO_Svc_Utils_CFLAGS} ${TAO_CosNaming_CFLAGS} ${TAO_IORTable_CFLAGS} ${TAO_Utils_CFLAGS} ${TAO_Strategies_CFLAGS})
			set(ACE_TAO_DIR ${TAO_Svc_Utils_PREFIX})
		else(UNIX)
			message(FATAL_ERROR "Please set ORB_ROOT.")
		endif(UNIX)		
	elseif(CORBA STREQUAL "RtORB")
	else()
		message(FATAL_ERROR "Please set ORB_ROOT.")
	endif()
endif()

set(RTM_MAJOR_VERSION ${OpenRTM_aist_VERSION_MAJOR})
set(RTM_MINOR_VERSION ${OpenRTM_aist_VERSION_MINOR})
set(RTM_REVISION_NUM ${OpenRTM_aist_VERSION_PATCH})
set(RTM_SHORT_VERSION ${OpenRTM_aist_VERSION_MAJOR}${OpenRTM_aist_VERSION_MINOR}${OpenRTM_aist_VERSION_PATCH})
set(RTM_VERSION ${OpenRTM_aist_VERSION})
set(OPENRTM_VERSION ${RTM_MAJOR_VERSION}.${RTM_MINOR_VERSION}.${RTM_REVISION_NUM})
set(OPENRTM_NAME OpenRTM-aist-${OPENRTM_VERSION})
set(RTM_SO_VERSION ${OpenRTM_aist_VERSION_MAJOR}.${OpenRTM_aist_VERSION_MINOR})
set(COIL_PROJECT_NAME coil)
if(MSVC)
	set(RTM_PROJECT_NAME RTC)
else()
	set(RTM_PROJECT_NAME RTC2)
endif()
set(RTCSKEL_PROJECT_NAME RTCSkel)

if(MSVC)
	if(${MSVC_TOOLSET_VERSION} STREQUAL "140" OR
			${MSVC_TOOLSET_VERSION} STREQUAL "141")
		set(RTM_VC_VER vc14)
	elseif(${MSVC_TOOLSET_VERSION} STREQUAL "142" OR
			${MSVC_TOOLSET_VERSION} STREQUAL "143")
		set(RTM_VC_VER vc16)
	else()
		set(RTM_VC_VER "vc${MSVC_TOOLSET_VERSION}")
	endif()
	set(INSTALL_RTM_SCRIPT_DIR ${OPENRTM_VERSION}/bin)
	set(INSTALL_RTM_BIN_DIR ${OPENRTM_VERSION}/bin/${RTM_VC_VER})
	set(INSTALL_RTM_INCLUDE_DIR ${OPENRTM_VERSION})
	set(INSTALL_COIL_INCLUDE_DIR ${OPENRTM_VERSION})
	set(INSTALL_RTM_LIB_DIR ${OPENRTM_VERSION}/bin/${RTM_VC_VER})
	set(INSTALL_RTM_EXAMPLE_DIR ${OPENRTM_VERSION}/Components/C++/Examples/${RTM_VC_VER})
	set(INSTALL_RTM_EXAMPLE_SHAREDLIB_DIR ${INSTALL_RTM_EXAMPLE_DIR})
	set(INSTALL_RTM_DOC_DIR ${OPENRTM_VERSION}/docs)
	set(INSTALL_RTM_EXT_DIR ${OPENRTM_VERSION}/ext/${RTM_VC_VER})
	set(INSTALL_RTM_CMAKE_DIR ${OPENRTM_VERSION}/cmake)
	set(INSTALL_RTM_ETC_DIR ${OPENRTM_VERSION}/ext)
	# For setting warning options manually, delete CMake default options.
	string(REGEX REPLACE "/W[0-4]" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
	if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
		set(CMAKE_INCLUDE_SYSTEM_FLAG_CXX "/imsvc ")
	endif()
elseif(UNIX)
	include(GNUInstallDirs)
	if(BUILD_RTM_LINUX_PKGS)
		set(CMAKE_INSTALL_LIBDIR ${CMAKE_INSTALL_LIBDIR}/${CMAKE_LIBRARY_ARCHITECTURE})
	endif()
	set(INSTALL_RTM_SCRIPT_DIR ${CMAKE_INSTALL_LIBDIR}/openrtm-${RTM_MAJOR_VERSION}.${RTM_MINOR_VERSION}/py_helper)
	set(INSTALL_RTM_BIN_DIR ${CMAKE_INSTALL_BINDIR})
	set(INSTALL_RTM_INCLUDE_DIR ${CMAKE_INSTALL_INCLUDEDIR}/openrtm-${RTM_MAJOR_VERSION}.${RTM_MINOR_VERSION})
	set(INSTALL_COIL_INCLUDE_DIR ${CMAKE_INSTALL_INCLUDEDIR}/coil-${RTM_MAJOR_VERSION}.${RTM_MINOR_VERSION})
	set(INSTALL_RTM_LIB_DIR ${CMAKE_INSTALL_LIBDIR})
	set(INSTALL_RTM_EXAMPLE_DIR ${CMAKE_INSTALL_DATAROOTDIR}/openrtm-${RTM_MAJOR_VERSION}.${RTM_MINOR_VERSION}/components/c++/examples)
	set(INSTALL_RTM_EXAMPLE_SHAREDLIB_DIR ${INSTALL_RTM_EXAMPLE_DIR}/rtc)
	set(INSTALL_RTM_DOC_DIR ${CMAKE_INSTALL_DATAROOTDIR}/openrtm-${RTM_MAJOR_VERSION}.${RTM_MINOR_VERSION}/doc)
	set(INSTALL_RTM_EXT_DIR ${CMAKE_INSTALL_LIBDIR}/openrtm-${RTM_MAJOR_VERSION}.${RTM_MINOR_VERSION})
	set(INSTALL_RTM_CMAKE_DIR ${CMAKE_INSTALL_LIBDIR}/openrtm-${RTM_MAJOR_VERSION}.${RTM_MINOR_VERSION}/cmake)
	set(INSTALL_RTM_ETC_DIR ${CMAKE_INSTALL_SYSCONFDIR})
	set(INSTALL_RTM_SHARE_DIR ${CMAKE_INSTALL_DATAROOTDIR})
	if(${CMAKE_CXX_COMPILER_VERSION} VERSION_GREATER_EQUAL 12.0.0)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-aggressive-loop-optimizations --param max-inline-insns-single=1000 --param inline-unit-growth=1000 --param large-function-growth=2000")
	endif()
else()
	set(INSTALL_RTM_SCRIPT_DIR lib/openrtm-${RTM_MAJOR_VERSION}.${RTM_MINOR_VERSION}/py_helper)
	set(INSTALL_RTM_BIN_DIR bin)
	set(INSTALL_RTM_INCLUDE_DIR include/openrtm-${RTM_MAJOR_VERSION}.${RTM_MINOR_VERSION})
	set(INSTALL_COIL_INCLUDE_DIR include/coil-${RTM_MAJOR_VERSION}.${RTM_MINOR_VERSION})
	set(INSTALL_RTM_LIB_DIR lib)
	set(INSTALL_RTM_EXAMPLE_DIR share/openrtm-${RTM_MAJOR_VERSION}.${RTM_MINOR_VERSION}/components/c++/examples)
	set(INSTALL_RTM_EXAMPLE_SHAREDLIB_DIR ${INSTALL_RTM_EXAMPLE_DIR}/rtc)
	set(INSTALL_RTM_DOC_DIR share/openrtm-${RTM_MAJOR_VERSION}.${RTM_MINOR_VERSION}/doc)
	set(INSTALL_RTM_EXT_DIR lib/openrtm-${RTM_MAJOR_VERSION}.${RTM_MINOR_VERSION})
	set(INSTALL_RTM_CMAKE_DIR lib/openrtm-${RTM_MAJOR_VERSION}.${RTM_MINOR_VERSION}/cmake)
	set(INSTALL_RTM_ETC_DIR etc)
	set(INSTALL_RTM_SHARE_DIR share)
endif(MSVC)

set(SSL_ENABLE FALSE CACHE BOOL "set SSL_ENABLE")
set(HTTP_ENABLE FALSE CACHE BOOL "set HTTP_ENABLE")
set(OBSERVER_ENABLE FALSE CACHE BOOL "set OBSERVER_ENABLE")

set(LOGGING_ENABLE TRUE CACHE BOOL "set LOGGING_ENABLE")
if(NO_LOGGING)
	set(ORB_C_FLAGS_LIST
		${ORB_C_FLAGS_LIST}
		-DNO_LOGGING
	)
endif(NO_LOGGING)


if(CORBA STREQUAL "ORBexpress")
	if(NOT IDL_COMPILE_COMMAND)
		set(IDL_COMPILE_COMMAND ${ORB_ROOT}/host/x86_linux_2.6/bin/idl2cpp)
	endif()
elseif(CORBA STREQUAL "omniORB")
	if(NOT IDL_COMPILE_COMMAND)
		if(test${ORB_ROOT} STREQUAL "test")
			set(IDL_COMPILE_COMMAND omniidl)
		else()
			if(VXWORKS)
				set(IDL_COMPILE_COMMAND ${ORB_ROOT}/build/bin/omniidl)
			elseif(UNIX)
				set(IDL_COMPILE_COMMAND ${ORB_ROOT}/bin/omniidl)
			elseif(MSVC)
				set(IDL_COMPILE_COMMAND ${ORB_ROOT}/bin/x86_win32/omniidl)
			endif()
		endif()
	endif()
	
	if(UNIX AND NOT ORB_ROOT)
		string(REGEX REPLACE "([0-9]).([0-9]).([0-9])$" "\\1" OMNI_VERSION1 ${OMNIORB_VERSION})
		string(REGEX REPLACE "([0-9]).([0-9]).([0-9])$" "\\2" OMNI_VERSION2 ${OMNIORB_VERSION})
		set(OMNI_VERSION ${OMNI_VERSION1}${OMNI_VERSION2} CACHE PATH "set OMNI_VERSION" FORCE)
	else()
		file(GLOB OMNIORB_VERSIONFILEPATH "${ORB_ROOT}/THIS_IS_OMNIORB_*_*_*")
		if(NOT OMNIORB_VERSIONFILEPATH)
			message(FATAL_ERROR "can not find THIS_IS_OMNIORB_*_*_*.")
		endif()
		get_filename_component(OMNIORB_VERSIONFILE "${OMNIORB_VERSIONFILEPATH}" NAME)
		string(REGEX REPLACE "THIS_IS_OMNIORB_([0-9])_([0-9])_([0-9]).*$" "\\1\\2" OMNI_VERSION ${OMNIORB_VERSIONFILE})
		string(REGEX REPLACE "THIS_IS_OMNIORB_([0-9])_([0-9])_([0-9]).*$" "\\3" OMNI_MINOR ${OMNIORB_VERSIONFILE})
		string(REGEX REPLACE "([0-9])([0-9])$" "\\1" OMNI_VERSION1 ${OMNI_VERSION})
		string(REGEX REPLACE "([0-9])([0-9])$" "\\2" OMNI_VERSION2 ${OMNI_VERSION})
	endif()
elseif(CORBA STREQUAL "TAO")
	if(NOT IDL_COMPILE_COMMAND)
		if(NOT ORB_ROOT)
			set(IDL_COMPILE_COMMAND ${ACE_TAO_DIR}/bin/tao_idl)
		else()
			if(VXWORKS)
				message(FATAL_ERROR "Please set IDL_COMPILE_COMMAND.")
			elseif(UNIX)
				set(IDL_COMPILE_COMMAND ${ORB_ROOT}/bin/tao_idl)
			elseif(MSVC)
				set(IDL_COMPILE_COMMAND ${ORB_ROOT}/bin/tao_idl)
			endif()
		endif()
	endif()
elseif(CORBA STREQUAL "RtORB")
	if(NOT IDL_COMPILE_COMMAND)
		if(NOT ORB_ROOT)
			set(IDL_COMPILE_COMMAND rtorb-idl)
		else()
			if(VXWORKS)
				message(FATAL_ERROR "Please set IDL_COMPILE_COMMAND.")
			elseif(UNIX)
				set(IDL_COMPILE_COMMAND ${ORB_ROOT}/bin/rtorb-idl)
			elseif(MSVC)
				set(IDL_COMPILE_COMMAND ${ORB_ROOT}/bin/rtorb-idl)
			endif()
		endif()
	endif()
endif()

if(VXWORKS)
	if("${VX_CPU_FAMILY}" STREQUAL "ppc")
		set(CPU_TYPE powerpc)
	elseif("${VX_CPU_FAMILY}" STREQUAL "simpentium")
		set(CPU_TYPE simpentium)
	elseif("${VX_CPU_FAMILY}" STREQUAL "simlinux")
		set(CPU_TYPE simlinux)
	endif()
endif(VXWORKS)



if(CORBA STREQUAL "ORBexpress")
	set(CORBA_NAME "ORBexpress" CACHE STRING "CORBA name.")
	if(RTP)
		set(OE_TARGET gcc_4.1.2_Rp)
		set(OE_TYPE config_rt_fast)
		set(ORB_LINK_DIR ${ORB_ROOT}/target/ppc_8xx-vxworks_6.6/${OE_TARGET}/${OE_TYPE}/lib)
		set(ORB_INCLUDE_DIR ${ORB_ROOT}/target/ppc_8xx-vxworks_6.6/${OE_TARGET}/${OE_TYPE}/include)

		set(ORB_LIBRARIES OEtcp OEmirror OEbridge OEudp OEipmc OEshrmem OEorb)

	else(RTP)
		set(OE_TARGET gcc_4.1.2_ExLcRi)
		set(OE_TYPE config_rt_full)
		set(ORB_LINK_DIR ${ORB_ROOT}/target/ppc_8xx-vxworks_6.6/${OE_TARGET}/config_rt_full/lib ${ORB_ROOT}/target/ppc_8xx-vxworks_6.6/${OE_TARGET}/${OE_TYPE}/OENames_2.1.6/lib)
		set(ORB_INCLUDE_DIR ${ORB_ROOT}/target/ppc_8xx-vxworks_6.6/${OE_TARGET}/${OE_TYPE}/include)
		set(ORB_INCLUDE_DIR ${ORB_INCLUDE_DIR} ${ORB_ROOT}/target/ppc_8xx-vxworks_6.6/${OE_TARGET}/${OE_TYPE}/OENames_2.1.6/include)

		set(ORB_LIBRARIES OEnames OEtcp OEmirror OEbridge OEudp OEipmc OEshrmem OEorb)
	endif()
	set(ORB_C_FLAGS_LIST
		${ORB_C_FLAGS_LIST}
		 -DCORBA_ANY_SHARED_PTR_OPERATORS -DINCLUDE_CORBA_CORBALOC
#		-Wpointer-arith -Wwrite-strings -Waggregate-return -Wredundant-decls -Wno-unused -Wshadow -Wundef -Wold-style-cast -fno-implement-inlines -fvolatile  -ansi -msoft-float
	)
	if("${VX_CPU_FAMILY}" STREQUAL "ppc")
		set(ORB_C_FLAGS_LIST ${ORB_C_FLAGS_LIST} -D__powerpc__)
	else()
		set(ORB_C_FLAGS_LIST ${ORB_C_FLAGS_LIST} -D__x86__)
	endif()
elseif(CORBA STREQUAL "omniORB")
	set(CORBA_NAME "omniORB" CACHE STRING "CORBA name.")
	if(VXWORKS)
		if("${VX_CPU_FAMILY}" STREQUAL "ppc")
			set(ORB_C_FLAGS_LIST
				${ORB_C_FLAGS_LIST}
				-D__vxWorks__
				-D__powerpc__
			)
		else()
			set(ORB_C_FLAGS_LIST
				${ORB_C_FLAGS_LIST}
				-D__vxWorks__
				-D__x86__
			)
		endif()
		if(RTP)
			set(ORB_LINK_DIR ${ORB_ROOT}/lib/${CPU_TYPE}_vxWorks_RTP_${VXWORKS_MAJOR}.${VXWORKS_MINOR})
		else(RTP)
			set(ORB_LINK_DIR ${ORB_ROOT}/lib/${CPU_TYPE}_vxWorks_kernel_${VXWORKS_MAJOR}.${VXWORKS_MINOR})
		endif()
		set(ORB_LIBRARIES omniDynamic4 omniORB4 omnithread)
		set(ORB_INCLUDE_DIR ${ORB_ROOT}/include)
	elseif(UNIX)
		if(ORB_ROOT)
			set(ORB_LINK_DIR ${ORB_ROOT}/lib)
			set(ORB_INCLUDE_DIR ${ORB_ROOT}/include)
		else(ORB_ROOT)
			set(ORB_LINK_DIR ${OMNIORB_LIBRARY_DIRS})
			set(ORB_INCLUDE_DIR ${OMNIORB_INCLUDE_DIRS})
		endif(ORB_ROOT)
		set(ORB_LIBRARIES omniDynamic4 omniORB4 omnithread)
		if(SSL_ENABLE)
			set(ORBSSL_LIBRARIES omnisslTP4)
			if(HTTP_ENABLE)
				set(ORBHTTP_LIBRARIES omnihttpCrypto4 omnihttpTP4)
			endif(HTTP_ENABLE)
		endif(SSL_ENABLE)
		

	elseif(MSVC)
		if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "ARM")
			set(ARCH_NAME ARM_win32)
			set(ORB_C_FLAGS_LIST
				${ORB_C_FLAGS_LIST}
				-D__WIN32__
				-D__arm__
			)
			set(ORB_LINK_DIR ${ORB_ROOT}/lib/${ARCH_NAME})
			set(OMNIORB_BINARY_DIR ${ORB_ROOT}/bin/${ARCH_NAME})
		else()
			set(ARCH_NAME x86_win32)
			set(ORB_C_FLAGS_LIST
				${ORB_C_FLAGS_LIST}
				-D__WIN32__
				-D__x86__
			)
			set(ORB_LINK_DIR ${ORB_ROOT}/lib/${ARCH_NAME})
			set(OMNIORB_BINARY_DIR ${ORB_ROOT}/bin/${ARCH_NAME})
		endif()
		
		file(GLOB OMNITHREAD_LIBS "${ORB_ROOT}/lib/x86_win32/omnithread*_rt.lib")
		foreach(OMNITHREAD_LIB ${OMNITHREAD_LIBS})
			get_filename_component(OMNITHREAD_LIBNAME "${OMNITHREAD_LIB}" NAME)
			if("${OMNITHREAD_LIB}" MATCHES "omnithread([0-9])([0-9])_rt.lib$")
				string(REGEX REPLACE "omnithread([0-9])([0-9])_rt.lib$" "\\1\\2" OMNITHREAD_VERSION ${OMNITHREAD_LIBNAME})
			endif()
		endforeach()
		if(NOT OMNITHREAD_VERSION)
			message(FATAL_ERROR "can not find omnithread**_rt.lib.")
		endif()
		set(ORB_LIBRARIES optimized;omniDynamic${OMNI_VERSION}${OMNI_MINOR}_rt;optimized;omniORB${OMNI_VERSION}${OMNI_MINOR}_rt;optimized;omnithread${OMNITHREAD_VERSION}_rt;debug;omniDynamic${OMNI_VERSION}${OMNI_MINOR}_rtd;debug;omniORB${OMNI_VERSION}${OMNI_MINOR}_rtd;debug;omnithread${OMNITHREAD_VERSION}_rtd)
		if(SSL_ENABLE)
			set(ORBSSL_LIBRARIES optimized;omnisslTP${OMNI_VERSION}${OMNI_MINOR}_rt;debug;omnisslTP${OMNI_VERSION}${OMNI_MINOR}_rtd)
			if(HTTP_ENABLE)
				set(ORBHTTP_LIBRARIES 
					optimized;omnihttpCrypto${OMNI_VERSION}${OMNI_MINOR}_rt;debug;omnihttpCrypto${OMNI_VERSION}${OMNI_MINOR}_rtd;
					optimized;omnihttpTP${OMNI_VERSION}${OMNI_MINOR}_rt;debug;omnihttpTP${OMNI_VERSION}${OMNI_MINOR}_rtd)
			endif(HTTP_ENABLE)
		endif(SSL_ENABLE)



		set(ORB_INSTALL_DIR ${OPENRTM_VERSION}/omniORB/${OMNI_VERSION1}.${OMNI_VERSION2}.${OMNI_MINOR}_${RTM_VC_VER}/)
		
		foreach(library ${ORB_LIBRARIES};${ORBSSL_LIBRARIES};${ORBHTTP_LIBRARIES})
			if((NOT (${library} STREQUAL "optimized")) AND (NOT (${library} STREQUAL "debug")))
				string(REPLACE "_rt" "*" dll "${library}")
				file(GLOB OMNIORB_DLLS "${OMNIORB_BINARY_DIR}/${dll}.dll")
				install(PROGRAMS ${OMNIORB_DLLS} DESTINATION ${ORB_INSTALL_DIR}/bin/${ARCH_NAME} COMPONENT corbalib)
				file(GLOB OMNIORB_PDBS "${OMNIORB_BINARY_DIR}/${dll}.pdb")
				install(PROGRAMS ${OMNIORB_PDBS} DESTINATION ${ORB_INSTALL_DIR}/bin/${ARCH_NAME} COMPONENT corbalib)
				
				install(PROGRAMS "${ORB_LINK_DIR}/${library}.lib" DESTINATION ${ORB_INSTALL_DIR}/lib/${ARCH_NAME} COMPONENT corbalib)
			endif()
		endforeach()

		file(GLOB OMNIORB_HEADERFILES1 "${ORB_ROOT}/include/*.h")
		file(GLOB OMNIORB_HEADERFILES2 "${ORB_ROOT}/include/*/*.h")
		file(GLOB OMNIORB_HEADERFILES3 "${ORB_ROOT}/include/*/*/*.h")
		file(GLOB OMNIORB_HEADERFILES4 "${ORB_ROOT}/include/*.hh")
		file(GLOB OMNIORB_HEADERFILES5 "${ORB_ROOT}/include/*/*.hh")
		file(GLOB OMNIORB_HEADERFILES6 "${ORB_ROOT}/include/*/*/*.hh")
		file(GLOB OMNIORB_HEADERFILES7 "${ORB_ROOT}/include/*.hxx")
		file(GLOB OMNIORB_HEADERFILES8 "${ORB_ROOT}/include/*/*.hxx")
		file(GLOB OMNIORB_HEADERFILES9 "${ORB_ROOT}/include/*/*/*.hxx")

		set(OMNIORB_HEADERFILES ${OMNIORB_HEADERFILES1} ${OMNIORB_HEADERFILES2} ${OMNIORB_HEADERFILES3}
					${OMNIORB_HEADERFILES4} ${OMNIORB_HEADERFILES5} ${OMNIORB_HEADERFILES6}
					${OMNIORB_HEADERFILES7} ${OMNIORB_HEADERFILES8} ${OMNIORB_HEADERFILES9})

		foreach(file_path ${OMNIORB_HEADERFILES})
			get_filename_component(dir_path "${file_path}" DIRECTORY)
			string(REPLACE "${ORB_ROOT}/include" "" file "${dir_path}")
			install(FILES ${file_path} DESTINATION ${ORB_INSTALL_DIR}/include/${file} COMPONENT corbalib)
		endforeach()


		file(GLOB OMNIORB_IDLFILES1 "${ORB_ROOT}/idl/*.idl")
		file(GLOB OMNIORB_IDLFILES2 "${ORB_ROOT}/idl/*/*.idl")

		set(OMNIORB_IDLFILES ${OMNIORB_IDLFILES1} ${OMNIORB_IDLFILES2})
		foreach(file_path ${OMNIORB_IDLFILES})
			get_filename_component(dir_path "${file_path}" DIRECTORY)
			string(REPLACE "${ORB_ROOT}/idl" "" file "${dir_path}")
			install(FILES ${file_path} DESTINATION ${ORB_INSTALL_DIR}/idl/${file} COMPONENT corbalib)
		endforeach()

		file(GLOB OMNIORB_SCRIPTS "${ORB_ROOT}/bin/scripts/*.py")
		
		install(FILES ${OMNIORB_SCRIPTS} DESTINATION ${ORB_INSTALL_DIR}/bin/scripts COMPONENT corbalib)
		install(DIRECTORY ${ORB_ROOT}/lib/python/omniidl DESTINATION ${ORB_INSTALL_DIR}/lib/python/ COMPONENT corbalib FILES_MATCHING PATTERN "*.py" PATTERN "__pycache__" EXCLUDE)
		install(DIRECTORY ${ORB_ROOT}/lib/python/omniidl_be DESTINATION ${ORB_INSTALL_DIR}/lib/python/ COMPONENT corbalib FILES_MATCHING PATTERN "*.py" PATTERN "__pycache__" EXCLUDE)

		install(PROGRAMS ${ORB_ROOT}/bin/${ARCH_NAME}/omniidl.exe DESTINATION ${ORB_INSTALL_DIR}/bin/${ARCH_NAME} COMPONENT corbalib)
		install(PROGRAMS ${ORB_ROOT}/bin/${ARCH_NAME}/omniNames.exe DESTINATION ${ORB_INSTALL_DIR}/bin/${ARCH_NAME} COMPONENT corbalib)



		set(ORB_INCLUDE_DIR ${ORB_ROOT}/include)
	endif(VXWORKS)
elseif(CORBA STREQUAL "TAO")
	set(CORBA_NAME "TAO" CACHE STRING "CORBA name.")
	
	if(VXWORKS)
		if("${VX_CPU_FAMILY}" STREQUAL "ppc")
			set(ORB_C_FLAGS_LIST ${ORB_C_FLAGS_LIST} -D__powerpc__)
		else()
			set(ORB_C_FLAGS_LIST ${ORB_C_FLAGS_LIST} -D__x86__)
		endif()
		set(ORB_C_FLAGS_LIST ${ORB_C_FLAGS_LIST} -DACE_VXWORKS=0x690 -DACE_HAS_PTHREADS -DACE_HAS_AIO_CALLS -DACE_LACKS_ISBLANK)

		set(ORB_LINK_DIR ${ORB_ROOT}/lib)
		set(ORB_LIBRARIES TAO_AnyTypeCode TAO_PortableServer TAO_Svc_Utils TAO_CosNaming TAO_DynamicInterface TAO_PI TAO_Utils TAO_PortableServer TAO_DiffServPolicy TAO_DynamicInterface TAO_Strategies TAO ACE)
		set(ORB_INCLUDE_DIR ${ORB_ROOT} ${ORB_ROOT}/TAO ${ORB_ROOT}/TAO/orbsvcs)
	elseif(UNIX)
		if(ORB_ROOT)
			set(ORB_LINK_DIR ${ORB_ROOT}/lib)
			set(ORB_LIBRARIES ACE TAO TAO_AnyTypeCode TAO_PortableServer TAO_Svc_Utils TAO_CosNaming TAO_IORTable TAO_CodecFactory TAO_PI TAO_Utils TAO_Strategies)
			set(ORB_INCLUDE_DIR ${ORB_ROOT}/include)
		else(ORB_ROOT)
			set(ORB_LINK_DIR ${ACE_TAO_LIBRARY_DIRS})
			set(ORB_LIBRARIES ${ACE_TAO_LIBRARIES})
			set(ORB_INCLUDE_DIR ${ACE_TAO_INCLUDE_DIRS})
		endif(ORB_ROOT)
	elseif(MSVC)
		set(ORB_LINK_DIR ${ORB_ROOT}/lib)
		set(ORB_LIBRARIES1 optimized;ACE;optimized;TAO;optimized;TAO_AnyTypeCode;optimized;TAO_PortableServer;optimized;TAO_Svc_Utils;optimized;TAO_CosNaming;optimized;TAO_IORTable;optimized;TAO_CodecFactory;optimized;TAO_PI;optimized;TAO_Utils;optimized;TAO_Strategies;debug;ACEd;debug;TAOd;debug;TAO_AnyTypeCoded;debug;TAO_PortableServerd;debug;TAO_Svc_Utilsd;debug;TAO_CosNamingd;debug;TAO_IORTabled;debug;TAO_CodecFactoryd;debug;TAO_PId;debug;TAO_Utilsd;debug;TAO_Strategiesd)
		set(ORB_LIBRARIES2 iphlpapi)
		set(ORB_LIBRARIES ${ORB_LIBRARIES1};${ORB_LIBRARIES2})

		set(ORB_INCLUDE_DIR ${ORB_ROOT} ${ORB_ROOT}/TAO ${ORB_ROOT}/TAO/orbsvcs)
		set(ORB_C_FLAGS_LIST ${ORB_C_FLAGS_LIST} -DWIN32_LEAN_AND_MEAN -DWITH_ACE)

		set(ORB_LIBRARIES_INSTALL ${ORB_LIBRARIES1};TAO_IDL_FE;TAO_IDL_FEd;TAO_IDL_BE;TAO_IDL_BEd;)
		set(ORB_INSTALL_DIR ${OPENRTM_VERSION}/ACE/${RTM_VC_VER}/)
		foreach(library ${ORB_LIBRARIES_INSTALL})
			if((NOT (${library} STREQUAL "optimized")) AND (NOT (${library} STREQUAL "debug")))
				install(PROGRAMS "${ORB_LINK_DIR}/${library}.dll" DESTINATION ${ORB_INSTALL_DIR}/bin/ COMPONENT corbalib)
				install(PROGRAMS "${ORB_LINK_DIR}/${library}.lib" DESTINATION ${ORB_INSTALL_DIR}/lib/ COMPONENT corbalib)
			endif()
		endforeach()

		add_compile_options(/wd4244)
		add_compile_options(/wd4996)

		file(GLOB ACE_HEADERFILES1 "${ORB_ROOT}/ace/*.h")
		file(GLOB ACE_HEADERFILES2 "${ORB_ROOT}/ace/*/*.h")
		file(GLOB ACE_HEADERFILES3 "${ORB_ROOT}/ace/*/*/*.h")
		file(GLOB ACE_HEADERFILES4 "${ORB_ROOT}/ace/*/*/*/*.h")
		file(GLOB ACE_HEADERFILES5 "${ORB_ROOT}/ace/*/*/*/*/*.h")

		file(GLOB ACE_HEADERFILES6 "${ORB_ROOT}/ace/*.inl")
		file(GLOB ACE_HEADERFILES7 "${ORB_ROOT}/ace/*/*.inl")
		file(GLOB ACE_HEADERFILES8 "${ORB_ROOT}/ace/*/*/*.inl")
		file(GLOB ACE_HEADERFILES9 "${ORB_ROOT}/ace/*/*/*/*.inl")
		file(GLOB ACE_HEADERFILES10 "${ORB_ROOT}/ace/*/*/*/*/*.inl")

		file(GLOB ACE_HEADERFILES11 "${ORB_ROOT}/ace/*.cpp")


		file(GLOB ACE_HEADERFILES16 "${ORB_ROOT}/TAO/tao/*.h")
		file(GLOB ACE_HEADERFILES17 "${ORB_ROOT}/TAO/tao/*/*.h")
		file(GLOB ACE_HEADERFILES18 "${ORB_ROOT}/TAO/tao/*/*/*.h")
		file(GLOB ACE_HEADERFILES19 "${ORB_ROOT}/TAO/tao/*/*/*/*.h")
		file(GLOB ACE_HEADERFILES20 "${ORB_ROOT}/TAO/tao/*/*/*/*/*.h")

		file(GLOB ACE_HEADERFILES21 "${ORB_ROOT}/TAO/tao/*.inl")
		file(GLOB ACE_HEADERFILES22 "${ORB_ROOT}/TAO/tao/*/*.inl")
		file(GLOB ACE_HEADERFILES23 "${ORB_ROOT}/TAO/tao/*/*/*.inl")
		file(GLOB ACE_HEADERFILES24 "${ORB_ROOT}/TAO/tao/*/*/*/*.inl")
		file(GLOB ACE_HEADERFILES25 "${ORB_ROOT}/TAO/tao/*/*/*/*/*.inl")

		file(GLOB ACE_HEADERFILES26 "${ORB_ROOT}/TAO/tao/*.cpp")
		file(GLOB ACE_HEADERFILES27 "${ORB_ROOT}/TAO/tao/*/*.cpp")


		file(GLOB ACE_HEADERFILES31 "${ORB_ROOT}/TAO/orbsvcs/*.h")
		file(GLOB ACE_HEADERFILES32 "${ORB_ROOT}/TAO/orbsvcs/*/*.h")
		file(GLOB ACE_HEADERFILES33 "${ORB_ROOT}/TAO/orbsvcs/*/*/*.h")

		file(GLOB ACE_HEADERFILES36 "${ORB_ROOT}/TAO/orbsvcs/*.inl")
		file(GLOB ACE_HEADERFILES37 "${ORB_ROOT}/TAO/orbsvcs/*/*.inl")



		file(GLOB ACE_HEADERFILES46 "${ORB_ROOT}/TAO/tao/*.pidl")
		file(GLOB ACE_HEADERFILES47 "${ORB_ROOT}/TAO/tao/*/*.pidl")
		file(GLOB ACE_HEADERFILES48 "${ORB_ROOT}/TAO/tao/*/*/*.pidl")
		file(GLOB ACE_HEADERFILES49 "${ORB_ROOT}/TAO/tao/*/*/*/*.pidl")
		file(GLOB ACE_HEADERFILES50 "${ORB_ROOT}/TAO/tao/*/*/*/*/*.pidl")


		set(ACE_HEADERFILES ${ACE_HEADERFILES1} ${ACE_HEADERFILES2} ${ACE_HEADERFILES3}
					${ACE_HEADERFILES4} ${ACE_HEADERFILES5} ${ACE_HEADERFILES6}
					${ACE_HEADERFILES7} ${ACE_HEADERFILES8} ${ACE_HEADERFILES9}
					${ACE_HEADERFILES10} ${ACE_HEADERFILES11} ${ACE_HEADERFILES12}
					${ACE_HEADERFILES13} ${ACE_HEADERFILES14} ${ACE_HEADERFILES15}
					${ACE_HEADERFILES16} ${ACE_HEADERFILES17} ${ACE_HEADERFILES18}
					${ACE_HEADERFILES19} ${ACE_HEADERFILES20} ${ACE_HEADERFILES21}
					${ACE_HEADERFILES22} ${ACE_HEADERFILES23} ${ACE_HEADERFILES24}
					${ACE_HEADERFILES25} ${ACE_HEADERFILES26} ${ACE_HEADERFILES27}
					${ACE_HEADERFILES28} ${ACE_HEADERFILES29} ${ACE_HEADERFILES30}
					${ACE_HEADERFILES31} ${ACE_HEADERFILES32} ${ACE_HEADERFILES33}
					${ACE_HEADERFILES34} ${ACE_HEADERFILES35} ${ACE_HEADERFILES36}
					${ACE_HEADERFILES37} ${ACE_HEADERFILES38} ${ACE_HEADERFILES39}
					${ACE_HEADERFILES40} ${ACE_HEADERFILES41} ${ACE_HEADERFILES42}
					${ACE_HEADERFILES43} ${ACE_HEADERFILES44} ${ACE_HEADERFILES45}
					${ACE_HEADERFILES46} ${ACE_HEADERFILES47} ${ACE_HEADERFILES48}
					${ACE_HEADERFILES49} ${ACE_HEADERFILES50})


		foreach(file_path ${ACE_HEADERFILES})
			get_filename_component(dir_path "${file_path}" DIRECTORY)
			string(REPLACE "${ORB_ROOT}/" "" file "${dir_path}")
			install(FILES ${file_path} DESTINATION ${ORB_INSTALL_DIR}/${file} COMPONENT corbalib)
		endforeach()



		file(GLOB TAO_IDLFILES1 "${ORB_ROOT}/TAO/*.idl")
		file(GLOB TAO_IDLFILES2 "${ORB_ROOT}/TAO/*/*.idl")

		set(TAO_IDLFILES ${TAO_IDLFILES1} ${TAO_IDLFILES2})
		foreach(file_path ${TAO_IDLFILES})
			get_filename_component(dir_path "${file_path}" DIRECTORY)
			string(REPLACE "${ORB_ROOT}/" "" file "${dir_path}")
			install(FILES ${file_path} DESTINATION ${ORB_INSTALL_DIR}/${file} COMPONENT corbalib)
		endforeach()


		install(PROGRAMS ${ORB_ROOT}/bin/tao_idl.exe DESTINATION ${ORB_INSTALL_DIR}/bin/ COMPONENT corbalib)
		install(PROGRAMS ${ORB_ROOT}/bin/ace_gperf.exe DESTINATION ${ORB_INSTALL_DIR}/bin/ COMPONENT corbalib)
		install(PROGRAMS ${ORB_ROOT}/TAO/orbsvcs/Naming_Service/tao_cosnaming.exe DESTINATION ${ORB_INSTALL_DIR}/bin/${ARCH_NAME} COMPONENT corbalib)


	endif()
elseif(CORBA STREQUAL "RtORB")
	set(CORBA_NAME "RtORB" CACHE STRING "CORBA name.")
	if(VXWORKS)
	elseif(UNIX)
		if(ORB_ROOT)
			set(ORB_LINK_DIR ${ORB_ROOT}/lib)
			set(ORB_LIBRARIES RtORB RtORB_cpp)
			set(ORB_INCLUDE_DIR ${ORB_ROOT}/include)
		else(ORB_ROOT)
			set(ORB_LIBRARIES RtORB RtORB_cpp)
		endif(ORB_ROOT)
	elseif(MSVC)
	endif()
endif()



if(VXWORKS)

	set(ORB_LIBRARIES_PATH)
	FOREACH(NAME ${ORB_LIBRARIES})
		set(ORB_LIB ORB_${NAME}_LIBRARY)
		LIST(APPEND ORB_LIBVARS ${ORB_LIB})
		FIND_LIBRARY(${ORB_LIB} ${NAME}
			PATHS ${ORB_LINK_DIR}
			CMAKE_FIND_ROOT_PATH_BOTH)
		if(${ORB_LIB})
			LIST(APPEND ORB_LIBRARIES_PATH ${${ORB_LIB}})
		endif(${ORB_LIB})
	ENDFOREACH(NAME)

	set(ORB_LIBRARIES ${ORB_LIBRARIES_PATH})
else(VXWORKS)
endif(VXWORKS)


#============================================================
# OS type selection
# - COIL_OS_DIR settings
# - Compile/linker option settings
# - RTM_OS_VXWORKS setting <- is it necessary?
#============================================================
if (UNIX)
	set (COIL_OS_DIR "posix")
	if     (CMAKE_SYSTEM_NAME MATCHES "Darwin")
		set (RTM_OS_DARWIN ON)
	elseif (CMAKE_SYSTEM_NAME MATCHES "FreeBSD")
		set (RTM_OS_FREEBSD ON)
	elseif (CMAKE_SYSTEM_NAME MATCHES "Linux")
		set (RTM_OS_LINUX ON)
		# for std::thread
		set (THREADS_PREFER_PTHREAD_FLAG ON)
		find_package(Threads)
		set (RTM_LINKER_OPTION ${CMAKE_DL_LIBS} rt Threads::Threads)
	elseif (CMAKE_SYSTEM_NAME MATCHES "QNX") 
		set(RTM_OS_QNX ON)
	else ()
		message(FATAL_ERROR "Unknown Operating System")
	endif ()
elseif (WIN32)
	set (COIL_OS_DIR "win32")
	set (RTM_OS_WIN32 ON)
	set (COIL_C_FLAGS_LIST "-D_WIN32_WINNT=0x0603")
	set (RTM_LINKER_OPTION advapi32.lib;ws2_32.lib;mswsock.lib;rpcrt4.lib;optimized;msvcrt.lib;optimized;msvcmrt.lib;debug;msvcrtd.lib)
elseif (VXWORKS)
	set (COIL_OS_DIR "vxworks")
	set (RTM_OS_VXWORKS ON)
endif ()

function(openrtm_include_rtm target)
	target_include_directories(${target}
		PRIVATE ${CMAKE_SOURCE_DIR}/src/lib/coil/common
		PRIVATE ${CMAKE_SOURCE_DIR}/src/lib/coil/${COIL_OS_DIR}
		PRIVATE ${CMAKE_SOURCE_DIR}/src/lib)
	target_include_directories(${target} SYSTEM
		PRIVATE ${CMAKE_BINARY_DIR}/src/lib/rtm/idl
		PRIVATE ${CMAKE_BINARY_DIR}/src/lib
		${ORB_INCLUDE_DIR})
	add_dependencies(${target} ${RTCSKEL_PROJECT_NAME}_IDLTGT)
endfunction()

function(openrtm_common_set_compile_definitions target)
	target_compile_definitions(${target} PRIVATE
		$<$<CXX_COMPILER_ID:MSVC>:
			# MSVC 2013 does not support noexcept.
			$<$<VERSION_LESS:${CMAKE_CXX_COMPILER_VERSION},19.00>:
				_ALLOW_KEYWORD_MACROS
				noexcept=_NOEXCEPT
			>
		>
		$<$<PLATFORM_ID:Windows>:NOMINMAX> # disable min/max macro in Windows.h
	)
endfunction()

function(openrtm_common_set_compile_options target)
	# Warning options
	set(WARN_GCC
		-pedantic
		-Wall
		-Wextra
		$<$<VERSION_GREATER:${CMAKE_CXX_COMPILER_VERSION},6.9.9>: -Walloc-zero>
		$<$<VERSION_GREATER:${CMAKE_CXX_COMPILER_VERSION},6.9.9>: -Walloca>
		$<$<VERSION_GREATER:${CMAKE_CXX_COMPILER_VERSION},4.9.9>: -Warray-bounds=2>
		$<$<VERSION_GREATER:${CMAKE_CXX_COMPILER_VERSION},7.9.9>: -Wcast-align=strict>
		-Wcast-qual
		$<$<VERSION_GREATER:${CMAKE_CXX_COMPILER_VERSION},7.9.9>: -Wcatch-value=3>
		$<$<VERSION_GREATER:${CMAKE_CXX_COMPILER_VERSION},7.9.9>: -Wclass-memaccess>
		-Wcomments
		-Wconditionally-supported
		-Wconversion
		-Wctor-dtor-privacy
		-Wdate-time
		-Wdelete-non-virtual-dtor
		-Wdisabled-optimization
		-Wdouble-promotion
		$<$<VERSION_GREATER:${CMAKE_CXX_COMPILER_VERSION},6.9.9>: -Wduplicated-branches>
		$<$<VERSION_GREATER:${CMAKE_CXX_COMPILER_VERSION},5.9.9>: -Wduplicated-cond>
		$<$<VERSION_GREATER:${CMAKE_CXX_COMPILER_VERSION},7.9.9>: -Wextra-semi>
		-Wfloat-conversion
		-Wfloat-equal
		-Wformat
		-Wformat-nonliteral
		$<$<VERSION_GREATER:${CMAKE_CXX_COMPILER_VERSION},6.9.9>: -Wformat-overflow=2>
		-Wformat-security
		$<$<VERSION_GREATER:${CMAKE_CXX_COMPILER_VERSION},4.9.9>: -Wformat-signedness>
		$<$<VERSION_GREATER:${CMAKE_CXX_COMPILER_VERSION},6.9.9>: -Wformat-truncation=2>
		-Wformat-y2k
		-Wformat=2
		-Wframe-larger-than=9000		# size change 4000 to 9000 so RTPreemptEC.cpp larger than 4000 bytes.
		$<$<VERSION_GREATER:${CMAKE_CXX_COMPILER_VERSION},6.9.9>: -Wimplicit-fallthrough=3>
		-Winit-self
		-Winline
		-Winvalid-pch
		-Wlogical-op
		-Wmissing-declarations
		-Wmissing-format-attribute
		-Wmissing-include-dirs
		-Wmultichar
		-Wnoexcept
		$<$<VERSION_GREATER:${CMAKE_CXX_COMPILER_VERSION},6.9.9>: -Wnoexcept-type>
		-Wnon-virtual-dtor
		$<$<VERSION_GREATER:${CMAKE_CXX_COMPILER_VERSION},5.9.9>: -Wnull-dereference>
		-Wold-style-cast
		-Woverlength-strings
		-Woverloaded-virtual
		-Wpacked
		-Wpedantic
		$<$<VERSION_GREATER:${CMAKE_CXX_COMPILER_VERSION},5.9.9>: -Wplacement-new=1>
		-Wpointer-arith
		-Wredundant-decls
		$<$<VERSION_GREATER:${CMAKE_CXX_COMPILER_VERSION},6.9.9>: -Wregister>
		-Wreorder
		$<$<VERSION_GREATER:${CMAKE_CXX_COMPILER_VERSION},5.9.9>: -Wscalar-storage-order>
		-Wshadow
		$<$<VERSION_GREATER:${CMAKE_CXX_COMPILER_VERSION},6.9.9>: -Wshadow-compatible-local>
		$<$<VERSION_GREATER:${CMAKE_CXX_COMPILER_VERSION},6.9.9>: -Wshadow-local>
		$<$<VERSION_GREATER:${CMAKE_CXX_COMPILER_VERSION},6.9.9>: -Wshadow=compatible-local>
		$<$<VERSION_GREATER:${CMAKE_CXX_COMPILER_VERSION},6.9.9>: -Wshadow=global>
		$<$<VERSION_GREATER:${CMAKE_CXX_COMPILER_VERSION},6.9.9>: -Wshadow=local>
		$<$<VERSION_GREATER:${CMAKE_CXX_COMPILER_VERSION},5.9.9>: -Wshift-overflow=1>
		-Wsign-conversion
		-Wsign-promo
		-Wstack-protector
		-Wstrict-null-sentinel
		-Wstrict-overflow=5
		$<$<VERSION_GREATER:${CMAKE_CXX_COMPILER_VERSION},6.9.9>: -Wstringop-overflow=4>
		$<$<VERSION_GREATER:${CMAKE_CXX_COMPILER_VERSION},13.9.9>: -Wsuggest-attribute=cold>
		-Wsuggest-attribute=format
		$<$<VERSION_GREATER:${CMAKE_CXX_COMPILER_VERSION},7.9.9>: -Wsuggest-attribute=malloc>
		$<$<VERSION_GREATER:${CMAKE_CXX_COMPILER_VERSION},4.9.9>: -Wsuggest-override>
		-Wswitch-default
		-Wswitch-enum
		-Wsynth
		-Wtrampolines
		-Wundef
		-Wunreachable-code
		$<$<VERSION_GREATER:${CMAKE_CXX_COMPILER_VERSION},5.9.9>: -Wunused-const-variable=2>
		-Wunused-macros
		-Wuseless-cast
		-Wvariadic-macros
		-Wvector-operation-performance
		-Wwrite-strings
		-Wzero-as-null-pointer-constant
		# exclude rule
		-Wno-padded)
	set(WARN_GCC_RELEASE
		${WARN_GCC}
		-Wno-cast-qual
		-Wno-conditionally-supported
		-Wno-conversion
		-Wno-missing-declarations
		-Wno-old-style-cast
		-Wno-overloaded-virtual
		-Wno-shadow
		-Wno-sign-compare
		-Wno-sign-conversion
		-Wno-strict-overflow
		-Wno-suggest-attribute=format
		$<$<VERSION_GREATER:${CMAKE_CXX_COMPILER_VERSION},7.9.9>: -Wno-suggest-attribute=malloc>
		-Wno-unused-macros
		-Wno-unused-result
		-Wno-useless-cast
		)
	set(WARN_CLANG
		-Weverything
		# exclude rule
		-Wno-documentation
		-Wno-documentation-unknown-command
		-Wno-c++98-compat
		-Wno-c++98-compat-pedantic
		-Wno-padded)
	set(WARN_CLANG_RELEASE
		${WARN_CLANG}
		-Wno-cast-qual
		-Wno-conversion
		-Wno-covered-switch-default
		-Wno-deprecated
		-Wno-disabled-macro-expansion
		-Wno-double-promotion
		-Wno-exit-time-destructors
		-Wno-format-nonliteral
		-Wno-global-constructors
		-Wno-header-hygiene
		-Wno-inconsistent-missing-destructor-override
		-Wno-missing-prototypes
		-Wno-missing-variable-declarations
		-Wno-old-style-cast
		-Wno-reserved-id-macro
		-Wno-shadow-field-in-constructor
		-Wno-shadow-field
		-Wno-shorten-64-to-32
		-Wno-sign-compare
		-Wno-sign-conversion
		-Wno-switch-enum
		-Wno-tautological-unsigned-zero-compare
		-Wno-undefined-reinterpret-cast
		-Wno-unreachable-code-break
		-Wno-unreachable-code-return
		-Wno-unreachable-code
		-Wno-unused-local-typedef
		-Wno-unused-macros
		-Wno-weak-vtables
		)

	  target_compile_options(${target} PRIVATE
		# multi-process build
		$<$<CXX_COMPILER_ID:MSVC>:/MP>

		# warning options
		$<$<CXX_COMPILER_ID:GNU>:
			$<$<CONFIG:Debug>:-O2 ${WARN_GCC}> # Some options need O2.
			$<$<CONFIG:Release>:-Werror ${WARN_GCC_RELEASE}>
			$<$<CONFIG:RelWithDebInfo>: ${WARN_GCC_RELEASE}>
			$<$<CONFIG:MinSizeRel>: ${WARN_GCC_RELEASE}>
		>

		$<$<CXX_COMPILER_ID:Clang>:
			$<$<CONFIG:Debug>:${WARN_CLANG}>
			$<$<CONFIG:Release>:-Werror ${WARN_CLANG_RELEASE}>
			$<$<CONFIG:RelWithDebInfo>: ${WARN_CLANG_RELEASE}>
			$<$<CONFIG:MinSizeRel>: ${WARN_CLANG_RELEASE}>
		>

		$<$<CXX_COMPILER_ID:MSVC>:
			$<$<CONFIG:Debug>:/W4>
			$<$<CONFIG:Release>:/W4 /WX>
			$<$<CONFIG:RelWithDebInfo>:/Wall>
			$<$<CONFIG:MinSizeRel>:/W4 /WX>
			/wd4589 # for omniORB.
			$<$<VERSION_LESS:${CMAKE_CXX_COMPILER_VERSION},19.00>:/wd4127> # for omniORB.
			$<$<VERSION_LESS:${CMAKE_CXX_COMPILER_VERSION},19.00>:/wd4510> # for Macho.
			$<$<VERSION_LESS:${CMAKE_CXX_COMPILER_VERSION},19.00>:/wd4512> # for omniORB.
			$<$<VERSION_LESS:${CMAKE_CXX_COMPILER_VERSION},19.00>:/wd4610> # for Macho.
		>
	)
endfunction()

function(openrtm_common_set_compile_props target)
	openrtm_common_set_compile_definitions(${target})
	openrtm_common_set_compile_options(${target})
	set_target_properties(${target} PROPERTIES
		POSITION_INDEPENDENT_CODE ON
		CXX_STANDARD 11
		CXX_EXTENSIONS OFF
		CXX_STANDARD_REQUIRED ON
	)
endfunction()

function(openrtm_gencode_set_compile_definitions target)
  target_compile_definitions(${target} PRIVATE
		$<$<PLATFORM_ID:Windows>:NOMINMAX> # disable min/max macro in Windows.h
	)
endfunction()

function(openrtm_gencode_set_compile_options target)
	target_compile_options(${target} PRIVATE
		# multi-process build
		$<$<CXX_COMPILER_ID:MSVC>:/MP>
		# warning options
		$<$<CXX_COMPILER_ID:MSVC>:/W0>
	)
endfunction()

function(openrtm_gencode_set_compile_props target)
	openrtm_gencode_set_compile_definitions(${target})
	openrtm_gencode_set_compile_options(${target})
	set_target_properties(${target} PROPERTIES
		POSITION_INDEPENDENT_CODE ON
		CXX_STANDARD 11
		CXX_EXTENSIONS OFF
		CXX_STANDARD_REQUIRED ON
	)
endfunction()

function(openrtm_idl_compile idl_name inc_path output category dependencies)
	set(skel_wrapper ${OpenRTM_aist_BINARY_DIR}/rtm2-skelwrapper)
	if(CORBA STREQUAL "ORBexpress")
		set(idl_flags -m -a -ex -fhh -fxcc -fc_c -fs -q -dOE_IDL -I${ORB_ROOT}/idl)
	elseif (CORBA STREQUAL "omniORB")
		set(idl_flags -bcxx -Wba -nf -Wbshortcut)
	elseif(CORBA STREQUAL "TAO")
		if(UNIX)
			if(NOT ORB_ROOT)
				 set(idl_flags -as -DTAO_IDL -I${ACE_TAO_DIR}/include)
			else()
				set(idl_flags -as -DTAO_IDL -I${ORB_ROOT}/include)
			endif()
		else()
			set(idl_flags -as -DTAO_IDL -I${ORB_ROOT}/TAO)
		endif()
	elseif(CORBA STREQUAL "RtORB")
		set(idl_flags --skeleton-impl)
	endif()

	add_custom_command(
		OUTPUT ${output}
		COMMAND ${IDL_COMPILE_COMMAND} ${idl_flags} ${inc_path} ${CMAKE_CURRENT_SOURCE_DIR}/${idl_name}
		COMMAND ${Python3_EXECUTABLE} -B ${skel_wrapper} --include-dir="" --skel-suffix=Skel --stub-suffix=Stub --idl-file=${idl_name} --category=${category} --dependencies=${dependencies}
		DEPENDS ${idl_name}
		WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
endfunction()

# return values
## TARGET
### - ${name}_IDLTGT : idl compile target
## VALIABLE
### - ${name}_IDLSTUB : stub file generated by skel wrapper
### - ${name}_IDLSKEL : skel file generated by skel wrapper
### - ${name}_IDLGEN  : .cpp generated by IDL compiler
### - ${name}_IDLHDR  : all generated header files
function(openrtm_idl_compile_all name idls inc_path categoty dependencies)
	foreach(idl ${idls})
		# set generated file name
		STRING(REPLACE ".idl" "" base "${idl}")
		set(stub ${base}Stub.cpp)
		set(skel ${base}Skel.cpp)
		set(stub_h ${base}Stub.h)
		set(skel_h ${base}Skel.h)
		if(CORBA STREQUAL "ORBexpress")
			set(h1 ${base}.h)
			set(h2 ${base}_c.h)
			set(s1 ${base}.cc)
			set(s2 ${base}_c.cc)
		elseif(CORBA STREQUAL "omniORB")
			set(h1 ${base}.hh)
			set(s1 ${base}SK.cc)
			set(s2 ${base}DynSK.cc)
		elseif(CORBA STREQUAL "TAO")
			set(h1 ${base}S.h)
			set(h2 ${base}C.h)
			set(h3 ${base}C.inl)
			set(s1 ${base}S.cpp)
			set(s2 ${base}C.cpp)
		elseif(CORBA STREQUAL "RtORB")
			set(h1 ${base}.h)
			set(h2 ${base}-decls.h)
			set(s1 ${base}-skels.c)
			set(s2 ${base}-common.c)
			set(s3 ${base}-stubs.c)
			set(s3 ${base}-skelimpl.c)
		endif()
		set(output
			${stub} ${skel} ${s1} ${s2}
			${stub_h} ${skel_h} ${h1} ${h2} ${h3})

		# register custom_command which generate source and header files.
		openrtm_idl_compile(${idl} "${inc_path}" "${output}" ${categoty} "${dependencies}")

		set(stub_list ${stub_list} ${stub})
		set(skel_list ${skel_list} ${skel})
		set(gen_list  ${gen_list}  ${s1} ${s2})
		set(hdr_list  ${hdr_list} ${stub_h} ${skel_h} ${h1} ${h2} ${h3})
		set(out_all ${out_all} ${output})
	endforeach()

	# register target of generated files
	add_custom_target(${name}_IDLTGT DEPENDS ${out_all})

	# set output variables.
	set(${name}_IDLSTUB ${stub_list} PARENT_SCOPE)
	set(${name}_IDLSKEL ${skel_list} PARENT_SCOPE)
	set(${name}_IDLGEN ${gen_list} PARENT_SCOPE)
	set(${name}_IDLHDR ${hdr_list} PARENT_SCOPE)
endfunction()

function(openrtm_set_link_props_shared target)
	if(${CMAKE_VERSION} VERSION_GREATER "3.12.99")
		target_link_options(${target}
			PRIVATE $<$<PLATFORM_ID:Windows>:/WX>
			PRIVATE $<$<PLATFORM_ID:Linux>:
				LINKER:--warn-common
				LINKER:--fatal-warnings
			>
		)
	endif()
endfunction()

function(openrtm_set_postfix target)
	if(DEFINED CMAKE_VS_PLATFORM_NAME)
		set_target_properties(${target} PROPERTIES
			RELEASE_POSTFIX ${RTM_SHORT_VERSION}_${RTM_VC_VER}_${CMAKE_VS_PLATFORM_NAME}
			DEBUG_POSTFIX   ${RTM_SHORT_VERSION}_${RTM_VC_VER}_${CMAKE_VS_PLATFORM_NAME}d)
	endif()
endfunction()

if(WIN32)
	set(MAKE_DEF_COMMAND ${CMAKE_SOURCE_DIR}/scripts/makedeffile.py)
endif(WIN32)

if(UNIX)
  if(ROS_ENABLE OR ROS2_ENABLE)
    set(INSTALL_ROS2TRANSPORT_CMAKE_DIR ${CMAKE_INSTALL_LIBDIR}/ros2transport-${RTM_MAJOR_VERSION}.${RTM_MINOR_VERSION}/cmake)
    if(ROS_ENABLE)
      set(INSTALL_ROSTRANSPORT_CMAKE_DIR ${CMAKE_INSTALL_LIBDIR}/rostransport-${RTM_MAJOR_VERSION}.${RTM_MINOR_VERSION}/cmake)
      configure_file(${PROJECT_SOURCE_DIR}/packages/deb/debian/rules.in
	 ${PROJECT_BINARY_DIR}/rules @ONLY
      )
    else()
      configure_file(${PROJECT_SOURCE_DIR}/packages/deb/debian/rules.ros2-only-support.in
	 ${PROJECT_BINARY_DIR}/rules @ONLY
      )
    endif()
    file(STRINGS "/etc/os-release" data_list REGEX "^(VERSION_CODENAME)=")
    # Look for lines like VERSION_CODENAME="..."
    foreach(var ${data_list})
      if("${var}" MATCHES "^(VERSION_CODENAME)=(.*)$")
        set(ubuntu_codename "${CMAKE_MATCH_2}")
      endif()
    endforeach()

    if(${ubuntu_codename} MATCHES "focal" OR
       ${ubuntu_codename} MATCHES "jammy")
      configure_file(${PROJECT_SOURCE_DIR}/src/ext/transport/FastRTPS/CORBACdrDataPubSubTypes.cpp.ubuntu2204_earlier
	      ${PROJECT_SOURCE_DIR}/src/ext/transport/FastRTPS/CORBACdrDataPubSubTypes.cpp @ONLY
      )
      configure_file(${PROJECT_SOURCE_DIR}/src/ext/transport/ROS2Transport/ROS2Serializer.h.ubuntu2204_earlier
	      ${PROJECT_SOURCE_DIR}/src/ext/transport/ROS2Transport/ROS2Serializer.h @ONLY
      )
    endif()

    if(BUILD_RTM_LINUX_PKGS)
      configure_file(${PROJECT_SOURCE_DIR}/packages/deb/debian/control.${ubuntu_codename}
             ${PROJECT_BINARY_DIR}/control @ONLY
      )
    endif()
  else()
    configure_file(${PROJECT_SOURCE_DIR}/packages/deb/debian/rules.not-ros-support.in
	 ${PROJECT_BINARY_DIR}/rules @ONLY
    )
    configure_file(${PROJECT_SOURCE_DIR}/packages/deb/debian/control.not-ros-support
         ${PROJECT_BINARY_DIR}/control @ONLY
    )
  endif()
endif(UNIX)

add_subdirectory(src)

set(UTILS_ENABLE TRUE CACHE BOOL "set UTILS_ENABLE ")
if(UTILS_ENABLE)
	add_subdirectory(utils)
endif(UTILS_ENABLE)

set(EXAMPLES_ENABLE TRUE CACHE BOOL "set EXAMPLES_ENABLE ")
if(EXAMPLES_ENABLE)
	add_subdirectory(examples)
endif(EXAMPLES_ENABLE)

add_subdirectory(etc)

set(DOCUMENTS_ENABLE FALSE CACHE BOOL "set DOCUMENTS_ENABLE")

if(DOCUMENTS_ENABLE)
	add_subdirectory(docs)
endif(DOCUMENTS_ENABLE)


CONFIGURE_FILE ("${PROJECT_SOURCE_DIR}/scripts/cmake/uninstall_target.cmake.in"
    "${PROJECT_BINARY_DIR}/uninstall_target.cmake" IMMEDIATE @ONLY)
ADD_CUSTOM_TARGET (remove "${CMAKE_COMMAND}" -P
    "${PROJECT_BINARY_DIR}/uninstall_target.cmake")

function(get_dist ARG0)
  if(NOT ${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set(${ARG0} ${CMAKE_SYSTEM_NAME} PARENT_SCOPE)
    return()
  endif()
  foreach(dist Debian Ubuntu RedHat Fedora CentOS Raspbian)
    execute_process(
      COMMAND grep ${dist} -s /etc/issue /etc/os-release /etc/redhat-release /etc/system-release
      OUTPUT_VARIABLE dist_name
      )
    if(${dist_name} MATCHES ${dist})
      set(${ARG0} ${dist} PARENT_SCOPE)
      return()
    endif()
  endforeach()
endfunction(get_dist)

get_dist(DIST_NAME)
message(STATUS "Distribution is ${DIST_NAME}")

