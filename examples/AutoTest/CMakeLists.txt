cmake_minimum_required (VERSION 3.0.2)

project (AutoTest
	VERSION ${RTM_VERSION}
	LANGUAGES CXX)

set(${PROJECT_NAME}_IDLS AutoTestService.idl)

openrtm_idl_compile_all(${PROJECT_NAME}
	"${${PROJECT_NAME}_IDLS}"
	"-I${CMAKE_CURRENT_SOURCE_DIR};-I${CMAKE_SOURCE_DIR}/src/lib/rtm/idl")

set(srcs AutoTestIn.cpp AutoTestServiceSVC_impl.cpp ${${PROJECT_NAME}_IDLSKEL})
ComponentBuild(AutoTestIn "${srcs}" AutoTestInComp.cpp "" "")
target_include_directories(AutoTestIn_objlib SYSTEM PUBLIC ${PROJECT_BINARY_DIR})

set(srcs AutoTestOut.cpp ${${PROJECT_NAME}_IDLSTUB})
ComponentBuild(AutoTestOut "${srcs}" AutoTestOutComp.cpp "" "")
target_include_directories(AutoTestOut_objlib SYSTEM PUBLIC ${PROJECT_BINARY_DIR})
