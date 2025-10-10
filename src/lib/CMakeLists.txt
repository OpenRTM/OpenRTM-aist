cmake_minimum_required (VERSION 3.16)

add_subdirectory(coil)
add_subdirectory(rtm)
add_subdirectory(hrtm)

set(RTM_INCLUDE_DIR ${RTM_INCLUDE_DIR} PARENT_SCOPE)
set(DATATYPE_FACTORIES ${DATATYPE_FACTORIES} PARENT_SCOPE)
set(RTM_INCLUDE_IDLS ${RTM_INCLUDE_IDLS} PARENT_SCOPE)
