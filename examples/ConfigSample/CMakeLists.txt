cmake_minimum_required (VERSION 3.5.1)
set(target ConfigSample)
project (${target}
	VERSION ${RTM_VERSION}
	LANGUAGES CXX)


set(srcs ConfigSample.cpp ConfigSample.h)
examples_build(ConfigSample SRCS "${srcs}" MAIN ConfigSampleComp.cpp)

install(FILES rtc.conf DESTINATION ${INSTALL_RTM_EXAMPLE_DIR} COMPONENT examples)
install(FILES configsample.conf DESTINATION ${INSTALL_RTM_EXAMPLE_DIR} COMPONENT examples)
