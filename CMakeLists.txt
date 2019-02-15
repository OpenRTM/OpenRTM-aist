cmake_minimum_required (VERSION 2.6)

project (OpenRTM_aist)






set(CORBA ${CORBA} CACHE STRING "set CORBA ")

if(NOT CORBA)
	message(FATAL_ERROR "Please set CORBA.")
endif()



set(ORB_ROOT ${ORB_ROOT} CACHE PATH "set ORB_ROOT")


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
			#set(ORB_ROOT "" CACHE PATH "set ORB_ROOT" FORCE)
		else(UNIX)
			message(FATAL_ERROR "Please set ORB_ROOT.")
		endif(UNIX)



	else()
		message(FATAL_ERROR "Please set ORB_ROOT.")
	endif()
endif()




if(MSVC)
	if (${CMAKE_GENERATOR} STREQUAL "Visual Studio 9 2008")
		set(RTM_VC_VER "vc9")
	endif()
	if (${CMAKE_GENERATOR} STREQUAL "Visual Studio 10 2010")
		set(RTM_VC_VER "vc10")
	endif()
	if (${CMAKE_GENERATOR} STREQUAL "Visual Studio 10 2010 Win64")
		set(RTM_VC_VER "vc10")
	endif()
	if (${CMAKE_GENERATOR} STREQUAL "Visual Studio 11 2012")
		set(RTM_VC_VER "vc11")
	endif()
	if (${CMAKE_GENERATOR} STREQUAL "Visual Studio 11 2012 Win64")
		set(RTM_VC_VER "vc11")
	endif()
	if (${CMAKE_GENERATOR} STREQUAL "Visual Studio 12 2013")
		set(RTM_VC_VER "vc12")
	endif()
	if (${CMAKE_GENERATOR} STREQUAL "Visual Studio 12 2013 Win64")
		set(RTM_VC_VER "vc12")
	endif()
	if (${CMAKE_GENERATOR} STREQUAL "Visual Studio 14 2015")
		set(RTM_VC_VER "vc14")
	endif()
	if (${CMAKE_GENERATOR} STREQUAL "Visual Studio 14 2015 Win64")
		set(RTM_VC_VER "vc14")
	endif()
	if (${CMAKE_GENERATOR} STREQUAL "Visual Studio 15 2017")
		set(RTM_VC_VER "vc141")
	endif()
	if (${CMAKE_GENERATOR} STREQUAL "Visual Studio 15 2017 Win64")
		set(RTM_VC_VER "vc141")
	endif()
	if (${CMAKE_GENERATOR} STREQUAL "Visual Studio 11 2012 ARM")
		set(RTM_VC_VER "vc11")
	endif()
	if (${CMAKE_GENERATOR} STREQUAL "Visual Studio 12 2013 ARM")
		set(RTM_VC_VER "vc12")
	endif()
	if (${CMAKE_GENERATOR} STREQUAL "Visual Studio 14 2015 ARM")
		set(RTM_VC_VER "vc14")
	endif()
	if (${CMAKE_GENERATOR} STREQUAL "Visual Studio 15 2017 ARM")
		set(RTM_VC_VER "vc141")
	endif()
endif(MSVC)



set(RTM_MAJOR_VERSION 2 CACHE STRING "RTM major version")
set(RTM_MINOR_VERSION 0 CACHE STRING "RTM minor version")
set(RTM_REVISION_NUM 0 CACHE STRING "RTM revision numver")
set(RTM_SHORT_VERSION 200 CACHE STRING "RTM short version")
set(RTM_VERSION 2.0.0 CACHE STRING "RTM version")
set(OPENRTM_VERSION ${RTM_MAJOR_VERSION}.${RTM_MINOR_VERSION}.${RTM_REVISION_NUM} CACHE STRING "RTM version.")
set(OPENRTM_NAME OpenRTM-aist-${OPENRTM_VERSION} CACHE STRING "RTM name.")


