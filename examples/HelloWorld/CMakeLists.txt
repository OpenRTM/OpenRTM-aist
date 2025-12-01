cmake_minimum_required (VERSION 3.16)
set(target HelloRTWorld)
project (${target}
	VERSION ${RTM_VERSION}
	LANGUAGES CXX)


set(srcs HelloRTWorld.cpp HelloRTWorld.h)
examples_build(HelloRTWorld SRCS "${srcs}" MAIN HelloRTWorldComp.cpp)
