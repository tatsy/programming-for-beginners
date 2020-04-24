include(FindPackageHandleStandardArgs)

set(EIGEN3_DIR "EIGEN3_DIR" CACHE PATH "")

find_path(EIGEN3_INCLUDE_DIR
    NAMES Eigen/Core
    PATHS
    /usr/include
    /usr/include/eigen3
    /usr/local/include
    /usr/local/include/eigen3
    ${EIGEN3_DIR}
    ${EIGEN3_DIR}/include)

find_package_handle_standard_args(Eigen3 DEFAULT_MSG EIGEN3_INCLUDE_DIR)

if (EIGEN3_FOUND)
    message(STATUS "Eigen3 include: ${EIGEN3_INCLUDE_DIR}")
    set(EIGEN3_INCLUDE_DIRS ${EIGEN3_INCLUDE_DIR})
    set(EIGEN_INCLUDE_DIR ${EIGEN3_INCLUDE_DIR})
    set(EIGEN_INCLUDE_DIRS ${EIGEN3_INCLUDE_DIR})
    mark_as_advanced(EIGEN3_INCLUDE_DIR)
endif()
