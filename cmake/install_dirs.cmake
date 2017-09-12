# This file sets up the paths necessary for installing the sc2api executables,
# libraries, includes, and cmake package.

# Offer the user the choice of overriding the installation directories.
# Notes:
#  - These are APPENDED TO CMAKE_INSTALL_PREFIX if they aren't absolute paths.
#    If the user specifies an absolute path from the command line then they're
#    used verbatim.
#  - The include directory provided is */include/sc2api. Inside this directory
#    will be the other include directories and files. E.g.
#    */include/sc2api/sc2api/. sc2_api.h is still included via
#    # include <sc2api/sc2_api.h>

# Set up lib, bin, include install destination dirs
set(
  SC2API_INSTALL_LIB_DIR
  "lib/sc2api"
   CACHE
   PATH
   "Installation directory for libraries"
)
set(
  SC2API_INSTALL_BIN_DIR
  "bin/sc2api"
  CACHE
  PATH
  "Installation directory for executables"
)
set(
  SC2API_INSTALL_INCLUDE_DIR
  "include/sc2api"
  CACHE
  PATH
  "Installation directory for header files"
)

# Choose our cmake file install dir based on OS
if(WIN32 AND NOT CYGWIN)
  set(DEF_INSTALL_CMAKE_DIR cmake)
else()
  set(DEF_INSTALL_CMAKE_DIR lib/cmake/SC2API)
endif()

# Set up actual cmake file install dir
set(
  SC2API_INSTALL_CMAKE_DIR
  ${DEF_INSTALL_CMAKE_DIR}
  CACHE
  PATH
  "Installation directory for CMake files"
)

# Make relative paths absolute
foreach(p LIB BIN INCLUDE CMAKE)
  set(var SC2API_INSTALL_${p}_DIR)
  if(NOT IS_ABSOLUTE "${${var}}")
    set(${var} "${CMAKE_INSTALL_PREFIX}/${${var}}")
  endif()
endforeach()
