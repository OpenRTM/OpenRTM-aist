cmake_minimum_required (VERSION 3.0.2)

project (SimpleService
	VERSION ${RTM_VERSION}
	LANGUAGES CXX)

set(${PROJECT_NAME}_IDLS MyService.idl)

openrtm_idl_compile_all(MyService
	"${${PROJECT_NAME}_IDLS}"
	"-I${CMAKE_CURRENT_SOURCE_DIR};-I${CMAKE_SOURCE_DIR}/src/lib/rtm/idl")

include_directories(SYSTEM ${CMAKE_CURRENT_BINARY_DIR})

set(srcs MyServiceConsumer.cpp
	${MyService_IDLSTUB})
ComponentBuild(MyServiceConsumer "${srcs}" MyServiceConsumerComp.cpp "MyService_IDLTGT" "")

set(srcs
	MyServiceProvider.cpp
	MyServiceSVC_impl.cpp
	${MyService_IDLSKEL})
ComponentBuild(MyServiceProvider "${srcs}" MyServiceProviderComp.cpp "MyService_IDLTGT" "")
