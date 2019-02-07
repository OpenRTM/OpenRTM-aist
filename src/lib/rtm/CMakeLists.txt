cmake_minimum_required (VERSION 2.6)

project (${RTM_PROJECT_NAME})







option (ARTLINUX "ARTLinux is avaiable" OFF)
option (HAVE_DLFCN_H "Define to 1 if you have the <dlfcn.h> header file." ON)
option (HAVE_INTTYPES_H "Define to 1 if you have the <inttypes.h> header file." ON)
option (HAVE_MEMORY_H "Define to 1 if you have the <memory.h> header file." ON)
option (HAVE_NLIST_H "Define to 1 if you have the <nlist.h> header file." ON)
option (HAVE_STDINT_H "Define to 1 if you have the <stdlib.h> header file." ON)
option (HAVE_STRINGS_H "Define to 1 if you have the <strings.h> header file." ON)
option (HAVE_STRING_H "Define to 1 if you have the <string.h> header file." ON)
option (HAVE_SYS_STAT_H "Define to 1 if you have the <sys/stat.h> header file." ON)
option (HAVE_SYS_TYPES_H "Define to 1 if you have the <sys/types.h> header file." ON)
option (HAVE_UNAME "Define to 1 if you have the `uname' function." ON)
option (HAVE_UNISTD_H "Define to 1 if you have the <unistd.h> header file." ON)
option (ORB_IS_MICO "ORB is MICO" OFF)
option (ORB_IS_OMNIORB "ORB is omniORB" OFF)
option (ORB_IS_ORBEXPRESS "ORB is ORBexpresds" OFF)
option (ORB_IS_ORBACUS "ORB is ORBacus" OFF)
option (ORB_IS_ORBIT2 "ORB is ORBit2" OFF)
option (ORB_IS_ORBIX "ORB is Orbix" OFF)
option (ORB_IS_RTORB "ORB is RtORB" OFF)
option (ORB_IS_TAO "ORB is TAO" OFF)
option (RTM_PACKAGE "Name of package" OFF)
option (RTM_PACKAGE_BUGREPORT "Define to the address where bug reports for this package should be sent." OFF)
option (RTM_PACKAGE_NAME "Define to the full name of this package." OFF)
option (RTM_PACKAGE_STRING "Define to the full name and version of this package." OFF)
option (RTM_PACKAGE_TARNAME "Define to the one symbol short name of this package." OFF)
option (RTM_PACKAGE_URL "Define to the home page for this package." OFF)
option (RTM_PACKAGE_VERSION "Define to the version of this package." OFF)
option (PYTHON_WRAPPER " Python wrapper is enable" OFF)
option (RTC_CORBA_CXXMAPPING11 "CORBA C++ mapping is ver1.1" OFF)
option (RTC_SOCK_DATAPORT "Socket DataPort is enable" OFF)
option (RTM_GCC2 "RTM is compiled with gcc2" OFF)
option (RTM_OMNIORB_40 "omniORB version is 4.0" OFF)
option (RTM_OMNIORB_41 "omniORB version is 4.1" OFF)
option (RTM_OMNIORB_42 "omniORB version is 4.2" OFF)
option (RTM_OS_CYGWIN "OS is Cygwin" OFF)
option (RTM_OS_DARWIN "OS is Max OS X" OFF)
option (RTM_OS_FREEBSD "OS is FreeBSD" OFF)
option (RTM_OS_HPUX "OS is HP-UX" OFF)
option (RTM_OS_IRIX "OS is IRIX" OFF)
option (RTM_OS_LINUX "OS is Linux" OFF)
option (RTM_OS_VXWORKS "OS is VxWorks" OFF)
option (RTM_OS_OSF1 "OS is OSF1" OFF)
option (RTM_OS_QNX "OS is QNX" OFF)
option (RTM_OS_SUNOS "OS is SunOS" OFF)
option (RTM_OS_WIN32 "OS is Windows" OFF)
option (RTM_RTDSC "RDTSC is enable" OFF)
option (WITH_DMALLOC "Define if using the dmalloc debugging malloc package" OFF)
option (_REENTRANT "Multi Thread Support" ON)


set(LT_OBJDIR .libs/ CACHE STRING "Define to the sub-directory in which libtool stores uninstalled libraries.")

if(VXWORKS)
	set (OSVERSION ON)
	set(OSVERSION ${VXWORKS_MAJOR}${VXWORKS_MINOR} CACHE STRING "OS version.")
	set (RTM_OS_VXWORKS ON)
	set (STDC_HEADERS ON)
