#####################
# START CMAKE SETUP #
#####################

cmake_minimum_required(VERSION 3.0)
project(libcp2p LANGUAGES C CXX)
set(CMAKE_C_COMPILER /usr/bin/gcc)


# handle debug build type
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    # -g indicates to include debug symbols
    list(APPEND flags -std=c17 -Wall -Wextra -Werror -pedantic -g)
    list(APPEND cxx-flags -std=c++2a -Wall -Wextra -Werror -pedantic -g)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Analyze")
    # override gcc compiler to v10 for static analysis
    set(CMAKE_C_COMPILER /usr/bin/gcc-10)
    list(APPEND flags -std=c17 -Wall -Wextra -Werror -pedantic -g -fanalyzer)
    list(APPEND cxx-flags -std=c++2a -Wall -Wextra -Werror -pedantic -g -fanalyzer)
    message(STATUS "enabling static analysis")
endif()

# handle ci build type
if(CMAKE_BUILD_TYPE STREQUAL "CI")
    set(CMAKE_BUILD_TYPE "Release")
    list(APPEND flags -std=c17 -Wall -Wextra -Werror -pedantic -I/usr/local/lib)
    list(APPEND cxx-flags -std=c++2a -Wall -Wextra -Werror -pedantic -I/usr/local/lib)
    # override the previous C compiler choice
    set(CMAKE_C_COMPILER /usr/bin/gcc-9)
endif()


# handle no build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
    list(APPEND flags -std=c17 -Wall -Wextra -Werror -pedantic)
    list(APPEND cxx-flags -std=c++2a -Wall -Wextra -Werror -pedantic)

endif()

include_directories(${PROJECT_SOURCE_DIR}/include)
include(${PROJECT_SOURCE_DIR}/cmake/pkgs/doxygen.cmake)
include(${PROJECT_SOURCE_DIR}/cmake/pkgs/valgrind.cmake)
include(${PROJECT_SOURCE_DIR}/cmake/libraries/logger.cmake)


# build documentation
add_doxygen_doc(
    BUILD_DIR
        ${PROJECT_SOURCE_DIR}/build
    DOXY_FILE
        ${PROJECT_SOURCE_DIR}/docs/Doxyfile.in
    TARGET_NAME
        doxygen-docs
    COMMENT
        "HTML documentation"
)

# enable valgrind 
define_setup_valgrind()
# enable testing
enable_testing()