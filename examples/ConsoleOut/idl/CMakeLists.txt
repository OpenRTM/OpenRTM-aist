set(idls )

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
