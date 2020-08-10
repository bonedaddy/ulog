file(GLOB_RECURSE LOGGER_SOURCES
    ./include/*.h
    ./src/*.c
)

add_library(liblogger ${LOGGER_SOURCES})
target_compile_options(liblogger PRIVATE ${flags})
target_link_libraries(liblogger pthread)


add_executable(logger-test ./tests/logger_test.c)
target_link_libraries(logger-test liblogger)
target_link_libraries(logger-test cmocka)
target_compile_options(logger-test PRIVATE ${flags})
add_test(NAME LoggerTest COMMAND logger-test)