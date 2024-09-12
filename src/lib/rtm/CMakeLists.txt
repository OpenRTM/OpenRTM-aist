cmake_minimum_required (VERSION 3.5.1)

project (${RTM_PROJECT_NAME}
	VERSION ${RTM_VERSION}
	LANGUAGES CXX)


if(WIN32)
	set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()

#==============================
# Configuration Options
#==============================
# CORBA options
set(ORB_IS_MICO OFF)
set(ORB_IS_OMNIORB OFF)
set(ORB_IS_ORBEXPRESS OFF)
set(ORB_IS_ORBACUS OFF)
set(ORB_IS_ORBIT2 OFF)
set(ORB_IS_ORBIX OFF)
set(ORB_IS_RTORB OFF)
set(ORB_IS_TAO OFF)
set(RTC_CORBA_CXXMAPPING11 ON)
set(RTM_OMNIORB_40 OFF)
set(RTM_OMNIORB_41 OFF)
set(RTM_OMNIORB_42 OFF)
set(RTM_OMNIORB_43 OFF)
# RTM_PACKAGE options
set(RTM_PACKAGE OFF)
set(RTM_PACKAGE_BUGREPORT OFF)
set(RTM_PACKAGE_NAME OFF)
set(RTM_PACKAGE_STRING OFF)
set(RTM_PACKAGE_TARNAME OFF)
set(RTM_PACKAGE_URL OFF)
set(RTM_PACKAGE_VERSION OFF)
# OS options
set(RTM_OS_DARWIN OFF)
set(RTM_OS_FREEBSD OFF)
set(RTM_OS_LINUX OFF)
set(RTM_OS_VXWORKS OFF)
set(RTM_OS_QNX OFF)
set(RTM_OS_WIN32 OFF)

#==============================
# OS type selection
#==============================
if (UNIX)
	if     (CMAKE_SYSTEM_NAME MATCHES "Darwin")
		set (RTM_OS_DARWIN ON)
	elseif (CMAKE_SYSTEM_NAME MATCHES "FreeBSD")
		set (RTM_OS_FREEBSD ON)
	elseif (CMAKE_SYSTEM_NAME MATCHES "Linux")
		set (RTM_OS_LINUX ON)
	elseif (CMAKE_SYSTEM_NAME MATCHES "QNX") 
		set (RTM_OS_QNX ON)
	else ()
		set (RTM_OS_LINUX ON)
	endif ()
elseif (WIN32)
	set (RTM_OS_WIN32 ON)
elseif (VXWORKS)
	set (RTM_OS_VXWORKS ON)
endif ()

#==============================
# CORBA type selection
#==============================
if(CORBA MATCHES "ORBexpress")
	set (ORB_IS_ORBEXPRESS ON)
elseif(CORBA MATCHES "omniORB")
	if(OMNI_VERSION MATCHES "40")
		set(RTM_OMNIORB_40 ON)
	elseif(OMNI_VERSION MATCHES "41")
		set(RTM_OMNIORB_41 ON)
	elseif(OMNI_VERSION MATCHES "42")
		set(RTM_OMNIORB_42 ON)
	else()
		set(RTM_OMNIORB_43 ON)
	endif()
	set (ORB_IS_OMNIORB ON)
elseif(CORBA MATCHES "TAO")
	set (ORB_IS_TAO ON)
elseif(CORBA MATCHES "RtORB")
	set (ORB_IS_RTORB ON)
endif()

#==============================
# Configuration options output
#==============================
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config_rtc_cmake.h.in ${PROJECT_BINARY_DIR}/config_rtc.h)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/version_cmake.h.in ${PROJECT_BINARY_DIR}/version.h)


#==============================
# Compile/link settings
#==============================
if(VXWORKS)
else(VXWORKS)
	link_directories(${ORB_LINK_DIR})
endif(VXWORKS)

add_definitions(${ORB_C_FLAGS_LIST})
add_definitions(${COIL_C_FLAGS_LIST})

if(${CMAKE_SYSTEM_NAME}test MATCHES QNXtest)
	include_directories(SYSTEM /usr/local/include)
	link_directories(/usr/qnx650/target/qnx6/x86/lib/gcc/4.4.2/ /usr/qnx650/target/qnx6/x86/lib/ /usr/local/lib)
endif()

#==============================
# Subdirectories settings
#==============================
add_subdirectory(idl)
add_subdirectory(ext)

