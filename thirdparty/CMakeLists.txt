## Wt dependency

# Don't build extras
set(BUILD_EXAMPLES OFF CACHE BOOL "Don't build Wt examples")
set(BUILD_TESTS OFF CACHE BOOL "Don't build Wt tests")
set(ENABLE_LIBWTTEST OFF CACHE BOOL "Don't build Wt tests")

# Find installed package or use submodule
set(USE_WT_SUBMODULE OFF)
find_package(Wt QUIET)
if(NOT Wt_FOUND)
    message(STATUS "Couldn't find system Wt, using submodule instead")
    set(USE_WT_SUBMODULE ON)
endif()

if(USE_WT_SUBMODULE)
    add_subdirectory(wt)
    set(Wt_INCLUDE_DIR ${WT_SOURCE_DIR}/src PATH "Wt include directory from submodule")
endif()
