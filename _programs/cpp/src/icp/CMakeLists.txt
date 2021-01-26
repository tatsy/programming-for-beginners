set(BUILD_TARGET "icp_exe")
add_executable(${BUILD_TARGET})

target_include_directories(${BUILD_TARGET} PUBLIC ${EIGEN3_INCLUDE_DIRS})

set(SOURCE_FILES
    main.cpp
    icp.h
    icp.cpp
    svd.h
    svd.cpp)

target_sources(
    ${BUILD_TARGET}
    PRIVATE
    ${SOURCE_FILES}
    ${COMMON_HEADERS})

source_group("Source Files" FILES ${SOURCE_FILES})
source_group("Common Headers" FILES ${COMMON_HEADERS})

if (MSVC)
    target_compile_options(${BUILD_TARGET} PRIVATE "/Zi")
    set_target_properties(${BUILD_TARGET} PROPERTIES LINK_FLAGS "/DEBUG /PROFILE")
endif()
