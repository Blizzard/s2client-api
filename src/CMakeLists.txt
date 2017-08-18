# All the source files for the sc2 api
file(GLOB sources_sc2 "sc2api/*.cpp" "sc2api/*.cc" "../include/sc2api/*.h")
file(GLOB sources_sc2_lib "sc2lib/*.cpp" "sc2lib/*.cc" "../include/sc2lib/*.h")
file(GLOB sources_utils "sc2utils/*.cpp" "sc2utils/*.cc" "../include/sc2utils/*.h")
file(GLOB sources_renderer "sc2renderer/*.cc" "sc2renderer/*.h" "../include/sc2renderer/*.h")

# Gather the protos.
file(GLOB proto_files
    "../protocol/s2clientprotocol/*.proto"
)

# Create the output directory for generated protos.
file(MAKE_DIRECTORY ${PROJECT_BINARY_DIR}/generated)

foreach(proto ${proto_files})
    get_filename_component(proto_name ${proto} NAME_WE)
    list(APPEND proto_src ${PROJECT_BINARY_DIR}/generated/${proto_name}.pb.h ${PROJECT_BINARY_DIR}/generated/${proto_name}.pb.cc)
    # Get just headers for lib install
    list(APPEND sc2protocol_headers "${PROJECT_BINARY_DIR}/generated/${proto_name}.pb.h")
endforeach()

# Now include that directory
include_directories(SYSTEM "${PROJECT_BINARY_DIR}/generated")

# Create the library
add_library(sc2api ${sources_sc2})
add_library(sc2lib ${sources_sc2_lib})
add_library(sc2protocol ${proto_src} ${proto_files})
add_library(sc2utils ${sources_utils})

# Glob header files together for install
file(GLOB sc2api_headers "../include/sc2api/*.h")
file(GLOB sc2lib_headers "../include/sc2lib/*.h")
file(GLOB sc2utils_headers "../include/sc2utils/*.h")

message(STATUS "PROTOCOL ${sc2protocol_headers}")

# Set the public headers for install
set_target_properties(sc2api PROPERTIES PUBLIC_HEADER "${sc2api_headers}")
set_target_properties(sc2lib PROPERTIES PUBLIC_HEADER "${sc2lib_headers}")
set_target_properties(sc2protocol PROPERTIES PUBLIC_HEADER "${sc2protocol_headers}")
set_target_properties(sc2utils PROPERTIES PUBLIC_HEADER "${sc2utils_headers}")

# Install the libraries and their includes
install(
  TARGETS sc2api
  ARCHIVE DESTINATION "lib"
  PUBLIC_HEADER DESTINATION "include/sc2api"
)
install(
  TARGETS sc2lib
  ARCHIVE DESTINATION "lib"
  PUBLIC_HEADER DESTINATION "include/sc2lib"
)
install(
  TARGETS sc2protocol
  ARCHIVE DESTINATION "lib"
  PUBLIC_HEADER DESTINATION "include" # These are expected to be included as
                                      # "*.pb.h" not "dir/*.pb.h"
)
install(
  TARGETS sc2utils
  ARCHIVE DESTINATION "lib"
  PUBLIC_HEADER DESTINATION "include/sc2utils"
)

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
    file(GLOB sc2renderer_headers "../include/sc2renderer/*.h")
    set_target_properties(sc2renderer PROPERTIES PUBLIC_HEADER "${sc2renderer_headers}")
    install(
      TARGETS sc2renderer
      ARCHIVE DESTINATION "lib"
      PUBLIC_HEADER DESTINATION "include/sc2renderer"
    )
    set_target_properties(sc2renderer PROPERTIES DEBUG_POSTFIX "d")
    target_link_libraries(sc2renderer SDL2-static)
    include_directories("../contrib/SDL-mirror/include")
endif ()

# It requires includes to civetweb and our own headers.
include_directories("../contrib/civetweb/include")
include_directories("../include")

if(COMMAND cmake_policy)
    cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

if (MSVC)
    set(CMAKE_CXX_FLAGS_DEBUG "/MTd /ZI /Od" CACHE STRING "" FORCE)
    set(CMAKE_CXX_FLAGS_RELEASE "/MT" CACHE STRING "" FORCE)

    set_target_properties(c-library PROPERTIES COMPILE_FLAGS "/MT")
else()
    set(CMAKE_CXX_FLAGS "-fpermissive")
endif()

add_dependencies(sc2protocol protoc)

foreach (proto ${proto_files})
    get_filename_component(proto_name ${proto} NAME)
    get_filename_component(proto_name_we ${proto} NAME_WE)
    set(output ${PROJECT_BINARY_DIR}/generated/${proto_name_we}.pb.h)

    add_custom_command(
        DEPENDS protoc
        COMMAND ${PROJECT_BINARY_DIR}/bin/protoc -I=${CMAKE_CURRENT_SOURCE_DIR}/../protocol/s2clientprotocol --cpp_out=${PROJECT_BINARY_DIR}/generated ${proto}
        OUTPUT ${PROJECT_BINARY_DIR}/generated/${proto_name_we}.pb.cc ${output}
    )

endforeach()

if (WIN32)
    set_target_properties(sc2api PROPERTIES COMPILE_FLAGS "/W4 /WX")
    set_target_properties(sc2lib PROPERTIES COMPILE_FLAGS "/W4 /WX")
    set_target_properties(sc2renderer PROPERTIES COMPILE_FLAGS "/W4 /WX")
    set_target_properties(sc2utils PROPERTIES COMPILE_FLAGS "/W3 /WX")
    set_target_properties(sc2protocol PROPERTIES COMPILE_FLAGS "/W0")
endif (WIN32)

target_link_libraries(sc2api sc2protocol c-library)
target_link_libraries(sc2lib sc2api)
