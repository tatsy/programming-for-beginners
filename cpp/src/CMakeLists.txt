# -------------------------------------
# Common headers
# -------------------------------------
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
file(GLOB COMMON_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/common/*.h")

# -------------------------------------
# Common targets
# -------------------------------------
set(MCUBES_LIBRARY "mcubes")

# -------------------------------------
# Build targets
# -------------------------------------
cmake_policy(SET CMP0076 NEW)
add_subdirectory(mcubes)
add_subdirectory(surfrecon)