#	set (STDC_HEADERS_VALUE 1 CACHE STRING "Define to 1 if you have the ANSI C header files.")
elseif(UNIX)
	set (STDC_HEADERS OFF)
	set (OSVERSION OFF)
	if(${CMAKE_SYSTEM_NAME}test MATCHES QNXtest)
		set (RTM_OS_QNX ON)
	else()
		set (RTM_OS_LINUX ON)
	endif()
elseif(WIN32)
	set (STDC_HEADERS OFF)
	set (OSVERSION OFF)
	set (RTM_OS_WIN32 ON)
endif()



if(CORBA MATCHES "ORBexpress")
	set (ORB_IS_ORBEXPRESS ON)
elseif(CORBA MATCHES "omniORB")
	if(OMNI_VERSION MATCHES "40")
		set (RTM_OMNIORB_40 ON)
	elseif(OMNI_VERSION MATCHES "41")
		set (RTM_OMNIORB_41 ON)
	else()
		set (RTM_OMNIORB_42 ON)
	endif()
	set (ORB_IS_OMNIORB ON)
elseif(CORBA MATCHES "TAO")
	set (ORB_IS_TAO ON)
endif()







set (RTC_CORBA_CXXMAPPING11 ON)




#ADD_DEFINITIONS(-DRTC_CORBA_CXXMAPPING11)






#ADD_DEFINITIONS(-DCORBA_ANY_SHARED_PTR_OPERATORS -DINCLUDE_CORBA_CORBALOC)












configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config_rtc_cmake.h.in ${PROJECT_BINARY_DIR}/config_rtc.h)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/version_cmake.h.in ${PROJECT_BINARY_DIR}/version.h)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config_rtc_cmake.h.in ${CMAKE_CURRENT_SOURCE_DIR}/config_rtc.h)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/version_cmake.h.in ${CMAKE_CURRENT_SOURCE_DIR}/version.h)




if(VXWORKS)
else(VXWORKS)
	link_directories(${ORB_LINK_DIR})
endif(VXWORKS)

include_directories(${ORB_INCLUDE_DIR})


set(RTM_INCLUDE_DIR ${CMAKE_SOURCE_DIR}
		${CMAKE_SOURCE_DIR}/src/lib/coil/common
		${PROJECT_BINARY_DIR}/idl
		${CMAKE_SOURCE_DIR}/src/lib/coil/${COIL_OS_DIR}
		${PROJECT_BINARY_DIR}/..
		${CMAKE_CURRENT_SOURCE_DIR}/..
		)







include_directories(
		${RTM_INCLUDE_DIR}
		)


if(${CMAKE_SYSTEM_NAME}test MATCHES QNXtest)
	include_directories(/usr/local/include)
	link_directories(/usr/qnx650/target/qnx6/x86/lib/gcc/4.4.2/ /usr/qnx650/target/qnx6/x86/lib/ /usr/local/lib)
endif()



set(RTM_INCLUDE_DIR ${RTM_INCLUDE_DIR}  PARENT_SCOPE)



add_subdirectory(idl)

add_subdirectory(ext)



