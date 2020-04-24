set(BUILD_TARGET "surface_recon")
add_executable(${BUILD_TARGET})

target_include_directories(${BUILD_TARGET} PUBLIC ${EIGEN3_INCLUDE_DIRS})
target_sources(${BUILD_TARGET} PRIVATE main.cpp)

get_target_property(SOURCE_LIST ${BUILD_TARGET} SOURCES)
source_group("Source Files" FILES ${SOURCE_LIST})

if (MSVC)
    target_compile_options(${BUILD_TARGET} PRIVATE "/Zi")
    set_target_properties(${BUILD_TARGET} PROPERTIES LINK_FLAGS "/DEBUG /PROFILE")
endif()