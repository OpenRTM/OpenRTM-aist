cmake_minimum_required (VERSION 3.0.2)

project (${RTM_PROJECT_NAME}
	VERSION ${RTM_VERSION}
	LANGUAGES CXX)


set(DATA_TYPE_LIST
	TimedState
	TimedShort
	TimedLong
	TimedUShort
	TimedULong
	TimedFloat
	TimedDouble
	TimedChar
	TimedWChar
	TimedBoolean
	TimedOctet
	TimedString
	TimedWString
	TimedShortSeq
	TimedLongSeq
	TimedUShortSeq
	TimedULongSeq
	TimedFloatSeq
	TimedDoubleSeq
	TimedCharSeq
	TimedWCharSeq
	TimedBooleanSeq
	TimedOctetSeq
	TimedStringSeq
	TimedWStringSeq
	TimedRGBColour
	TimedPoint2D
	TimedVector2D
	TimedPose2D
	TimedVelocity2D
	TimedAcceleration2D
	TimedPoseVel2D
	TimedSize2D
	TimedGeometry2D
	TimedCovariance2D
	TimedPointCovariance2D
	TimedCarlike
	TimedSpeedHeading2D
	TimedPoint3D
	TimedVector3D
	TimedOrientation3D
	TimedPose3D
	TimedVelocity3D
	TimedAngularVelocity3D
	TimedAcceleration3D
	TimedAngularAcceleration3D
	TimedPoseVel3D
	TimedSize3D
	TimedGeometry3D
	TimedCovariance3D
	TimedSpeedHeading3D
	TimedOAP
	TimedQuaternion
	ActArrayActuatorPos
	ActArrayActuatorSpeed
	ActArrayActuatorCurrent
	ActArrayState
	CameraImage
	Fiducials
	GPSData
	GripperState
	INSData
	LimbState
	Hypotheses2D
	Hypotheses3D
	Features
	MultiCameraImages
	Path2D
	Path3D
	PointCloud
	PanTiltAngles
	PanTiltState
	RangeData
	IntensityData
)




set(ARTLINUX OFF)
set(HAVE_DLFCN_H ON)
set(HAVE_INTTYPES_H ON)
set(HAVE_MEMORY_H ON)
set(HAVE_NLIST_H ON)
set(HAVE_STDINT_H ON)
set(HAVE_STRINGS_H ON)
set(HAVE_STRING_H ON)
set(HAVE_SYS_STAT_H ON)
set(HAVE_SYS_TYPES_H ON)
set(HAVE_UNAME ON)
set(HAVE_UNISTD_H ON)
set(ORB_IS_MICO OFF)
set(ORB_IS_OMNIORB OFF)
set(ORB_IS_ORBEXPRESS OFF)
set(ORB_IS_ORBACUS OFF)
set(ORB_IS_ORBIT2 OFF)
set(ORB_IS_ORBIX OFF)
set(ORB_IS_RTORB OFF)
set(ORB_IS_TAO OFF)
set(RTM_PACKAGE OFF)
set(RTM_PACKAGE_BUGREPORT OFF)
set(RTM_PACKAGE_NAME OFF)
set(RTM_PACKAGE_STRING OFF)
set(RTM_PACKAGE_TARNAME OFF)
set(RTM_PACKAGE_URL OFF)
set(RTM_PACKAGE_VERSION OFF)
set(PYTHON_WRAPPER OFF)
set(RTC_CORBA_CXXMAPPING11 OFF)
set(RTC_SOCK_DATAPORT OFF)
set(RTM_GCC2 OFF)
set(RTM_OMNIORB_40 OFF)
set(RTM_OMNIORB_41 OFF)
set(RTM_OMNIORB_42 OFF)
set(RTM_OS_CYGWIN OFF)
set(RTM_OS_DARWIN OFF)
set(RTM_OS_FREEBSD OFF)
set(RTM_OS_HPUX OFF)
set(RTM_OS_IRIX OFF)
set(RTM_OS_LINUX OFF)
set(RTM_OS_VXWORKS OFF)
set(RTM_OS_OSF1 OFF)
set(RTM_OS_QNX OFF)
set(RTM_OS_SUNOS OFF)
set(RTM_OS_WIN32 OFF)
set(RTM_RTDSC OFF)
set(WITH_DMALLOC OFF)
set(_REENTRANT ON)


