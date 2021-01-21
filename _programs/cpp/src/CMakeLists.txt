# -------------------------------------
# Common headers
# -------------------------------------
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
file(GLOB COMMON_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/common/*.h")

# -------------------------------------
# Common targets
# -------------------------------------
set(MCUBES_LIBRARY "mcubes_static")

# -------------------------------------
# Build targets
# -------------------------------------
cmake_policy(SET CMP0076 NEW)
if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/mcubes")
    add_subdirectory(mcubes)
endif()

if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/icp")
    add_subdirectory(icp)
endif()

if (EXISTS  "${CMAKE_CURRENT_SOURCE_DIR}/surfrecon")
    add_subdirectory(surfrecon)
endif()

