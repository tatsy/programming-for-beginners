include_directories(${CMAKE_CURRENT_SOURCE_DIR}/common)

cmake_policy(SET CMP0076 NEW)
add_subdirectory(mcubes)
add_subdirectory(surfrecon)
