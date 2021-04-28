project(minisdp)

set(DMINISDP ${CMAKE_CURRENT_SOURCE_DIR}/mini_sdp)

add_compile_options(-Wl,--no-undefined -std=c++11 -g -D__STDC_FORMAT_MACROS)
if (NOT CMAKE_BUILD_TYPE MATCHES "Debug")
  add_compile_options(-O2)
endif()

include_directories(${DMINISDP})
aux_source_directory(${DMINISDP} SRCS)

add_library(minisdp STATIC ${SRCS})

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/test)
