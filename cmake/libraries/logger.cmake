file(GLOB_RECURSE LOGGER_SOURCES
    ./include/*.h
    ./src/*.c
)

add_library(liblogger ${LOGGER_SOURCES})
target_compile_options(liblogger PRIVATE ${flags})
