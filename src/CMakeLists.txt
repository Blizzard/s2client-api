# All the source files for the sc2 api
file(GLOB sources_sc2 "sc2api/*.cpp" "sc2api/*.cc" "../include/sc2api/*.h")
file(GLOB sources_sc2_lib "sc2lib/*.cpp" "sc2lib/*.cc" "../include/sc2lib/*.h")
file(GLOB sources_utils "sc2utils/*.cpp" "sc2utils/*.cc" "../include/sc2utils/*.h")
file(GLOB sources_renderer "sc2renderer/*.cc" "sc2renderer/*.h" "../include/sc2renderer/*.h")

# Gather the protos.
file(GLOB proto_files
    "../protocol/s2clientprotocol/*.proto"
)

if ("${proto_files}" STREQUAL "")
	message(FATAL_ERROR "Missing root protocol submodule.\nRemove your protocol directory and run:\ngit submodule update --recursive --init")
endif ()

# Create the output directory for generated protos.
file(MAKE_DIRECTORY ${PROJECT_BINARY_DIR}/generated/s2clientprotocol)

foreach(proto ${proto_files})
    get_filename_component(proto_name ${proto} NAME_WE)
    list(APPEND proto_src ${PROJECT_BINARY_DIR}/generated/s2clientprotocol/${proto_name}.pb.h ${PROJECT_BINARY_DIR}/generated/s2clientprotocol/${proto_name}.pb.cc)
endforeach()

# Now include that directory
include_directories(SYSTEM "${PROJECT_BINARY_DIR}/generated")

# Setup MSVC parallelized builds
add_compile_options($<$<CXX_COMPILER_ID:MSVC>:/MP>)

# Create the library
add_library(sc2api ${sources_sc2})
add_library(sc2lib ${sources_sc2_lib})
add_library(sc2protocol ${proto_src} ${proto_files})
add_library(sc2utils ${sources_utils})

set_target_properties(sc2api PROPERTIES DEBUG_POSTFIX "d")
set_target_properties(sc2lib PROPERTIES DEBUG_POSTFIX "d")
set_target_properties(sc2protocol PROPERTIES DEBUG_POSTFIX "d")
set_target_properties(sc2utils PROPERTIES DEBUG_POSTFIX "d")

target_link_libraries(sc2protocol libprotobuf)

if (APPLE)
    target_link_libraries(sc2utils "-framework Carbon")
endif ()

# Exclude SDL and related projects to work around linker issue.
if (NOT APPLE)
    add_library(sc2renderer ${sources_renderer})
    set_target_properties(sc2renderer PROPERTIES DEBUG_POSTFIX "d")
    target_link_libraries(sc2renderer SDL2-static)
    include_directories("../contrib/SDL-mirror/include")
endif ()

# It requires includes to civetweb and our own headers.
include_directories("../contrib/civetweb/include")
include_directories("../contrib/SDL-mirror/include")
include_directories("../contrib/ipv6-parse")
include_directories("../include")

if(COMMAND cmake_policy)
    cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

if (MSVC)
    set(CMAKE_CXX_FLAGS_DEBUG "/MTd /ZI /Od" CACHE STRING "" FORCE)
    set(CMAKE_CXX_FLAGS_RELEASE "/MT" CACHE STRING "" FORCE)

    set_target_properties(civetweb-c-library PROPERTIES COMPILE_FLAGS "/MT")
else()
    set(CMAKE_CXX_FLAGS "-fpermissive")  
endif()

add_dependencies(sc2protocol protoc)

foreach (proto ${proto_files})
    get_filename_component(proto_name ${proto} NAME)
    get_filename_component(proto_name_we ${proto} NAME_WE)
    set(outputCC ${PROJECT_BINARY_DIR}/generated/s2clientprotocol/${proto_name_we}.pb.cc)
    set(outputH ${PROJECT_BINARY_DIR}/generated/s2clientprotocol/${proto_name_we}.pb.h)

    add_custom_command(
        DEPENDS protoc
        COMMAND ${PROJECT_BINARY_DIR}/bin/protoc -I=${CMAKE_CURRENT_SOURCE_DIR}/../protocol/ --cpp_out=${PROJECT_BINARY_DIR}/generated ${proto}
        OUTPUT ${outputCC} ${outputH}
    )

endforeach()


if (MSVC)
    set_target_properties(sc2api PROPERTIES COMPILE_FLAGS "/W4 /WX")
    set_target_properties(sc2lib PROPERTIES COMPILE_FLAGS "/W4 /WX")
    set_target_properties(sc2renderer PROPERTIES COMPILE_FLAGS "/W4 /WX")
    set_target_properties(sc2utils PROPERTIES COMPILE_FLAGS "/W3 /WX")
    set_target_properties(sc2protocol PROPERTIES COMPILE_FLAGS "/W0")
endif (MSVC)

target_link_libraries(sc2api sc2protocol civetweb-c-library ipv6-parse)
target_link_libraries(sc2lib sc2api)