set(rtm_headers
	ConfigAdmin.h
	ModuleManager.h
	CorbaNaming.h
	NVUtil.h
	ManagerActionListener.h
	LocalServiceAdmin.h
	Factory.h
	PublisherPeriodic.h
	ECFactory.h
	OutPortCorbaCdrConsumer.h
	PeriodicExecutionContext.h
	InPortPushConnector.h
	CORBA_IORUtil.h
	ManagerServant.h
	OpenHRPExecutionContext.h
	RTObjectStateMachine.h
	InPortProvider.h
	PortConnectListener.h
	DefaultPeriodicTask.h
	NamingManager.h
	OutPortCorbaCdrProvider.h
	InPortDirectProvider.h
	PortBase.h
	OutPortProvider.h
	ExecutionContextProfile.h
	PublisherFlush.h
	OutPortPushConnector.h
	CorbaPort.h
	SdoConfiguration.h
	RTObject.h
	DataFlowComponentBase.h
	ManagerConfig.h
	SystemLogger.h
	ExecutionContextWorker.h
	ExecutionContextBase.h
	ExtTrigExecutionContext.h
	InPortBase.h
	SdoOrganization.h
	PortAdmin.h
	InPortPullConnector.h
	ConfigurationListener.h
	OutPortPullConnector.h
	LogstreamFile.h
	RTCUtil.h
	CdrRingBuffer.h
	InPortCorbaCdrProvider.h
	ConnectorListener.h
	PeriodicECSharedComposite.h
	PublisherNew.h
	ComponentActionListener.h
	InPortDirectConsumer.h
	OutPortBase.h
	Manager.h
	InPortCorbaCdrConsumer.h
	FactoryInit.h
	SdoServiceAdmin.h
	InPortConnector.h
	OutPortConnector.h
	NumberingPolicy.h
	BufferBase.h
	BufferStatus.h
	CORBA_SeqUtil.h
	CdrBufferBase.h
	ConnectorBase.h
	CorbaConsumer.h
	DataInPort.h
	DataOutPort.h
	DataPortStatus.h
	DefaultConfiguration.h
	InPort.h
	InPortConsumer.h
	ListenerHolder.h
	LocalServiceBase.h
	ObjectManager.h
	OutPort.h
	OutPortConsumer.h
	PeriodicTaskFactory.h
	PortCallback.h
	PublisherBase.h
	RTC.h
	RingBuffer.h
	SdoServiceConsumerBase.h
	SdoServiceProviderBase.h
	StateMachine.h
	Typename.h
	FsmActionListener.h
	EventPort.h
	Macho.h
	StaticFSM.h
	InPortSHMConsumer.h
	InPortSHMProvider.h
	OutPortSHMConsumer.h
	OutPortSHMProvider.h
	SharedMemoryPort.h
	Timestamp.h
	SimulatorExecutionContext.h
	DirectInPortBase.h
	DirectOutPortBase.h
	DirectPortBase.h
	NumberingPolicyBase.h
	NamingServiceNumberingPolicy.h
	NodeNumberingPolicy.h
	OutPortDirectConsumer.h
	OutPortDirectProvider.h
	CORBA_RTCUtil.h
	OutPortDSProvider.h
	OutPortDSConsumer.h
	InPortDSProvider.h
	InPortDSConsumer.h
	MultilayerCompositeEC.h
	EventBase.h
	${PROJECT_BINARY_DIR}/config_rtc.h
	${PROJECT_BINARY_DIR}/version.h
)


set(rtm_srcs
	ConfigAdmin.cpp
	ModuleManager.cpp
	CorbaNaming.cpp
	NVUtil.cpp
	ManagerActionListener.cpp
	LocalServiceAdmin.cpp
	Factory.cpp
	PublisherPeriodic.cpp
	ECFactory.cpp
	OutPortCorbaCdrConsumer.cpp
	PeriodicExecutionContext.cpp
	InPortPushConnector.cpp
	CORBA_IORUtil.cpp
	ManagerServant.cpp
	OpenHRPExecutionContext.cpp
	RTObjectStateMachine.cpp
	InPortProvider.cpp
	PortConnectListener.cpp
	DefaultPeriodicTask.cpp
	NamingManager.cpp
	OutPortCorbaCdrProvider.cpp
	InPortDirectProvider.cpp
	PortBase.cpp
	OutPortProvider.cpp
	ExecutionContextProfile.cpp
	PublisherFlush.cpp
	OutPortPushConnector.cpp
	CorbaPort.cpp
	SdoConfiguration.cpp
	RTObject.cpp
	DataFlowComponentBase.cpp
	ManagerConfig.cpp
	SystemLogger.cpp
	ExecutionContextWorker.cpp
	ExecutionContextBase.cpp
	ExtTrigExecutionContext.cpp
	InPortBase.cpp
	SdoOrganization.cpp
	PortAdmin.cpp
	InPortPullConnector.cpp
	ConfigurationListener.cpp
	OutPortPullConnector.cpp
	LogstreamFile.cpp
	RTCUtil.cpp
	CdrRingBuffer.cpp
	InPortCorbaCdrProvider.cpp
	ConnectorListener.cpp
	PeriodicECSharedComposite.cpp
	PublisherNew.cpp
	ComponentActionListener.cpp
	InPortDirectConsumer.cpp
	OutPortBase.cpp
	Manager.cpp
	InPortCorbaCdrConsumer.cpp
	FactoryInit.cpp
	SdoServiceAdmin.cpp
	InPortConnector.cpp
	OutPortConnector.cpp
	NumberingPolicy.cpp
	FsmActionListener.cpp
	Macho.cpp
	InPortSHMConsumer.cpp
	InPortSHMProvider.cpp
	OutPortSHMConsumer.cpp
	OutPortSHMProvider.cpp
	SharedMemoryPort.cpp
	SimulatorExecutionContext.cpp
	NumberingPolicyBase.cpp
	NamingServiceNumberingPolicy.cpp
	NodeNumberingPolicy.cpp
	OutPortDirectConsumer.cpp
	OutPortDirectProvider.cpp
	CORBA_RTCUtil.cpp
	OutPortDSProvider.cpp
	OutPortDSConsumer.cpp
	InPortDSProvider.cpp
	InPortDSConsumer.cpp
	MultilayerCompositeEC.cpp
	${rtm_headers}
)