#==============================
# Header list
#==============================
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
	LogstreamBase.h
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
	CORBA_CdrMemoryStream.h
	ByteData.h
	ByteDataStreamBase.h
	DataTypeUtil.h
	${PROJECT_BINARY_DIR}/config_rtc.h
	${PROJECT_BINARY_DIR}/version.h
)

#==============================
# Source list
#==============================
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
	SimulatorExecutionContext.cpp
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
	ByteData.cpp
	ByteDataStreamBase.cpp
	CORBA_CdrMemoryStream.cpp
	ConnectorBase.cpp
	LocalServiceBase.cpp
	${rtm_headers}
)

# TAO specific sources and headers
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

# RtORB　specific sources and headers
if(NOT CORBA MATCHES "RtORB")
	set(rtm_headers ${rtm_headers}
		SharedMemoryPort.h
		InPortSHMConsumer.h
		InPortSHMProvider.h
		OutPortSHMConsumer.h
		OutPortSHMProvider.h
	 )
	set(rtm_srcs ${rtm_srcs}
		SharedMemoryPort.cpp
		InPortSHMConsumer.cpp
		InPortSHMProvider.cpp
		OutPortSHMConsumer.cpp
		OutPortSHMProvider.cpp
	 )
endif()

# VxWorks specific sources/headers and settings
if(VXWORKS)
	set(rtm_headers ${rtm_headers} VxWorksRTExecutionContext.h)
	set(rtm_srcs ${rtm_srcs} VxWorksRTExecutionContext.cpp)
	if(RTP)
	else(RTP)
		set(rtm_headers ${rtm_headers} VxWorksInterruptExecutionContext.h)
		set(rtm_srcs ${rtm_srcs} VxWorksInterruptExecutionContext.cpp)
	endif(RTP)
endif(VXWORKS)

if(VXWORKS)
	add_library(${PROJECT_NAME} STATIC ${rtm_srcs})
	openrtm_common_set_compile_props(${PROJECT_NAME})
	openrtm_include_rtm(${PROJECT_NAME})
	if(OBSERVER_ENABLE)
		target_compile_definitions(${PROJECT_NAME} PRIVATE ENABLE_OBSERVER)
		target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/src)
		target_include_directories(${PROJECT_NAME} SYSTEM PRIVATE ${CMAKE_BINARY_DIR}/src/ext/sdo/observer)
		add_dependencies(${PROJECT_NAME} ComponentObserverConsumer_IDLTGT)
	endif(OBSERVER_ENABLE)
	install(TARGETS ${PROJECT_NAME} LIBRARY DESTINATION ${INSTALL_RTM_LIB_DIR}
					ARCHIVE DESTINATION ${INSTALL_RTM_BIN_DIR}
					RUNTIME DESTINATION ${INSTALL_RTM_BIN_DIR}
					COMPONENT openrtm)

