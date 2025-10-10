cmake_minimum_required (VERSION 3.16)

project (StaticFSM
	VERSION ${RTM_VERSION}
	LANGUAGES CXX)

examples_build(Display
  SRCS Display.cpp Display.h
  MAIN DisplayComp.cpp)

examples_build(Inputbutton
  SRCS Inputbutton.cpp Inputbutton.h
  MAIN InputbuttonComp.cpp)

examples_build(Microwave
  SRCS Microwave.cpp Microwave.h MicrowaveFsm.cpp MicrowaveFsm.h
  MAIN MicrowaveComp.cpp
  VXADDFILE ${CMAKE_SOURCE_DIR}/src/lib/rtm/Macho.cpp)
