#include <string.h>
#include <stdio.h>
#include "logger.h"

typedef struct args {
    COLORS test_color;
    const char *want_ansi;
} args_t;
typedef struct test {
    const char *name;
    args_t args;
} test_t;


void validate_test_args(test testdata) {
    //   printf("%s\n", format_colored(testdata.args.test_color, testdata.name));
    char *scheme = get_ansi_color_scheme(testdata.args.test_color);
    if (strcmp(scheme, testdata.args.want_ansi) == 0) {

        printf("%stest %s passed\n", scheme, testdata.name);
        return;
    }
    printf("test %s failed\n", testdata.name);
}

int main(void) {
    // thread logger tests
    thread_logger *thl = new_thread_logger(true);
    LOG_INFO(thl, 0, "this is an info log");
    LOG_WARN(thl, 0, "this is a warn log");
    LOG_ERROR(thl, 0, "this is an error log");
    LOG_DEBUG(thl, 0, "this is a debug log");
    LOGF_INFO(thl, 0, "this is a %s style info log", "printf");
    LOGF_WARN(thl, 0, "this is a %s style warn log", "printf");
    LOGF_ERROR(thl, 0, "this is a %s style error log", "printf");
    LOGF_DEBUG(thl, 0, "this is a %s style debug log", "printf");
    clear_thread_logger(thl);
    
    // file logger tests
    file_logger *fhl = new_file_logger("testfile.log", true);
    fLOG_INFO(fhl, "this is an info log");
    fLOG_WARN(fhl, "this is a warn log");
    fLOG_ERROR(fhl, "this is an error log");
    fLOG_DEBUG(fhl, "this is a debug log");
    fLOGF_INFO(fhl, "this is a %s style info log", "printf");
    fLOGF_WARN(fhl, "this is a %s style warn log", "printf");
    fLOGF_ERROR(fhl, "this is a %s style error log", "printf");
    fLOGF_DEBUG(fhl, "this is a %s style debug log", "printf");
    LOG_INFO(fhl->thl, 0, "this will only log to stdout");
    LOGF_INFO(fhl->thl, 0, "this will only log to %s", "stdout");
    clear_file_logger(fhl);

    test_t tests[8] = {
        {
            .name = "red", 
            .args = {
                .test_color = COLORS_RED,
                .want_ansi = ANSI_COLOR_RED,
            },
        },
        {
            .name = "soft_red",
            .args = {
                .test_color = COLORS_SOFT_RED,
                .want_ansi = ANSI_COLOR_SOFT_RED,
            },
        },
        {
            .name = "green", 
            .args = {
                .test_color = COLORS_GREEN,
                .want_ansi = ANSI_COLOR_GREEN,
            },
        },
        {
            .name = "yellow", 
            .args = {
                .test_color = COLORS_YELLOW,
                .want_ansi = ANSI_COLOR_YELLOW,
            },
        },
        {
            .name = "blue", 
            .args = {
                .test_color = COLORS_BLUE,
                .want_ansi = ANSI_COLOR_BLUE,
            },
        },
        {
            .name = "magenta", 
            .args = {
                .test_color = COLORS_MAGENTA,
                .want_ansi = ANSI_COLOR_MAGENTA,
            },
        },
        {
            .name = "cyan", 
            .args = {
                .test_color = COLORS_CYAN,
                .want_ansi = ANSI_COLOR_CYAN,
            },
        },
        {
            .name = "reset", 
            .args = {
                .test_color = COLORS_RESET,
                .want_ansi = ANSI_COLOR_RESET,
            },
        },
    };
    for (int i = 0; i < 7; i++) {
        validate_test_args(tests[i]);
    }
}