include_directories(${CMAKE_CURRENT_SOURCE_DIR}/common)

cmake_policy(SET CMP0076 NEW)
add_subdirectory(march_cubes)
add_subdirectory(surface_recon)
