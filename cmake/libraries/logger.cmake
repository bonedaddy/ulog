file(GLOB_RECURSE LOGGER_SOURCES
    ./include/*.h
    ./src/*.c 
)

add_library(liblogger ${LOGGER_SOURCES})
target_compile_options(liblogger PRIVATE ${flags})
target_link_libraries(liblogger pthread)


add_executable(logger-test-c ./tests/logger_test.c)
target_link_libraries(logger-test-c liblogger)
target_link_libraries(logger-test-c cmocka)
target_compile_options(logger-test-c PRIVATE ${flags})

add_executable(logger-test-cpp ./tests/logger_test.cpp)
target_link_libraries(logger-test-cpp liblogger)
target_link_libraries(logger-test-cpp cmocka)
target_compile_options(logger-test-cpp PRIVATE ${cxx-flags})


add_test(NAME LoggerTestC COMMAND logger-test-c)
add_test(NAME LoggerTestCpp COMMAND logger-test-cpp)