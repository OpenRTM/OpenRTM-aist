cmake_minimum_required (VERSION 3.16)

project (Analyzer
	VERSION ${RTM_VERSION}
	LANGUAGES CXX)


set(srcs Analyzer.cpp Analyzer.h)
examples_build(Analyzer SRCS "${srcs}" MAIN AnalyzerComp.cpp)



set(srcs Analyzer_test.cpp Analyzer_test.h)
examples_build(Analyzer_test SRCS "${srcs}" MAIN Analyzer_testComp.cpp)
