cmake_minimum_required (VERSION 3.16)

if(POLICY CMP0144)
  cmake_policy(SET CMP0144 NEW)
endif()
if(POLICY CMP0167)
  cmake_policy(SET CMP0167 NEW)
endif()

find_package(Boost COMPONENTS filesystem)
if(Boost_FOUND)
	add_subdirectory(nameservice_file)
endif(Boost_FOUND)