if(CORBA MATCHES "TAO")
	set(rtm_headers ${rtm_headers}
		InPortCorbaCdrUDPConsumer.h
		InPortCorbaCdrUDPProvider.h
	 )
	set(rtm_srcs ${rtm_srcs} 
		InPortCorbaCdrUDPConsumer.cpp
		InPortCorbaCdrUDPProvider.cpp
	 )
endif()

if(VXWORKS)
	set(rtm_headers ${rtm_headers} VxWorksRTExecutionContext.h)
	set(rtm_srcs ${rtm_srcs} VxWorksRTExecutionContext.cpp)
	if(RTP)
	else(RTP)
		set(rtm_headers ${rtm_headers} VxWorksInterruptExecutionContext.h)
		set(rtm_srcs ${rtm_srcs} VxWorksInterruptExecutionContext.cpp)
	endif(RTP)
endif(VXWORKS)


if(OBSERVER_ENABLE)
	add_definitions(-DENABLE_OBSERVER)
	include_directories(${CMAKE_SOURCE_DIR}/src)
	#set(rtm_headers ${rtm_headers} ${CMAKE_SOURCE_DIR}/src/ext/sdo/observer/ComponentObserverConsumer.h)
	set(rtm_srcs ${rtm_srcs} ${CMAKE_SOURCE_DIR}/src/ext/sdo/observer/ComponentObserverConsumer.cpp	)
	install(FILES ${CMAKE_SOURCE_DIR}/src/ext/sdo/observer/ComponentObserverConsumer.h DESTINATION ${INSTALL_RTM_EXT_DIR}/sdo/observer COMPONENT headers)
endif(OBSERVER_ENABLE)






#message(STATUS ${ALL_IDL_SRCS})

#message(STATUS ${ORB_LIBRARIES})

#add_executable(rtm rtcd.cpp TestRTC.cpp ${rtm_srcs} ${ALL_IDL_SRCS})
#add_library(rtm STATIC ${rtm_srcs} ${ALL_IDL_SRCS} )





if(WIN32)
	#FOREACH(filename ${SKEL_IDL_SRCS})
	#	set(skel_files ${skel_files} ${SKEL_IDL_DIR}/${filename})
	#ENDFOREACH()

	set(DEF_FILE_NAME  ${RTM_PROJECT_NAME}.def)
	#ADD_DEFINITIONS(-DLIBRARY_EXPORTS)
	#add_library(${RTM_PROJECT_NAME}_static STATIC ${rtm_srcs} ${ALL_IDL_SRCS_RTM})

	set(rtm_srcs ${rtm_srcs} ${SKEL_IDL_SRCS})
	set_source_files_properties(${SKEL_IDL_SRCS} PROPERTIES GENERATED 1)
	add_library(${RTM_PROJECT_NAME}_static STATIC ${rtm_srcs} )
	
	add_dependencies(${RTM_PROJECT_NAME}_static ALL_IDL_TGT)

	#add_dependencies(${RTM_PROJECT_NAME}_static ${RTCSKEL_PROJECT_NAME})
	get_property(rtm_static_location TARGET ${RTM_PROJECT_NAME}_static PROPERTY LOCATION)


	install(TARGETS ${RTM_PROJECT_NAME}_static LIBRARY DESTINATION ${INSTALL_RTM_LIB_DIR}
					ARCHIVE DESTINATION ${INSTALL_RTM_LIB_DIR}
					RUNTIME DESTINATION ${INSTALL_RTM_LIB_DIR}
					COMPONENT openrtm)

	#message(STATUS ${rtm_static_location})
	add_custom_command(OUTPUT ${DEF_FILE_NAME}
		COMMAND python ${MAKE_DEF_COMMAND} ${rtm_static_location} ${RTM_PROJECT_NAME} ${RTM_SHORT_VERSION} ${DEF_FILE_NAME}
		
		WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
	)
	add_custom_target(${RTM_PROJECT_NAME}_TGT DEPENDS ${DEF_FILE_NAME})
	add_dependencies(${RTM_PROJECT_NAME}_TGT ${RTM_PROJECT_NAME}_static)
	set(rtm_srcs ${rtm_srcs} ${CMAKE_CURRENT_BINARY_DIR}/${DEF_FILE_NAME})

	#get_property(rtcskel_location TARGET ${RTCSKEL_PROJECT_NAME} PROPERTY LOCATION)
	#add_custom_command(OUTPUT ${RTCSKEL_PROJECT_NAME}.def
	#	COMMAND python ${MAKE_DEF_COMMAND} ${rtcskel_location} ${RTCSKEL_PROJECT_NAME} ${RTM_SHORT_VERSION} ${RTCSKEL_PROJECT_NAME}.def
	#	WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
	#)