set(COIL_PROJECT_NAME coil${RTM_MAJOR_VERSION}${RTM_MINOR_VERSION}${RTM_REVISION_NUM})
set(RTM_PROJECT_NAME RTC${RTM_MAJOR_VERSION}${RTM_MINOR_VERSION}${RTM_REVISION_NUM})
set(RTCSKEL_PROJECT_NAME RTCSkel)

if(MSVC)
	set(COIL_PROJECT_NAME ${COIL_PROJECT_NAME}_${RTM_VC_VER})
	set(RTM_PROJECT_NAME ${RTM_PROJECT_NAME}_${RTM_VC_VER})
endif(MSVC)

if(MSVC)
	set(INSTALL_RTM_SCRIPT_DIR ${OPENRTM_VERSION}/bin/)

	set(INSTALL_RTM_BIN_DIR ${OPENRTM_VERSION}/bin/${RTM_VC_VER}/)
	set(INSTALL_RTM_INCLUDE_DIR ${OPENRTM_VERSION}/)
	set(INSTALL_COIL_INCLUDE_DIR ${OPENRTM_VERSION}/)
	set(INSTALL_RTM_LIB_DIR ${OPENRTM_VERSION}/bin/${RTM_VC_VER}/)
	set(INSTALL_RTM_EXAMPLE_DIR ${OPENRTM_VERSION}/Components/C++/Examples/${RTM_VC_VER}/)
	set(INSTALL_RTM_EXAMPLE_SHAREDLIB_DIR ${INSTALL_RTM_EXAMPLE_DIR})
	set(INSTALL_RTM_DOC_DIR ${OPENRTM_VERSION}/docs/)
	set(INSTALL_RTM_EXT_DIR ${OPENRTM_VERSION}/ext/)
	set(INSTALL_RTM_CMAKE_DIR ${OPENRTM_VERSION}/cmake/)
	set(INSTALL_RTM_ETC_DIR ${OPENRTM_VERSION}/ext/)
else(MSVC)
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




set(SSL_ENABLE ${SSL_ENABLE} CACHE BOOL "set SSL_ENABLE")
set(OBSERVER_ENABLE ${OBSERVER_ENABLE} CACHE BOOL "set OBSERVER_ENABLE")


if(CORBA STREQUAL "ORBexpress")
	if(NOT IDL_COMPILE_COMMAND)
		set(IDL_COMPILE_COMMAND ${ORB_ROOT}/host/x86_linux_2.6/bin/idl2cpp)
	endif()
	SET(IDLCOMMAND_FLAGS -m -a -ex -fhh -fxcc -fc_c -fs -q -dOE_IDL -I${ORB_ROOT}/idl )
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
			endif(VXWORKS)
		endif()
	endif()
	SET(IDLCOMMAND_FLAGS -bcxx -Wba -nf -Wbshortcut)
	#SET(IDLCOMMAND_FLAGS -bcxx -Wba -nf)

	set(OMNI_VERSION ${OMNI_VERSION} CACHE STRING "set OMNI_VERSION")
	if(UNIX AND OMNIORB_FOUND)
		string(REGEX REPLACE "([0-9]).([0-9]).([0-9])$" "\\1" OMNI_VERSION1 ${OMNIORB_VERSION})
		string(REGEX REPLACE "([0-9]).([0-9]).([0-9])$" "\\2" OMNI_VERSION2 ${OMNIORB_VERSION})
		set(OMNI_VERSION ${OMNI_VERSION1}${OMNI_VERSION2} CACHE PATH "set OMNI_VERSION" FORCE)
	else()

		set(OMNI_MINOR ${OMNI_MINOR} CACHE STRING "set OMNI_MINOR ")
		set(OMNITHREAD_VERSION ${OMNI_MINOR} CACHE STRING "set OMNITHREAD_VERSION ")
		string(REGEX REPLACE "([0-9])([0-9])$" "\\1" OMNI_VERSION1 ${OMNI_VERSION})
		string(REGEX REPLACE "([0-9])([0-9])$" "\\2" OMNI_VERSION2 ${OMNI_VERSION})
	endif()
