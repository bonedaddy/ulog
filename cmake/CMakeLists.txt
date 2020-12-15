# cmake file for use with clib

file(GLOB_RECURSE ULOG_SOURCES
    ./deps/ulog/*.h
    ./deps/ulog/*.c
)

add_library(libulog
    SHARED
        ${ULOG_SOURCES}
)

target_compile_options(libulog PRIVATE ${flags})
target_link_libraries(libulog pthread)