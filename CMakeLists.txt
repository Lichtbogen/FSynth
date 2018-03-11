project(FSynth)

cmake_minimum_required(VERSION 2.8)

set(LIBSRC 
  src/cshell.c  
  src/hull.c  
  src/logging.c  
  src/prompt.c   
  src/sequencer.c
  src/errors.c  
  src/list.c  
  src/samples.c
  src/wavefmt.c
)

set(PROGSRC
  src/main.c
)

add_library(fs ${LIBSRC})
add_executable(fsynth ${PROGSRC})
target_link_libraries(fsynth readline fs m)