elseif(CORBA STREQUAL "TAO")
	if(NOT IDL_COMPILE_COMMAND)
		if(test${ORB_ROOT} STREQUAL "test")
			set(IDL_COMPILE_COMMAND tao_idl)
		else()
			if(VXWORKS)
				message(FATAL_ERROR "Please set IDL_COMPILE_COMMAND.")
			elseif(UNIX)
				set(IDL_COMPILE_COMMAND ${ORB_ROOT}/bin/tao_idl)
			elseif(MSVC)
				set(IDL_COMPILE_COMMAND ${ORB_ROOT}/bin/tao_idl)
			endif(VXWORKS)
		endif()
	endif()
	if(UNIX)
		if(test${ORB_ROOT} STREQUAL "test")
			SET(IDLCOMMAND_FLAGS -as -DTAO_IDL -I/usr/include -I/usr/local/include)
		else()
			SET(IDLCOMMAND_FLAGS -as -DTAO_IDL -I${ORB_ROOT}/include)
		endif()
	elseif(VXWORKS)
		SET(IDLCOMMAND_FLAGS -as -DTAO_IDL -I${ORB_ROOT}/TAO)
	elseif(MSVC)
		SET(IDLCOMMAND_FLAGS -as -DTAO_IDL -I${ORB_ROOT}/TAO)
	endif()
endif()

if(UNIX)
	set(SKELWRAPPER_COMMAND ${CMAKE_SOURCE_DIR}/utils/rtm-skelwrapper/rtm-skelwrapper)
elseif(VXWORKS)
	set(SKELWRAPPER_COMMAND ${CMAKE_SOURCE_DIR}/utils/rtm-skelwrapper/rtm-skelwrapper)
elseif(MSVC)
	set(SKELWRAPPER_COMMAND ${CMAKE_SOURCE_DIR}/utils/rtm-skelwrapper/rtm-skelwrapper)
endif()








if(VXWORKS)
	if("${VX_CPU_FAMILY}" STREQUAL "ppc")
		SET (CPU_TYPE powerpc)
	elseif("${VX_CPU_FAMILY}" STREQUAL "simpentium")
		SET (CPU_TYPE simpentium)
	elseif("${VX_CPU_FAMILY}" STREQUAL "simlinux")
		SET (CPU_TYPE simlinux)
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
		#set(OE_TARGET gcc_4.1.2_Lc)
		#set(OE_TYPE config_rt_fast)
		set(OE_TYPE config_rt_full)
		#set(OE_TYPE config_rt_minimum)
		#set(OE_TYPE config_rt_small)
		set(ORB_LINK_DIR ${ORB_ROOT}/target/ppc_8xx-vxworks_6.6/${OE_TARGET}/config_rt_full/lib ${ORB_ROOT}/target/ppc_8xx-vxworks_6.6/${OE_TARGET}/${OE_TYPE}/OENames_2.1.6/lib)
		set(ORB_INCLUDE_DIR ${ORB_ROOT}/target/ppc_8xx-vxworks_6.6/${OE_TARGET}/${OE_TYPE}/include)
		set(ORB_INCLUDE_DIR ${ORB_INCLUDE_DIR} ${ORB_ROOT}/target/ppc_8xx-vxworks_6.6/${OE_TARGET}/${OE_TYPE}/OENames_2.1.6/include)


		
		
		#include_directories(${ORB_ROOT}/target/ppc_8xx-vxworks_6.6/gcc_4.1.2_ExLcRi/config_rt_full/OENames_2.1.6/include)
		set(ORB_LIBRARIES OEnames OEtcp OEmirror OEbridge OEudp OEipmc OEshrmem OEorb)
		#set(ORB_LIBRARIES OEtcp OEmirror OEbridge OEudp OEipmc OEshrmem OEudp OEorb OEnames)
		
	endif()
	SET (ORB_C_FLAGS_LIST
		 -DCORBA_ANY_SHARED_PTR_OPERATORS -DINCLUDE_CORBA_CORBALOC
#		-Wpointer-arith -Wwrite-strings -Waggregate-return -Wredundant-decls -Wno-unused -Wshadow -Wundef -Wold-style-cast -fno-implement-inlines -fvolatile  -ansi -msoft-float
	)
	if("${VX_CPU_FAMILY}" STREQUAL "ppc")
		set(ORB_C_FLAGS_LIST ${ORB_C_FLAGS_LIST} -D__powerpc__)
	else()
		set(ORB_C_FLAGS_LIST ${ORB_C_FLAGS_LIST} -D__x86__)
	endif()
	ADD_DEFINITIONS(${ORB_C_FLAGS_LIST})
