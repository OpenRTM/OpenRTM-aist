if(OpenRTM_aist_BINARY_DIR)
  openrtm_idl_compile_all(MyServiceProvider
    MyService.idl
    "-I${CMAKE_CURRENT_SOURCE_DIR}/include/MyServiceConsumer;-I${CMAKE_SOURCE_DIR}/src/lib/rtm/idl"
    MyService "")

  set(skel ${CMAKE_CURRENT_BINARY_DIR}/MyServiceSkel.cpp)
  set(MyServiceProvider_IDLSKEL ${skel} PARENT_SCOPE)

  set(srcs ../src/MyServiceProvider.cpp ../src/MyServiceSVC_impl.cpp ../include/MyServiceProvider/MyServiceProvider.h)
  examples_build(MyServiceProvider
    SRCS "${srcs}"
    GENSRCS ${MyServiceProvider_IDLSKEL}
    MAIN ../src/MyServiceProviderComp.cpp
    EXTERNAL_INCLUDE PUBLIC ../include/MyServiceProvider  ${CMAKE_CURRENT_BINARY_DIR}
    DEPS MyServiceProvider_IDLTGT)
else(OpenRTM_aist_BINARY_DIR)

set(idls ${CMAKE_CURRENT_SOURCE_DIR}/MyService.idl )

macro(_IDL_OUTPUTS _idl _dir _result)
    set(${_result} ${_dir}/${_idl}Skel.cpp ${_dir}/${_idl}Skel.h)
endmacro(_IDL_OUTPUTS)

macro(_COMPILE_IDL _idl_file)

    get_filename_component(_idl ${_idl_file} NAME_WE)
    set(_idl_srcs_var ${_idl}_SRCS)
    _IDL_OUTPUTS(${_idl} ${CMAKE_CURRENT_BINARY_DIR} ${_idl_srcs_var})

    if(WIN32)
        set(_python_command "python")
    else(WIN32)
        set(_python_command "python3")
    endif(WIN32)

    if(${OPENRTM_VERSION_MAJOR} LESS 2)
        if(NOT WIN32)
            execute_process(COMMAND rtm-config --prefix OUTPUT_VARIABLE OPENRTM_DIR
            OUTPUT_STRIP_TRAILING_WHITESPACE)
            execute_process(COMMAND rtm-config --idlflags OUTPUT_VARIABLE OPENRTM_IDLFLAGS
            OUTPUT_STRIP_TRAILING_WHITESPACE)
            separate_arguments(OPENRTM_IDLFLAGS)
            execute_process(COMMAND rtm-config --idlc OUTPUT_VARIABLE OPENRTM_IDLC
            OUTPUT_STRIP_TRAILING_WHITESPACE)
            set(_rtm_skelwrapper_command "rtm-skelwrapper")
        else(NOT WIN32)
            set(_rtm_skelwrapper_command "rtm-skelwrapper.py")
        endif(NOT WIN32)
        get_filename_component(_idl ${_idl_file} NAME_WE)
        set(_idl_srcs_var ${_idl}_SRCS)
        _IDL_OUTPUTS(${_idl} ${CMAKE_CURRENT_BINARY_DIR} ${_idl_srcs_var})
        set(OPENRTM_IDL_WRAPPER ${_rtm_skelwrapper_command})
        set(OPENRTM_IDL_WRAPPER_FLAGS --include-dir= --skel-suffix=Skel --stub-suffix=Stub --idl-file=${_idl}.idl)
    endif()
 
    add_custom_command(OUTPUT ${${_idl_srcs_var}}
        COMMAND ${_python_command} ${OPENRTM_DIR}/bin/${OPENRTM_IDL_WRAPPER} ${OPENRTM_IDL_WRAPPER_FLAGS} --idl-file=${_idl}.idl
        COMMAND ${OPENRTM_IDLC} ${OPENRTM_IDLFLAGS} ${_idl_file}
        WORKING_DIRECTORY ${CURRENT_BINARY_DIR}
        DEPENDS ${_idl_file}
        COMMENT "Compiling ${_idl_file}" VERBATIM)
    add_custom_target(${_idl}_TGT DEPENDS ${${_idl_srcs_var}})
    set(ALL_IDL_SRCS ${ALL_IDL_SRCS} ${${_idl_srcs_var}})
    if(NOT TARGET ALL_IDL_TGT)
        add_custom_target(ALL_IDL_TGT)
    endif(NOT TARGET ALL_IDL_TGT)
    add_dependencies(ALL_IDL_TGT ${_idl}_TGT)
endmacro(_COMPILE_IDL)

# Module exposed to the user
macro(OPENRTM_COMPILE_IDL_FILES)
    foreach(idl ${ARGN})
        _COMPILE_IDL(${idl})
    endforeach(idl)
endmacro(OPENRTM_COMPILE_IDL_FILES)


OPENRTM_COMPILE_IDL_FILES(${idls})
set(ALL_IDL_SRCS ${ALL_IDL_SRCS} PARENT_SCOPE)
FILTER_LIST("ALL_IDL_SRCS" "hh$" idl_headers)

endif(OpenRTM_aist_BINARY_DIR)
