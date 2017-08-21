# This file contains all of the install logic for the sc2api package. That is,
# this doesn't install the actual libraries and headers, but it provides the
# structures that allow a user to import and use those libraries and headers.

# Get the relative path from the cmake install dir to the include dir
file(
  RELATIVE_PATH
  REL_INCLUDE_DIR
  "${SC2API_INSTALL_CMAKE_DIR}"
  "${SC2API_INSTALL_INCLUDE_DIR}"
)

# Set up the variables to configure the SC2APIConfig.cmake file with
set(CONF_INCLUDE_DIRS "\${SC2API_CMAKE_DIR}/${REL_INCLUDE_DIR}")

# Configure the config file that will end up being installed
configure_file(
  "cmake/SC2APIConfig.cmake.in"
  "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/SC2APIConfig.cmake"
  @ONLY
)

# Install the SC2APIConfig.cmake file
install(
  FILES
  "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/SC2APIConfig.cmake"
  # "${PROJECT_BINARY_DIR}/SC2APIConfigVersion.cmake"
  DESTINATION "${SC2API_INSTALL_CMAKE_DIR}"
  COMPONENT cmake_package
)

# This is currently disabled due to some issues with the build process. Due to
# the sc2api target's dependence on CivetWeb's c-library target and the
# sc2renderer target's dependence on SDL2's SDL2-static target we encounter
# errors from cmake when trying to install our own export set. Please see
# issue #90 in the blizzard/s2client-api repo for further discussion.
# Install the export set for use with the install-tree
# install(
#   EXPORT SC2API_EXPORTS
#   DESTINATION "${SC2API_INSTALL_CMAKE_DIR}"
#   COMPONENT cmake_package
# )