elseif(CORBA STREQUAL "omniORB")
	set(CORBA_NAME "omniORB" CACHE STRING "CORBA name.")
	if(VXWORKS)
		if("${VX_CPU_FAMILY}" STREQUAL "ppc")
			SET (ORB_C_FLAGS_LIST
				-D__vxWorks__
				-D__powerpc__
			)
		else()
			SET (ORB_C_FLAGS_LIST
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
		if("${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "ARM")
			SET (ORB_C_FLAGS_LIST
				-D__arm__
				-D__linux__
			)
		else()
			SET (ORB_C_FLAGS_LIST
				-D__x86__
				-D__linux__
			)
		endif()
		if(${CMAKE_SYSTEM_NAME}test MATCHES QNXtest)
			SET (ORB_C_FLAGS_LIST
				${ORB_C_FLAGS_LIST}
				-D__QNX__
			)
		else()
			SET (ORB_C_FLAGS_LIST
				${ORB_C_FLAGS_LIST}
				-D__linux__
			)
		endif()
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
		endif(SSL_ENABLE)
		

	elseif(MSVC)
		if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "ARM")
			SET(ARCH_NAME ARM_win32)
			SET (ORB_C_FLAGS_LIST
				-D__WIN32__
				-D__arm__
			)
			set(ORB_LINK_DIR ${ORB_ROOT}/lib/${ARCH_NAME})
			set(OMNIORB_BINARY_DIR ${ORB_ROOT}/bin/${ARCH_NAME})
		else()
			SET(ARCH_NAME x86_win32)
			SET (ORB_C_FLAGS_LIST
				-D__WIN32__
				-D__x86__
			)
			set(ORB_LINK_DIR ${ORB_ROOT}/lib/${ARCH_NAME})
			set(OMNIORB_BINARY_DIR ${ORB_ROOT}/bin/${ARCH_NAME})
		endif()
		set(ORB_LIBRARIES optimized;omniDynamic${OMNI_VERSION}${OMNI_MINOR}_rt;optimized;omniORB${OMNI_VERSION}${OMNI_MINOR}_rt;optimized;omnithread${OMNITHREAD_VERSION}_rt;debug;omniDynamic${OMNI_VERSION}${OMNI_MINOR}_rtd;debug;omniORB${OMNI_VERSION}${OMNI_MINOR}_rtd;debug;omnithread${OMNITHREAD_VERSION}_rtd)
		if(SSL_ENABLE)
			set(ORBSSL_LIBRARIES optimized;omnisslTP${OMNI_VERSION}${OMNI_MINOR}_rt;debug;omnisslTP${OMNI_VERSION}${OMNI_MINOR}_rtd)
		endif(SSL_ENABLE)



		#message(STATUS ${OMNI_VERSION1})
		#message(STATUS ${OMNI_VERSION2})

		set(ORB_INSTALL_DIR ${OPENRTM_VERSION}/omniORB/${OMNI_VERSION1}.${OMNI_VERSION2}.${OMNI_MINOR}_${RTM_VC_VER}/)
		foreach(library ${ORB_LIBRARIES};${ORBSSL_LIBRARIES})
			if((NOT (${library} STREQUAL "optimized")) AND (NOT (${library} STREQUAL "debug")))
				string(REPLACE "_rt" "_${RTM_VC_VER}_rt" dll "${library}")
				install(PROGRAMS "${OMNIORB_BINARY_DIR}/${dll}.dll" DESTINATION ${ORB_INSTALL_DIR}/bin/${ARCH_NAME} COMPONENT corbalib)
				install(PROGRAMS "${OMNIORB_BINARY_DIR}/${dll}.pdb" DESTINATION ${ORB_INSTALL_DIR}/bin/${ARCH_NAME} COMPONENT corbalib)
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
			#message(STATUS ${dir_path})
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
		

		install(PROGRAMS ${ORB_ROOT}/bin/${ARCH_NAME}/omniidl.exe DESTINATION ${ORB_INSTALL_DIR}/bin/${ARCH_NAME} COMPONENT corbalib)
		install(PROGRAMS ${ORB_ROOT}/bin/${ARCH_NAME}/omniNames.exe DESTINATION ${ORB_INSTALL_DIR}/bin/${ARCH_NAME} COMPONENT corbalib)


		#install(FILES  ${OMNIORB_HEADERFILES} DESTINATION ${ORB_INSTALL_DIR}/include)

		set(ORB_INCLUDE_DIR ${ORB_ROOT}/include)
	endif(VXWORKS)


	

	
	ADD_DEFINITIONS(${ORB_C_FLAGS_LIST})
elseif(CORBA STREQUAL "TAO")
	set(CORBA_NAME "TAO" CACHE STRING "CORBA name.")
	
	if(VXWORKS)
		if("${VX_CPU_FAMILY}" STREQUAL "ppc")
			SET(ORB_C_FLAGS_LIST -D__powerpc__)
		else()
			SET(ORB_C_FLAGS_LIST -D__x86__)
		endif()
		SET(ORB_C_FLAGS_LIST ${ORB_C_FLAGS_LIST} -DACE_VXWORKS=0x690 -DACE_HAS_PTHREADS -DACE_HAS_AIO_CALLS -DACE_LACKS_ISBLANK)

		set(ORB_LINK_DIR ${ORB_ROOT}/lib)
		set(ORB_LIBRARIES TAO_AnyTypeCode TAO_PortableServer TAO_Svc_Utils TAO_CosNaming TAO_DynamicInterface TAO_PI TAO_Utils TAO_PortableServer TAO_DiffServPolicy TAO_DynamicInterface TAO_Strategies TAO ACE)
		set(ORB_INCLUDE_DIR ${ORB_ROOT} ${ORB_ROOT}/TAO ${ORB_ROOT}/TAO/orbsvcs)
	elseif(UNIX)
		set(ORB_LINK_DIR ${ORB_ROOT}/lib)
		set(ORB_LIBRARIES ACE TAO TAO_AnyTypeCode TAO_PortableServer TAO_Svc_Utils TAO_CosNaming TAO_IORTable TAO_CodecFactory TAO_PI TAO_Utils TAO_Strategies)
		set(ORB_INCLUDE_DIR ${ORB_ROOT}/include)
	elseif(MSVC)
		set(ORB_LINK_DIR ${ORB_ROOT}/lib)
		set(ORB_LIBRARIES1 optimized;ACE;optimized;TAO;optimized;TAO_AnyTypeCode;optimized;TAO_PortableServer;optimized;TAO_Svc_Utils;optimized;TAO_CosNaming;optimized;TAO_IORTable;optimized;TAO_CodecFactory;optimized;TAO_PI;optimized;TAO_Utils;optimized;TAO_Strategies;debug;ACEd;debug;TAOd;debug;TAO_AnyTypeCoded;debug;TAO_PortableServerd;debug;TAO_Svc_Utilsd;debug;TAO_CosNamingd;debug;TAO_IORTabled;debug;TAO_CodecFactoryd;debug;TAO_PId;debug;TAO_Utilsd;debug;TAO_Strategiesd)
		set(ORB_LIBRARIES2 iphlpapi)
		set(ORB_LIBRARIES ${ORB_LIBRARIES1};${ORB_LIBRARIES2})

		set(ORB_INCLUDE_DIR ${ORB_ROOT} ${ORB_ROOT}/TAO ${ORB_ROOT}/TAO/orbsvcs)
		SET(ORB_C_FLAGS_LIST ${ORB_C_FLAGS_LIST} -D_WINSOCKAPI_ -DWITH_ACE)


		set(ORB_INSTALL_DIR ${OPENRTM_VERSION}/ACE/${RTM_VC_VER}/)
		foreach(library ${ORB_LIBRARIES1})
			if((NOT (${library} STREQUAL "optimized")) AND (NOT (${library} STREQUAL "debug")))
				install(PROGRAMS "${ORB_LINK_DIR}/${library}.dll" DESTINATION ${ORB_INSTALL_DIR}/bin/ COMPONENT corbalib)
				#install(PROGRAMS "${ORB_LINK_DIR}/${library}.pdb" DESTINATION ${ORB_INSTALL_DIR}/bin/ COMPONENT corbalib)
				install(PROGRAMS "${ORB_LINK_DIR}/${library}.lib" DESTINATION ${ORB_INSTALL_DIR}/lib/ COMPONENT corbalib)
			endif()
		endforeach()


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
		#file(GLOB ACE_HEADERFILES12 "${ORB_ROOT}/ace/*/*.cpp")
		#file(GLOB ACE_HEADERFILES13 "${ORB_ROOT}/ace/*/*/*.cpp")
		#file(GLOB ACE_HEADERFILES14 "${ORB_ROOT}/ace/*/*/*/*.cpp")
		#file(GLOB ACE_HEADERFILES15 "${ORB_ROOT}/ace/*/*/*/*/*.cpp")


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
		#file(GLOB ACE_HEADERFILES28 "${ORB_ROOT}/TAO/tao/*/*/*.cpp")
		#file(GLOB ACE_HEADERFILES29 "${ORB_ROOT}/TAO/tao/*/*/*/*.cpp")
		#file(GLOB ACE_HEADERFILES30 "${ORB_ROOT}/TAO/tao/*/*/*/*/*.cpp")


		file(GLOB ACE_HEADERFILES31 "${ORB_ROOT}/TAO/orbsvcs/*.h")
		file(GLOB ACE_HEADERFILES32 "${ORB_ROOT}/TAO/orbsvcs/*/*.h")
		file(GLOB ACE_HEADERFILES33 "${ORB_ROOT}/TAO/orbsvcs/*/*/*.h")
		#file(GLOB ACE_HEADERFILES34 "${ORB_ROOT}/TAO/orbsvcs/*/*/*/*.h")
		#file(GLOB ACE_HEADERFILES35 "${ORB_ROOT}/TAO/orbsvcs/*/*/*/*/*.h")

		file(GLOB ACE_HEADERFILES36 "${ORB_ROOT}/TAO/orbsvcs/*.inl")
		file(GLOB ACE_HEADERFILES37 "${ORB_ROOT}/TAO/orbsvcs/*/*.inl")
		#file(GLOB ACE_HEADERFILES38 "${ORB_ROOT}/TAO/orbsvcs/*/*/*.inl")
		#file(GLOB ACE_HEADERFILES39 "${ORB_ROOT}/TAO/orbsvcs/*/*/*/*.inl")
		#file(GLOB ACE_HEADERFILES40 "${ORB_ROOT}/TAO/orbsvcs/*/*/*/*/*.inl")

		#file(GLOB ACE_HEADERFILES41 "${ORB_ROOT}/TAO/orbsvcs/*.cpp")
		#file(GLOB ACE_HEADERFILES42 "${ORB_ROOT}/TAO/orbsvcs/*/*.cpp")
		#file(GLOB ACE_HEADERFILES43 "${ORB_ROOT}/TAO/orbsvcs/*/*/*.cpp")
		#file(GLOB ACE_HEADERFILES44 "${ORB_ROOT}/TAO/orbsvcs/*/*/*/*.cpp")
		#file(GLOB ACE_HEADERFILES45 "${ORB_ROOT}/TAO/orbsvcs/*/*/*/*/*.cpp")


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
					${ACE_HEADERFILES43} ${ACE_HEADERFILES44} ${ACE_HEADERFILES45})


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


	endif()
	
	ADD_DEFINITIONS(${ORB_C_FLAGS_LIST})
