cmake_minimum_required(VERSION 3.0.0)
project(CppBeginners)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

if (UNIX AND NOT APPLE)
    set(LINUX TRUE)
endif()

# ----------
# Options
# ----------
option(WITH_OPENMP "Use OpenMP" ON)

# ----------
# C++ compiler setting
# ----------
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if (UNIX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -Wall")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -g -Wall")
    if (${CMAKE_CXX_COMPILER_ID} MATCHES "Clang")
        set(CXX_FS_LIBRARY "c++fs")
    else()
        set(CXX_FS_LIBRARY "stdc++fs")
    endif()
endif()

# ----------
# Required packages
# ----------
find_package(Eigen3 REQUIRED)

if (WITH_OPENMP)
    find_package(OpenMP)
    if (OPENMP_FOUND)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
        message(STATUS "OpenMP enabled")
    endif()
endif()

# ----------
# OS specific settings
# ----------
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

if (MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")      # Parallel compilation
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHsc")    # Enable exception
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4251")  # Disable warning for improper text encoding
    add_definitions("-D_CRT_SECURE_NO_WARNINGS")
endif()

if (APPLE)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -framework Cocoa -framework OpenGL -framework IOKit -framework CoreVideo")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -framework Cocoa -framework OpenGL -framework IOKit -framework CoreVideo")
endif()

if (LINUX)
    find_package(PkgConfig REQUIRED)
    pkg_search_module(GLFW3 REQUIRED glfw3)
    set(ALL_LIBRARIES ${ALL_LIBRARIES} ${CMAKE_DL_LIBS} ${GLFW3_STATIC_LIBRARIES})
endif()

# ----------
# Folder settings
# ----------
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_DEBUG_POSTFIX "-debug")

# ----------
# Process subdirectories
# ----------
add_subdirectory(src)