else(VXWORKS)
	add_library(${PROJECT_NAME}_objlib OBJECT ${rtm_srcs})
	openrtm_common_set_compile_props(${PROJECT_NAME}_objlib)
	openrtm_include_rtm(${PROJECT_NAME}_objlib)
	target_compile_definitions(${PROJECT_NAME}_objlib PRIVATE LIBRARY_EXPORTS)
	if(OBSERVER_ENABLE)
		target_compile_definitions(${PROJECT_NAME}_objlib PRIVATE ENABLE_OBSERVER)
		target_include_directories(${PROJECT_NAME}_objlib PRIVATE ${CMAKE_SOURCE_DIR}/src)
		target_include_directories(${PROJECT_NAME}_objlib SYSTEM PRIVATE ${CMAKE_BINARY_DIR}/src/ext/sdo/observer)
		add_dependencies(${PROJECT_NAME}_objlib ComponentObserverConsumer_IDLTGT)
	endif(OBSERVER_ENABLE)

	if(WIN32)
		target_compile_definitions(${PROJECT_NAME}_objlib PRIVATE RTM_SKEL_IMPORT_SYMBOL)
		add_library(${PROJECT_NAME}-static STATIC
			$<TARGET_OBJECTS:${COIL_PROJECT_NAME}_objlib>
			$<TARGET_OBJECTS:${PROJECT_NAME}_objlib>)
		add_library(${PROJECT_NAME} SHARED
			$<TARGET_OBJECTS:${COIL_PROJECT_NAME}_objlib>
			$<TARGET_OBJECTS:${PROJECT_NAME}_objlib>)
	else()
		if(CORBA MATCHES "RtORB")
			add_library(${PROJECT_NAME}-static STATIC
				$<TARGET_OBJECTS:${PROJECT_NAME}_objlib>
				$<TARGET_OBJECTS:${COIL_PROJECT_NAME}_objlib>)
			add_library(${PROJECT_NAME} SHARED
				$<TARGET_OBJECTS:${PROJECT_NAME}_objlib>
				$<TARGET_OBJECTS:${COIL_PROJECT_NAME}_objlib>)
		else()
			add_library(${PROJECT_NAME}-static STATIC
				$<TARGET_OBJECTS:${PROJECT_NAME}_objlib>
				$<TARGET_OBJECTS:${COIL_PROJECT_NAME}_objlib>
				$<TARGET_OBJECTS:${RTCSKEL_PROJECT_NAME}_objlib>)
			add_library(${PROJECT_NAME} SHARED
				$<TARGET_OBJECTS:${PROJECT_NAME}_objlib>
				$<TARGET_OBJECTS:${COIL_PROJECT_NAME}_objlib>
				$<TARGET_OBJECTS:${RTCSKEL_PROJECT_NAME}_objlib>)
		endif()
	endif()

	openrtm_set_link_props_shared(${PROJECT_NAME})
	openrtm_set_postfix(${PROJECT_NAME})

	set(libs ${ORB_LIBRARIES} ${UUID_LIBRARIES} ${RTM_LINKER_OPTION})

	if(WIN32)
		set(libs ${libs} ${RTCSKEL_PROJECT_NAME})
	endif()

	if(OBSERVER_ENABLE)
		set(libs ${libs} ComponentObserverConsumer)
	endif()

	if(CORBA MATCHES "RtORB")
		set(libs ${libs} ${RTM_IDL_LIBS})
	endif()

	target_link_libraries(${PROJECT_NAME} PRIVATE ${libs})

	if(UNIX)
		install(TARGETS ${PROJECT_NAME} LIBRARY DESTINATION ${INSTALL_RTM_LIB_DIR}
						ARCHIVE DESTINATION ${INSTALL_RTM_LIB_DIR}
						RUNTIME DESTINATION ${INSTALL_RTM_BIN_DIR}
						COMPONENT openrtm)

		set_target_properties(${PROJECT_NAME} PROPERTIES VERSION ${RTM_VERSION} SOVERSION ${RTM_SO_VERSION})
		set_target_properties(${PROJECT_NAME}-static PROPERTIES OUTPUT_NAME ${PROJECT_NAME} CLEAN_DIRECT_OUTPUT 1)
		install(TARGETS ${PROJECT_NAME}-static LIBRARY DESTINATION ${INSTALL_RTM_LIB_DIR}
						ARCHIVE DESTINATION ${INSTALL_RTM_LIB_DIR}
						RUNTIME DESTINATION ${INSTALL_RTM_BIN_DIR}
						COMPONENT openrtm)

	elseif(WIN32)
		add_dependencies(${PROJECT_NAME} ${RTCSKEL_PROJECT_NAME}_IDLTGT)
		add_dependencies(${PROJECT_NAME} ${PROJECT_NAME}-static)
		install(TARGETS ${PROJECT_NAME} LIBRARY DESTINATION ${INSTALL_RTM_LIB_DIR}
						ARCHIVE DESTINATION ${INSTALL_RTM_LIB_DIR}
						RUNTIME DESTINATION ${INSTALL_RTM_LIB_DIR}
						COMPONENT openrtm)

		install(TARGETS ${PROJECT_NAME}-static LIBRARY DESTINATION ${INSTALL_RTM_LIB_DIR}
						ARCHIVE DESTINATION ${INSTALL_RTM_LIB_DIR}
						RUNTIME DESTINATION ${INSTALL_RTM_LIB_DIR}
						COMPONENT openrtm)
		set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS OFF)
	endif()

	if(WIN32)
		set(DATATYPE_FACTORIES ${RTCSKEL_PROJECT_NAME} PARENT_SCOPE)
	endif()

	if(CORBA MATCHES "RtORB")
		set(DATATYPE_FACTORIES ${RTM_IDL_LIBS} PARENT_SCOPE)
	endif()

endif(VXWORKS)

set(RTM_INCLUDE_IDLS ${RTM_INCLUDE_IDLS} PARENT_SCOPE)

#============================
# Installation settings
#============================
install(FILES  ${rtm_headers} DESTINATION ${INSTALL_RTM_INCLUDE_DIR}/rtm COMPONENT headers)