endif()


if(NO_LOGGING)
	SET (ORB_C_FLAGS_LIST
		${ORB_C_FLAGS_LIST}
		-DNO_LOGGING
	)
	ADD_DEFINITIONS(-DNO_LOGGING)
endif(NO_LOGGING)


if(VXWORKS)

	set(ORB_LIBRARIES_PATH)
	FOREACH(NAME ${ORB_LIBRARIES})
		SET(ORB_LIB ORB_${NAME}_LIBRARY)
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


set(ORB_INCLUDE_DIR ${ORB_INCLUDE_DIR})
set(ORB_C_FLAGS_LIST ${ORB_C_FLAGS_LIST})












if(VXWORKS)
	set(COIL_OS_DIR "vxworks")
	set(RTM_OS_VXWORKS ON)
	ADD_DEFINITIONS(-O2)
elseif(UNIX)
	set(COIL_OS_DIR "posix")
	set(CMAKE_POSITION_INDEPENDENT_CODE ON)
	if(${CMAKE_SYSTEM_NAME}test MATCHES QNXtest)
		set (RTM_OS_QNX ON)
	else()
		set(RTM_OS_LINUX ON)
		set(RTM_LINKER_OPTION dl rt)
	endif()
elseif(WIN32)
	set(COIL_OS_DIR "win32")
	set(RTM_LINKER_OPTION advapi32.lib;ws2_32.lib;mswsock.lib;rpcrt4.lib;optimized;msvcrt.lib;optimized;msvcmrt.lib;debug;msvcrtd.lib)
	ADD_DEFINITIONS(-DLIBRARY_EXPORTS)
endif()







if(WIN32)
	set(MAKE_DEF_COMMAND ${CMAKE_SOURCE_DIR}/build/makedeffile.py)
endif(WIN32)

add_subdirectory(src)
add_subdirectory(examples)
add_subdirectory(utils)
add_subdirectory(etc)

set(DOCUMENTS_ENABLE ${DOCUMENTS_ENABLE} CACHE BOOL "set DOCUMENTS_ENABLE")

if(DOCUMENTS_ENABLE)
	add_subdirectory(docs)
endif(DOCUMENTS_ENABLE)

if(WIN32)
	install(FILES ${CMAKE_SOURCE_DIR}/win32/OpenRTM-aist/bin/rtm-naming.bat DESTINATION ${INSTALL_RTM_SCRIPT_DIR} COMPONENT utils)
endif(WIN32)


CONFIGURE_FILE ("${PROJECT_SOURCE_DIR}/uninstall_target.cmake.in"
    "${PROJECT_BINARY_DIR}/uninstall_target.cmake" IMMEDIATE @ONLY)
ADD_CUSTOM_TARGET (uninstall "${CMAKE_COMMAND}" -P
    "${PROJECT_BINARY_DIR}/uninstall_target.cmake")