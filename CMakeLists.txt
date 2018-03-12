cmake_minimum_required(VERSION 2.8)
cmake_policy(SET CMP0048 NEW)

project(FSynth, VERSION "0.0.2.4")

# Project settings variables
set(PROJECT_LIBRARY_NAME fs)
set(PROJECT_EXE_NAME fsynth)

# Project settings header file
configure_file(
  ${PROJECT_SOURCE_DIR}/src/fs_config_in.h
  ${PROJECT_BINARY_DIR}/fs_config.h
)

# FS library source files
set(LIBRARY_SOURCE
  ${PROJECT_SOURCE_DIR}/src/cshell.c
  ${PROJECT_SOURCE_DIR}/src/hull.c
  ${PROJECT_SOURCE_DIR}/src/logging.c
  ${PROJECT_SOURCE_DIR}/src/sequencer.c
  ${PROJECT_SOURCE_DIR}/src/errors.c
  ${PROJECT_SOURCE_DIR}/src/list.c
  ${PROJECT_SOURCE_DIR}/src/samples.c
  ${PROJECT_SOURCE_DIR}/src/wavefmt.c
)

# FS tool source files
set(PROGRAM_SOURCE
  ${PROJECT_SOURCE_DIR}/src/main.c
)

# Additional link libraries for the main program
set(PROGRAM_EXT_LIBS
  readline
  m
)

# Add project binary for configuration files
include_directories(${PROJECT_BINARY_DIR})

# Build targets
add_library(${PROJECT_LIBRARY_NAME} ${LIBRARY_SOURCE})
add_executable(${PROJECT_EXE_NAME} ${PROGRAM_SOURCE})
if (${CMAKE_BUILD_TYPE} MATCHES "Release")
  set(PROGRAM_EXT_LIBS ${PROGRAM_EXT_LIBS} "-s")
endif(${CMAKE_BUILD_TYPE} MATCHES "Release")
target_link_libraries(${PROJECT_EXE_NAME} ${PROGRAM_EXT_LIBS} ${PROJECT_LIBRARY_NAME})

# Add install targets
install(TARGETS ${PROJECT_EXE_NAME} DESTINATION bin)
install(TARGETS ${PROJECT_LIBRARY_NAME} DESTINATION lib)
install(FILES ${PROJECT_SOURCE_DIR}/src/fsynth.h DESTINATION include)
install(FILES ${PROJECT_BINARY_DIR}/fs_config.h DESTINATION include)