#	add_custom_command(OUTPUT ${RTM_PROJECT_NAME}.def
#		COMMAND lib -out:"rtm_static.lib"  "$(IntDir)*.obj" "idl\\RTCSkel.dir\\Release\\*.obj"
#		COMMAND python ${MAKE_DEF_COMMAND} rtm_static.lib ${RTM_PROJECT_NAME} ${RTM_SHORT_VERSION} ${RTM_PROJECT_NAME}.def
#		PRE_LINK
#		WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
#		DEPENDS ${RTM_PROJECT_NAME}
#	)
#	set(rtm_srcs ${rtm_srcs} ${CMAKE_CURRENT_BINARY_DIR}/${RTM_PROJECT_NAME}.def)
endif(WIN32)


if(VXWORKS)
	add_library(${RTM_PROJECT_NAME} STATIC ${rtm_srcs})
	install(TARGETS ${RTM_PROJECT_NAME} LIBRARY DESTINATION ${INSTALL_RTM_LIB_DIR}
					ARCHIVE DESTINATION ${INSTALL_RTM_BIN_DIR}
					RUNTIME DESTINATION ${INSTALL_RTM_BIN_DIR}
					COMPONENT openrtm)

else(VXWORKS)
	
	add_library(${RTM_PROJECT_NAME} SHARED ${rtm_srcs})
	set_target_properties(${RTM_PROJECT_NAME} PROPERTIES DEBUG_POSTFIX d)


	



	if(UNIX)
		target_link_libraries(${RTM_PROJECT_NAME} ${ORB_LIBRARIES} ${COIL_PROJECT_NAME} ${RTCSKEL_PROJECT_NAME} ${RTM_LINKER_OPTION})
		install(TARGETS ${RTM_PROJECT_NAME} LIBRARY DESTINATION ${INSTALL_RTM_LIB_DIR}
						ARCHIVE DESTINATION ${INSTALL_RTM_LIB_DIR}
						RUNTIME DESTINATION ${INSTALL_RTM_BIN_DIR}
						COMPONENT openrtm)


		add_library(${RTM_PROJECT_NAME}-static STATIC ${rtm_srcs})
		set_target_properties(${RTM_PROJECT_NAME}-static PROPERTIES DEBUG_POSTFIX d)
		set_target_properties(${RTM_PROJECT_NAME}-static PROPERTIES OUTPUT_NAME ${RTM_PROJECT_NAME} CLEAN_DIRECT_OUTPUT 1)
		install(TARGETS ${RTM_PROJECT_NAME}-static LIBRARY DESTINATION ${INSTALL_RTM_LIB_DIR}
						ARCHIVE DESTINATION ${INSTALL_RTM_LIB_DIR}
						RUNTIME DESTINATION ${INSTALL_RTM_BIN_DIR}
						COMPONENT openrtm)
	elseif(WIN32)
		target_link_libraries(${RTM_PROJECT_NAME} ${ORB_LIBRARIES} ${COIL_PROJECT_NAME} ${RTM_LINKER_OPTION})
		add_dependencies(${RTM_PROJECT_NAME} ALL_IDL_TGT)
		set_source_files_properties(${DEF_FILE_NAME} PROPERTIES GENERATED 1)
		add_dependencies(${RTM_PROJECT_NAME} ${RTM_PROJECT_NAME}_TGT)
		add_dependencies(${RTM_PROJECT_NAME} ${RTM_PROJECT_NAME}_static)
		install(TARGETS ${RTM_PROJECT_NAME} LIBRARY DESTINATION ${INSTALL_RTM_LIB_DIR}
						ARCHIVE DESTINATION ${INSTALL_RTM_LIB_DIR}
						RUNTIME DESTINATION ${INSTALL_RTM_LIB_DIR}
						COMPONENT openrtm)

	endif()
endif(VXWORKS)

install(FILES  ${rtm_headers} DESTINATION ${INSTALL_RTM_INCLUDE_DIR}/rtm COMPONENT headers)



add_dependencies(${RTM_PROJECT_NAME} ${COIL_PROJECT_NAME})
#add_dependencies(rtm ALL_IDL_TGT)




add_dependencies(${RTM_PROJECT_NAME} ${RTCSKEL_PROJECT_NAME})





#target_link_libraries(rtm ${ORB_LIBRARIES} coil)



