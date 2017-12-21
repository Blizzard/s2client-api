cmake_minimum_required(VERSION 3.1)

option(BUILD_API_EXAMPLES "Build Examples" ON)
option(BUILD_API_TESTS "Build Tests" ON)
project(s2client-api)

# Use bin as the directory for all executables.
# This will make protoc easy to find.
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

# Windows builds subdirectories Debug/Release.
# These variables will overwrite that and put binaries in bin.
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${PROJECT_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${PROJECT_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${PROJECT_BINARY_DIR}/bin)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${PROJECT_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${PROJECT_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${PROJECT_BINARY_DIR}/bin)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_MINSIZEREL ${PROJECT_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_MINSIZEREL ${PROJECT_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL ${PROJECT_BINARY_DIR}/bin)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELWITHDEBINFO ${PROJECT_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELWITHDEBINFO ${PROJECT_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${PROJECT_BINARY_DIR}/bin)

# Build with c++14 support.
set(CMAKE_CXX_STANDARD 14)

# Disable default libraries in visual studio
if (MSVC)
    # Default to statically-linked runtime.
    if("${MSVC_RUNTIME}" STREQUAL "")
        set(MSVC_RUNTIME "static")
    endif()

    # Set compiler options.
    set(variables
        CMAKE_C_FLAGS_DEBUG
        CMAKE_C_FLAGS_MINSIZEREL
        CMAKE_C_FLAGS_RELEASE
        CMAKE_C_FLAGS_RELWITHDEBINFO
        CMAKE_CXX_FLAGS_DEBUG
        CMAKE_CXX_FLAGS_MINSIZEREL
        CMAKE_CXX_FLAGS_RELEASE
        CMAKE_CXX_FLAGS_RELWITHDEBINFO
    )
    if(${MSVC_RUNTIME} STREQUAL "static")
        message(STATUS
          "MSVC -> forcing use of statically-linked runtime."
        )
        foreach(variable ${variables})
            if(${variable} MATCHES "/MD")
                string(REGEX REPLACE "/MD" "/MT" ${variable} "${${variable}}")
            endif()
        endforeach()
    else()
        message(STATUS
            "MSVC -> forcing use of dynamically-linked runtime."
        )
        foreach(variable ${variables})
            if(${variable} MATCHES "/MT")
                string(REGEX REPLACE "/MT" "/MD" ${variable} "${${variable}}")
            endif()
        endforeach()
    endif()
endif ()

# Allow creating filters for projects in visual studio.
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# -- Settings for building civetweb
# Don't build civetweb tests.
set(CIVETWEB_BUILD_TESTING OFF CACHE BOOL "" FORCE)
set(CIVETWEB_ENABLE_WEBSOCKETS ON CACHE BOOL "" FORCE)
set(CIVETWEB_ENABLE_SERVER_EXECUTABLE OFF CACHE BOOL "" FORCE)
set(CIVETWEB_ENABLE_IPV6 ON CACHE BOOL "" FORCE)

# Don't build civetweb with sanitizers
set(CIVETWEB_ENABLE_ASAN OFF CACHE BOOL "" FORCE)

# Settings for ipv6 parse library
set(IPV6_PARSE_LIBRARY_ONLY ON CACHE BOOL "" FORCE)

# Don't build protobuf tests.
set(protobuf_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(protobuf_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

# Don't build SDL dynamic lib.
set(SDL_SHARED OFF CACHE BOOL "" FORCE)

# Run civetwebs cmake.
add_subdirectory("contrib/civetweb")

# Enable IPv6 in civetweb
target_compile_options(civetweb-c-library PUBLIC -DUSE_IPV6=1)

# Run protobufs cmake.
add_subdirectory("contrib/protobuf/cmake")

# Run ipv6-parse cmake.
add_subdirectory("contrib/ipv6-parse")

set_target_properties(civetweb-c-library PROPERTIES FOLDER contrib)
set_target_properties(libprotobuf PROPERTIES FOLDER contrib)
set_target_properties(libprotobuf-lite PROPERTIES FOLDER contrib)
set_target_properties(libprotoc PROPERTIES FOLDER contrib)
set_target_properties(protoc PROPERTIES FOLDER contrib)
set_target_properties(ipv6-parse PROPERTIES FOLDER contrib)

if (MSVC)
    set_target_properties(libprotobuf libprotobuf-lite libprotoc protoc PROPERTIES COMPILE_FLAGS "/W0")
    set_source_files_properties(${libprotobuf_files} PROPERTIES COMPILE_FLAGS "/W0")
    set_source_files_properties(${protobuf_SHARED_OR_STATIC} PROPERTIES COMPILE_FLAGS "/W0")
    set_source_files_properties(${libprotobuf_lite_files} PROPERTIES COMPILE_FLAGS "/W0")
    add_definitions(-D_SCL_SECURE_NO_WARNINGS)
endif (MSVC)

# Exclude SDL and related projects to work around linker issue.
if (NOT APPLE)
	# Run SDLs cmake.
	add_subdirectory("contrib/SDL-mirror")

	set_target_properties(SDL2main PROPERTIES FOLDER contrib)
	set_target_properties(SDL2-static PROPERTIES FOLDER contrib)
endif ()

add_subdirectory("src")

# Exclude SDL and related projects to work around linker issue.
if (NOT APPLE)
    set_target_properties(sc2renderer PROPERTIES FOLDER utilities)
    set_target_properties(uninstall PROPERTIES FOLDER CMakePredefinedTargets)
endif ()

if (BUILD_API_EXAMPLES)
    add_subdirectory("examples")
endif ()

if (BUILD_API_TESTS)
    add_subdirectory("tests")
endif ()
