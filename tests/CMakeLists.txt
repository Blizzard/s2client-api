include_directories("../include")
include_directories("../examples/common")
include_directories("${PROJECT_BINARY_DIR}/generated")

file(GLOB test_SRC
    "*.h"
    "*.cc"
)

# Setup MSVC parallelized builds
add_compile_options($<$<CXX_COMPILER_ID:MSVC>:/MP>)

#all_tests
add_executable(all_tests ${test_SRC})
set_target_properties(all_tests PROPERTIES FOLDER tests)

if (MSVC)
	# set_target_properties(all_tests PROPERTIES LINK_FLAGS "/NODEFAULTLIB:libcmt")
endif ()

if (MSVC AND (NOT (MSVC_VERSION LESS 1910)))
	target_link_libraries(all_tests legacy_stdio_definitions)
endif ()

target_link_libraries(all_tests sc2api sc2lib sc2utils)
