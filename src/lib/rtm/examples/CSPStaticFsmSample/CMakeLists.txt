set(CSP_SAMPLE_GROUP CSPStaticFsmSample)
set(CSP_SAMPLE_TARGETS)
csp_sample_component(Inputbutton)
csp_sample_component(Microwave MicrowaveFsm.cpp MicrowaveFsm.h)
csp_sample_configs()

# GCC 11 can reject Macho's inline state lookup when optimizing this example.
set_property(SOURCE Microwave.cpp APPEND PROPERTY COMPILE_OPTIONS
  "$<$<AND:$<CXX_COMPILER_ID:GNU>,$<VERSION_GREATER_EQUAL:${CMAKE_CXX_COMPILER_VERSION},11>,$<VERSION_LESS:${CMAKE_CXX_COMPILER_VERSION},12>,$<CONFIG:Release>>:-Wno-inline>")
