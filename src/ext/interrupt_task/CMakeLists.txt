cmake_minimum_required (VERSION 3.0.2)



set(target interrupt_task)

project (${target}
	VERSION ${RTM_VERSION}
	LANGUAGES CXX)

add_executable(${target} ${target}.cpp)
openrtm_common_set_compile_props(${target})

set_target_properties(${target} PROPERTIES SUFFIX ".out")