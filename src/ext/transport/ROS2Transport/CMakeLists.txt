cmake_minimum_required(VERSION 3.5)

project (ROS2Transport
	VERSION ${RTM_VERSION}
	LANGUAGES C CXX)


if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
	add_compile_options(-Wall -Wextra -Wpedantic)
endif()



find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(fastcdr REQUIRED)
find_package(fastrtps REQUIRED)




link_directories(${ORB_LINK_DIR})
add_definitions(${ORB_C_FLAGS_LIST})

if(WIN32)
	add_definitions(-DNOGDI)
	add_definitions(-DRTM_SKEL_IMPORT_SYMBOL)
endif()


set(target ROS2Transport)

set(srcs ROS2Transport.cpp ROS2Transport.h ROS2MessageInfo.cpp ROS2MessageInfo.h ROS2Serializer.cpp ROS2Serializer.h)

set(INSTALL_ROS2TRANSPORT_LIB_DIR ${INSTALL_RTM_EXT_DIR}/transport)
set(INSTALL_ROS2TRANSPORT_INCLUDE_DIR ${INSTALL_RTM_INCLUDE_DIR}/rtm/ext)
set(INSTALL_FASTRTPSTRANSPORT_LIB FastRTPSTransport)

if(VXWORKS AND NOT RTP)
	set(libs ${RTCSKEL_PROJECT_NAME} fastcdr fastrtps)

	add_executable(${target} ${srcs})
	openrtm_common_set_compile_props(${target})
	openrtm_set_link_props_shared(${target})
	openrtm_include_rtm(${target})
	target_include_directories(${target}
		PRIVATE ${PROJECT_SOURCE_DIR}/..)
	target_include_directories(${target} SYSTEM
		PUBLIC ${rclcpp_INCLUDE_DIRS}
		PUBLIC ${rmw_fastrtps_cpp_INCLUDE_DIRS}
		PUBLIC ${std_msgs_INCLUDE_DIRS}
		PUBLIC ${geometry_msgs_INCLUDE_DIRS}
		PUBLIC ${sensor_msgs_INCLUDE_DIRS})
	target_link_libraries(${target} ${libs})


	target_compile_definitions(${target} PRIVATE STD_MSGS_VERSION_MAJOR=${std_msgs_VERSION_MAJOR} 
								GEOMETORY_MSGS_VERSION_MAJOR=${geometry_msgs_VERSION_MAJOR} 
								SENSOR_MSGS_VERSION_MAJOR=${sensor_msgs_VERSION_MAJOR} 
								STD_MSGS_VERSION_MINOR=${std_msgs_VERSION_MINOR} 
								GEOMETORY_MSGS_VERSION_MINOR=${geometry_msgs_VERSION_MINOR} 
								SENSOR_MSGS_VERSION_MINOR==${sensor_msgs_VERSION_MINOR})
	if(${rclcpp_VERSION_MAJOR} GREATER 1)
		set_target_properties(${target} PROPERTIES
			POSITION_INDEPENDENT_CODE ON
			CXX_STANDARD 14
			CXX_EXTENSIONS OFF
			CXX_STANDARD_REQUIRED ON
		)
	endif()

	install(TARGETS ${target} LIBRARY DESTINATION ${INSTALL_ROS2TRANSPORT_LIB_DIR}
				ARCHIVE DESTINATION ${INSTALL_ROS2TRANSPORT_LIB_DIR}
				RUNTIME DESTINATION ${INSTALL_ROS2TRANSPORT_LIB_DIR}
				COMPONENT ext)
else()
	set(libs ${RTM_PROJECT_NAME} ${ORB_LIBRARIES} ${DATATYPE_FACTORIES} FastRTPSTransport 
		${rclcpp_LIBRARIES}  
		${std_msgs_LIBRARIES__rosidl_typesupport_fastrtps_cpp} 
		${geometry_msgs_LIBRARIES__rosidl_typesupport_fastrtps_cpp} 
		${sensor_msgs_LIBRARIES__rosidl_typesupport_fastrtps_cpp}
		fastcdr fastrtps)
	add_library(${target} SHARED ${srcs})
	openrtm_common_set_compile_props(${target})
	openrtm_include_rtm(${target})
	openrtm_set_link_props_shared(${target})
	target_include_directories(${target}
		PRIVATE ${PROJECT_SOURCE_DIR}/..)
	target_include_directories(${target} SYSTEM
					PUBLIC ${rclcpp_INCLUDE_DIRS}
					PUBLIC ${rmw_fastrtps_cpp_INCLUDE_DIRS}
					PUBLIC ${std_msgs_INCLUDE_DIRS}
					PUBLIC ${geometry_msgs_INCLUDE_DIRS}
					PUBLIC ${sensor_msgs_INCLUDE_DIRS})
	target_link_libraries(${target} PRIVATE ${libs} ${RTM_LINKER_OPTION})
	target_compile_definitions(${target} PRIVATE STD_MSGS_VERSION_MAJOR=${std_msgs_VERSION_MAJOR} 
								GEOMETORY_MSGS_VERSION_MAJOR=${geometry_msgs_VERSION_MAJOR} 
								SENSOR_MSGS_VERSION_MAJOR=${sensor_msgs_VERSION_MAJOR} 
								STD_MSGS_VERSION_MINOR=${std_msgs_VERSION_MINOR} 
								GEOMETORY_MSGS_VERSION_MINOR=${geometry_msgs_VERSION_MINOR} 
								SENSOR_MSGS_VERSION_MINOR==${sensor_msgs_VERSION_MINOR})
	if(${rclcpp_VERSION_MAJOR} GREATER 1)
		set_target_properties(${target} PROPERTIES
			POSITION_INDEPENDENT_CODE ON
			CXX_STANDARD 14
			CXX_EXTENSIONS OFF
			CXX_STANDARD_REQUIRED ON
		)
	endif()
	set_target_properties(${target} PROPERTIES PREFIX "")


	install(TARGETS ${target} LIBRARY DESTINATION ${INSTALL_ROS2TRANSPORT_LIB_DIR}
				ARCHIVE DESTINATION ${INSTALL_ROS2TRANSPORT_LIB_DIR}
				RUNTIME DESTINATION ${INSTALL_ROS2TRANSPORT_LIB_DIR}
				COMPONENT ext)
endif()

install(FILES ROS2MessageInfo.h ROS2Serializer.h DESTINATION ${INSTALL_ROS2TRANSPORT_INCLUDE_DIR}/ROS2Transport COMPONENT ext)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/ROS2TransportConfig.cmake.in ${PROJECT_BINARY_DIR}/ROS2TransportConfig.cmake @ONLY)
if(UNIX)
	install(FILES ${PROJECT_BINARY_DIR}/ROS2TransportConfig.cmake DESTINATION ${INSTALL_ROS2TRANSPORT_CMAKE_DIR} COMPONENT cmakefiles)
else()
	install(FILES ${PROJECT_BINARY_DIR}/ROS2TransportConfig.cmake DESTINATION ${INSTALL_RTM_CMAKE_DIR} COMPONENT cmakefiles)
endif()




if(VXWORKS)
	if(RTP)
	else(RTP)	
		set_target_properties(${target} PROPERTIES SUFFIX ".out")
	endif(RTP)
endif(VXWORKS)






