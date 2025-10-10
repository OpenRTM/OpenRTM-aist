cmake_minimum_required (VERSION 3.16)

project (StringIO
	VERSION ${RTM_VERSION}
	LANGUAGES CXX)



set(srcs StringIn.cpp StringIn.h)
examples_build(StringIn SRCS "${srcs}" MAIN StringInComp.cpp)


set(srcs StringOut.cpp StringOut.h)
examples_build(StringOut SRCS "${srcs}" MAIN StringOutComp.cpp)
