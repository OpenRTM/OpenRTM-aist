cmake_minimum_required (VERSION 3.16)

file(GLOB XML_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.xml")
foreach(file_path ${XML_FILES})
	install(FILES ${file_path} DESTINATION ${INSTALL_RTM_EXAMPLE_DIR}/templates COMPONENT sources)
endforeach()


