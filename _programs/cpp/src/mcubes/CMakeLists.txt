# -------------------------------------
# Marching cubes library
# -------------------------------------

add_library(${MCUBES_LIBRARY} SHARED)

set(MCUBES_SOURCES
    ${COMMON_HEADERS}
    mcubes.h
    mcubes.cpp
    mcubes_utils.h
    mcubes_utils.cpp
)

target_sources(${MCUBES_LIBRARY} PUBLIC ${MCUBES_SOURCES})

get_target_property(SOURCE_LIST ${MCUBES_LIBRARY} SOURCES)
source_group("Source Files" FILES ${SOURCE_LIST})

# -------------------------------------
# Marching cubes executable
# -------------------------------------
set(BUILD_TARGET "march_cubes")
add_executable(${BUILD_TARGET})

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