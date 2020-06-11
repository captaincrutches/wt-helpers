cmake_minimum_required(VERSION 3.11.0 FATAL_ERROR)

# detect if we're a subproject
if(NOT DEFINED PROJECT_NAME)
  set(NOT_SUBPROJECT ON)
endif()

project(wt-helpers)

# Enforce C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Need at least these compiler versions, or we're not getting a full C++17 library
set(GCC_MIN_VERSION 7.0.0)
set(CLANG_MIN_VERSION 5.0.0)
set(APPLECLANG_MIN_VERSION 10.0.0)

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS ${GCC_MIN_VERSION})
    message(FATAL_ERROR "Insufficient GCC version (${CMAKE_CXX_COMPILER_VERSION} < ${GCC_MIN_VERSION})")

elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS ${CLANG_MIN_VERSION})
    message(FATAL_ERROR "Insufficient Clang version (${CMAKE_CXX_COMPILER_VERSION} < ${CLANG_MIN_VERSION})")

elseif(CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS ${APPLECLANG_MIN_VERSION})
    message(FATAL_ERROR "Insufficient AppleClang version (${CMAKE_CXX_COMPILER_VERSION} < ${APPLECLANG_MIN_VERSION})")

endif()

# Export compile_commands.json all the time, since iwyu and clang-tidy need it
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Submodules
add_subdirectory(thirdparty)

# Our Sources
add_subdirectory(src)

# Enable Wall
target_compile_options(wt-helpers PRIVATE -Wall -Wextra -Wno-unused-parameter)

# Option for Werror
option(WERROR "Treat warnings as errors" OFF)
if (WERROR)
    message(STATUS "Treating warnings as errors")
    target_compile_options(wt-helpers PRIVATE -Werror)
endif()

if (NOT_SUBPROJECT)
    # IWYU analysis.  These properties only affect targets created after they're set,
    # so setting them here ensures IWYU is run on our code but not Wt itself.
    find_program(IWYU_BINARY NAMES include-what-you-use)
    if (IWYU_BINARY)
        set(IWYU_ARGS
            -Xiwyu --cxx17ns
            -Xiwyu --mapping_file=${CMAKE_CURRENT_LIST_DIR}/wt-helpers.imp
        )

        find_program(IWYU_PY iwyu_tool.py)
        if (IWYU_PY)
            # Somewhat of a hack; causes the target to return non-zero (fail) if we got any "should"s from iwyu
            # i.e. it told us we had something incorrect
            add_custom_target(iwyu env PYTHONUNBUFFERED=true
                ${IWYU_PY} -p ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_LIST_DIR}/src -j8 -- ${IWYU_ARGS} 2>&1 | tee iwyu.log
                COMMAND grep -c "should" iwyu.log | xargs -I {} test 0 -eq {}
                VERBATIM USES_TERMINAL
            )
        else()
            message(WARNING "Couldn't find iwyu_tool.py")
        endif()

        # If IWYU was specified, include it in the build step
        option(IWYU "Enable IWYU analysis" OFF)
        if (IWYU)
            message(STATUS "Enabling IWYU analysis")
            set(CMAKE_CXX_INCLUDE_WHAT_YOU_USE ${IWYU_BINARY} ${IWYU_ARGS})
        else()
            message(STATUS "Not enabling IWYU analysis")
        endif()
    else()
        message(WARNING "Couldn't find include-what-you-use")
    endif()

    # clang-tidy analysis using .clang-tidy config file
    option(TIDY "Enable clang-tidy analysis" OFF)
    find_program(CLANG_TIDY_BINARY clang-tidy)

    if (CLANG_TIDY_BINARY)
        # run-clang-tidy.py lives in llvm/9/share/clang, which isn't in PATH
        get_filename_component(clang_bin ${CLANG_TIDY_BINARY} DIRECTORY)
        find_program(CLANG_TIDY_PY run-clang-tidy.py HINTS "${clang_bin}/../share/clang")

        # If we found run-clang-tidy.py, add a target to run just that
        # We can't pass -warnings-as-errors right to this, so it's set by default in .clang-tidy
        if (CLANG_TIDY_PY)
            add_custom_target(tidy env PYTHONUNBUFFERED=true ${CLANG_TIDY_PY}
                -clang-tidy-binary ${CLANG_TIDY_BINARY}
                -p ${CMAKE_CURRENT_BINARY_DIR}
                -quiet
                ${CMAKE_CURRENT_LIST_DIR}/src
                VERBATIM USES_TERMINAL
            )
        else()
            message(WARNING "Couldn't find run-clang-tidy.py")
        endif()

        # If TIDY was specified, include clang-tidy in the build step
        if (TIDY)
            message(STATUS "Enabling clang-tidy analysis")

            # warnings-as-errors is set by default in .clang-tidy for the standalone tidy target
            # If we chose not to enable Werror, override that here.
            if (NOT WERROR)
                set(TIDY_WERROR "-warnings-as-errors=''")
            endif()

            set(CMAKE_CXX_CLANG_TIDY ${CLANG_TIDY_BINARY}
                ${TIDY_WERROR}
            )
        else()
            message(STATUS "Not enabling clang-tidy analysis")
        endif()

    else()
        message(WARNING "Couldn't find clang-tidy")
    endif()
endif()

# And set some values for those including us to use
set(WT_HELPERS_INCLUDE_DIR ${CMAKE_CURRENT_LIST_DIR}/src CACHE PATH "Include directory for CC's WT helpers")
