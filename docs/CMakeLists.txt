cmake_minimum_required (VERSION 3.16)


project (docs
	VERSION ${RTM_VERSION}
	LANGUAGES CXX)

set(INPUT_RTM_DIRECTORY ${PROJECT_SOURCE_DIR}/../src/lib/rtm)
set(INPUT_COIL_DIRECTORY ${PROJECT_SOURCE_DIR}/../src/lib/coil)
set(INPUT_IDL_DIRECTORY ${PROJECT_SOURCE_DIR}/../src/lib/rtm/idl)
set(INPUT_EXT_DIRECTORY ${PROJECT_SOURCE_DIR}/../src/lib/rtm/ext)



set(ENABLED_SECTIONS jp)
set(OUTPUT_LANGUAGE Japanese)
set(OUTPUT_DIRECTORY IDLReference)




configure_file(doxygen_idlref.conf.cmake.in ${PROJECT_BINARY_DIR}/doxygen_idlref.conf @ONLY)

set(OUTPUT_DIRECTORY ClassReference)
configure_file(doxygen_classref.conf.cmake.in ${PROJECT_BINARY_DIR}/doxygen_classref.conf @ONLY)

add_custom_command(
	OUTPUT ${PROJECT_BINARY_DIR}/IDLReference/html/index.html
	COMMAND doxygen ${PROJECT_BINARY_DIR}/doxygen_idlref.conf
	WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
)
add_custom_target(IDLReference DEPENDS IDLReference/html/index.html)
add_dependencies(${RTM_PROJECT_NAME} IDLReference)


add_custom_command(
	OUTPUT ${PROJECT_BINARY_DIR}/ClassReference/html/index.html
	COMMAND doxygen ${PROJECT_BINARY_DIR}/doxygen_classref.conf
	WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
)
add_custom_target(ClassReference DEPENDS ClassReference/html/index.html)
add_dependencies(${RTM_PROJECT_NAME} ClassReference)




set(ENABLED_SECTIONS )
set(OUTPUT_LANGUAGE English)
set(OUTPUT_DIRECTORY IDLReference-en)


configure_file(doxygen_idlref.conf.cmake.in ${PROJECT_BINARY_DIR}/doxygen_idlref-en.conf @ONLY)


set(OUTPUT_DIRECTORY ClassReference-en)

configure_file(doxygen_classref.conf.cmake.in ${PROJECT_BINARY_DIR}/doxygen_classref-en.conf @ONLY)



add_custom_command(
	OUTPUT ${PROJECT_BINARY_DIR}/IDLReference-en/html/index.html
	COMMAND doxygen ${PROJECT_BINARY_DIR}/doxygen_idlref-en.conf
	WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
)
add_custom_target(IDLReference-en DEPENDS IDLReference-en/html/index.html)
add_dependencies(${RTM_PROJECT_NAME} IDLReference-en)


add_custom_command(
	OUTPUT ${PROJECT_BINARY_DIR}/ClassReference-en/html/index.html
	COMMAND doxygen ${PROJECT_BINARY_DIR}/doxygen_classref-en.conf
	WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
)
add_custom_target(ClassReference-en DEPENDS ClassReference-en/html/index.html)
add_dependencies(${RTM_PROJECT_NAME} ClassReference-en)


install(DIRECTORY ${PROJECT_BINARY_DIR}/IDLReference DESTINATION ${INSTALL_RTM_DOC_DIR}/idl COMPONENT documentation)
install(DIRECTORY ${PROJECT_BINARY_DIR}/ClassReference DESTINATION ${INSTALL_RTM_DOC_DIR}/c++ COMPONENT documentation)
install(DIRECTORY ${PROJECT_BINARY_DIR}/IDLReference-en DESTINATION ${INSTALL_RTM_DOC_DIR}/idl COMPONENT documentation)
install(DIRECTORY ${PROJECT_BINARY_DIR}/ClassReference-en DESTINATION ${INSTALL_RTM_DOC_DIR}/c++ COMPONENT documentation)
 