set(LT_OBJDIR .libs/)

if(VXWORKS)
	set(OSVERSION ON)
	set(OSVERSION ${VXWORKS_MAJOR}${VXWORKS_MINOR})
	set(RTM_OS_VXWORKS ON)
	set(STDC_HEADERS ON)
#	set(STDC_HEADERS_VALUE 1)
elseif(UNIX)
	set(STDC_HEADERS OFF)
	set(OSVERSION OFF)
	if(${CMAKE_SYSTEM_NAME}test MATCHES QNXtest)
		set(RTM_OS_QNX ON)
	else()
		set(RTM_OS_LINUX ON)
	endif()
elseif(WIN32)
	set(STDC_HEADERS OFF)
	set(OSVERSION OFF)
	set(RTM_OS_WIN32 ON)
endif()



if(CORBA MATCHES "ORBexpress")
	set (ORB_IS_ORBEXPRESS ON)
elseif(CORBA MATCHES "omniORB")
	if(OMNI_VERSION MATCHES "40")
		set(RTM_OMNIORB_40 ON)
	elseif(OMNI_VERSION MATCHES "41")
		set(RTM_OMNIORB_41 ON)
	else()
		set(RTM_OMNIORB_42 ON)
	endif()
	set (ORB_IS_OMNIORB ON)
elseif(CORBA MATCHES "TAO")
	set (ORB_IS_TAO ON)
endif()


set(RTC_CORBA_CXXMAPPING11 ON)



configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config_rtc_cmake.h.in ${PROJECT_BINARY_DIR}/config_rtc.h)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/version_cmake.h.in ${PROJECT_BINARY_DIR}/version.h)




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
	CORBA_CdrMemoryStream.h
	ByteData.h
	ByteDataStreamBase.h
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
	if(WIN32)
		add_definitions(-DLIBRARY_EXPORTS)
	endif(WIN32)

	add_library(${PROJECT_NAME}_objlib OBJECT ${rtm_srcs})
	openrtm_common_set_compile_props(${PROJECT_NAME}_objlib)
	openrtm_include_rtm(${PROJECT_NAME}_objlib)
	if(OBSERVER_ENABLE)
		target_compile_definitions(${PROJECT_NAME}_objlib PRIVATE ENABLE_OBSERVER)
		target_include_directories(${PROJECT_NAME}_objlib PRIVATE ${CMAKE_SOURCE_DIR}/src)
		target_include_directories(${PROJECT_NAME}_objlib SYSTEM PRIVATE ${CMAKE_BINARY_DIR}/src/ext/sdo/observer)
		add_dependencies(${PROJECT_NAME}_objlib ComponentObserverConsumer_IDLTGT)
	endif(OBSERVER_ENABLE)

	add_library(${PROJECT_NAME}-static STATIC
		$<TARGET_OBJECTS:${PROJECT_NAME}_objlib>
		$<TARGET_OBJECTS:IDL_OBJLIB>)

	if(WIN32)
		set(DEF_FILE_NAME ${PROJECT_NAME}.def)

		add_custom_command(TARGET ${PROJECT_NAME}-static
			POST_BUILD
			COMMAND python -B ${MAKE_DEF_COMMAND} $<TARGET_FILE:${PROJECT_NAME}-static> ${PROJECT_NAME} ${RTM_SHORT_VERSION} ${DEF_FILE_NAME}
			WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
		)
	endif(WIN32)

	add_library(${PROJECT_NAME} SHARED
	  $<TARGET_OBJECTS:${PROJECT_NAME}_objlib>
	  $<TARGET_OBJECTS:IDL_OBJLIB>
	  ${CMAKE_CURRENT_BINARY_DIR}/${DEF_FILE_NAME})
	set_target_properties(${PROJECT_NAME} PROPERTIES DEBUG_POSTFIX d)

	if(OBSERVER_ENABLE)
		target_link_libraries(${PROJECT_NAME} PRIVATE
			${ORB_LIBRARIES} ${COIL_PROJECT_NAME} ${RTM_LINKER_OPTION} ComponentObserverConsumer)
	else(OBSERVER_ENABLE)
		target_link_libraries(${PROJECT_NAME} PRIVATE
			${ORB_LIBRARIES} ${COIL_PROJECT_NAME} ${RTM_LINKER_OPTION})
	endif(OBSERVER_ENABLE)

	if(UNIX)
		install(TARGETS ${PROJECT_NAME} LIBRARY DESTINATION ${INSTALL_RTM_LIB_DIR}
						ARCHIVE DESTINATION ${INSTALL_RTM_LIB_DIR}
						RUNTIME DESTINATION ${INSTALL_RTM_BIN_DIR}
						COMPONENT openrtm)

		set_target_properties(${PROJECT_NAME} PROPERTIES VERSION ${RTM_VERSION} SOVERSION ${RTM_SO_VERSION})
		set_target_properties(${PROJECT_NAME}-static PROPERTIES DEBUG_POSTFIX d)
		set_target_properties(${PROJECT_NAME}-static PROPERTIES OUTPUT_NAME ${PROJECT_NAME} CLEAN_DIRECT_OUTPUT 1)
		install(TARGETS ${PROJECT_NAME}-static LIBRARY DESTINATION ${INSTALL_RTM_LIB_DIR}
						ARCHIVE DESTINATION ${INSTALL_RTM_LIB_DIR}
						RUNTIME DESTINATION ${INSTALL_RTM_BIN_DIR}
						COMPONENT openrtm)

	elseif(WIN32)
		add_dependencies(${PROJECT_NAME} ${RTCSKEL_PROJECT_NAME}_IDLTGT)
		set_source_files_properties(${DEF_FILE_NAME} PROPERTIES GENERATED 1)
		add_dependencies(${PROJECT_NAME} ${PROJECT_NAME}-static)
		install(TARGETS ${PROJECT_NAME} LIBRARY DESTINATION ${INSTALL_RTM_LIB_DIR}
						ARCHIVE DESTINATION ${INSTALL_RTM_LIB_DIR}
						RUNTIME DESTINATION ${INSTALL_RTM_LIB_DIR}
						COMPONENT openrtm)

		install(TARGETS ${PROJECT_NAME}-static LIBRARY DESTINATION ${INSTALL_RTM_LIB_DIR}
						ARCHIVE DESTINATION ${INSTALL_RTM_LIB_DIR}
						RUNTIME DESTINATION ${INSTALL_RTM_LIB_DIR}
						COMPONENT openrtm)

		foreach(data_type ${DATA_TYPE_LIST})
			set(RTC_DATA_TYPE RTC::${data_type})
			set(filename ${PROJECT_BINARY_DIR}/${data_type}Factory.cpp)
			set(DT_TARGET ${data_type}Factory_${RTM_VC_VER})
			configure_file(${CMAKE_CURRENT_SOURCE_DIR}/SerializerFactory.cpp.in ${filename} @ONLY)
			add_library(${DT_TARGET} SHARED ${filename})
			target_link_libraries(${DT_TARGET} PRIVATE
				${ORB_LIBRARIES} ${COIL_PROJECT_NAME} ${RTM_LINKER_OPTION} ${PROJECT_NAME})
			set_target_properties(${DT_TARGET} PROPERTIES DEBUG_POSTFIX d)
			add_dependencies(${DT_TARGET} ${PROJECT_NAME})
			set(DATATYPE_FACTORIES ${DATATYPE_FACTORIES} ${DT_TARGET})
			install(TARGETS ${DT_TARGET} LIBRARY DESTINATION ${INSTALL_RTM_LIB_DIR}
							ARCHIVE DESTINATION ${INSTALL_RTM_LIB_DIR}
							RUNTIME DESTINATION ${INSTALL_RTM_LIB_DIR}
							COMPONENT openrtm)
		endforeach()
		set(DATATYPE_FACTORIES ${DATATYPE_FACTORIES} PARENT_SCOPE)

	endif()
endif(VXWORKS)

install(FILES  ${rtm_headers} DESTINATION ${INSTALL_RTM_INCLUDE_DIR}/rtm COMPONENT headers)
