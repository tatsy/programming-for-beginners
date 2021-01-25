# -------------------------------------
# Marching cubes library
# -------------------------------------

add_library(${MCUBES_LIBRARY} STATIC)

set(SOURCE_FILES
    mcubes.h
    mcubes.cpp
    mcubes_utils.h
    mcubes_utils.cpp)

target_include_directories(${MCUBES_LIBRARY} PUBLIC ${EIGEN3_INCLUDE_DIRS})
target_sources(${MCUBES_LIBRARY} PUBLIC ${SOURCE_FILES} ${COMMON_HEADERS})

source_group("Source Files" FILES ${SOURCE_FILES})
source_group("Common Headers" FILES ${COMMON_HEADERS})

# -------------------------------------
# Marching cubes executable
# -------------------------------------
set(BUILD_TARGET "mcubes_exe")
add_executable(${BUILD_TARGET})

target_include_directories(${BUILD_TARGET} PUBLIC ${EIGEN3_INCLUDE_DIRS})
target_sources(
    ${BUILD_TARGET}
    PRIVATE
    main.cpp
)

target_link_libraries(${BUILD_TARGET} ${MCUBES_LIBRARY})

get_target_property(SOURCE_LIST ${BUILD_TARGET} SOURCES)
source_group("Source Files" FILES ${SOURCE_LIST})

if (MSVC)
    target_compile_options(${BUILD_TARGET} PRIVATE "/Zi")
    set_target_properties(${BUILD_TARGET} PROPERTIES LINK_FLAGS "/DEBUG /PROFILE")
endif()
