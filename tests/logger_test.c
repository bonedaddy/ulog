#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <assert.h>
#include <pthread.h>
#include "logger.h"
#include "colors.h"
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

void *test_thread_log(void *data) {
    thread_logger *thl = (thread_logger *)data;
    thl->log(thl, 0, "this is an info log", LOG_LEVELS_INFO, __FILENAME__, __LINE__);
    thl->logf(thl, 0, LOG_LEVELS_INFO, __FILENAME__, __LINE__, "%s\t%s", "one", "two");

    thl->log(thl, 0, "this is a warn log", LOG_LEVELS_WARN, __FILENAME__, __LINE__);
    thl->logf(thl, 0, LOG_LEVELS_WARN, __FILENAME__, __LINE__, "%s\t%s", "one", "two");

    thl->log(thl, 0, "this is an error log", LOG_LEVELS_ERROR, __FILENAME__, __LINE__);
    thl->logf(thl, 0, LOG_LEVELS_ERROR, __FILENAME__, __LINE__, "%s\t%s", "one", "two");

    thl->log(thl, 0, "this is a debug log", LOG_LEVELS_DEBUG, __FILENAME__, __LINE__);
    thl->logf(thl, 0, LOG_LEVELS_DEBUG, __FILENAME__, __LINE__, "%s\t%s", "one", "two");
    // commenting this out seems to get rid of memleaks reported by valgrind
    // pthread_exit(NULL);
    return NULL;
}

void *test_file_log(void *data) {
    file_logger *fhl = (file_logger *)data;
    fhl->thl->log(fhl->thl, fhl->fd, "this is an info log", LOG_LEVELS_INFO, __FILENAME__, __LINE__);
    fhl->thl->logf(fhl->thl, fhl->fd, LOG_LEVELS_INFO, __FILENAME__, __LINE__, "%s\t%s", "one", "two");

    fhl->thl->log(fhl->thl, fhl->fd, "this is a warn log", LOG_LEVELS_WARN, __FILENAME__, __LINE__);
    fhl->thl->logf(fhl->thl, fhl->fd, LOG_LEVELS_WARN, __FILENAME__, __LINE__, "%s\t%s", "one", "two");

    fhl->thl->log(fhl->thl, fhl->fd, "this is an error log", LOG_LEVELS_ERROR, __FILENAME__, __LINE__);
    fhl->thl->logf(fhl->thl, fhl->fd, LOG_LEVELS_ERROR, __FILENAME__, __LINE__, "%s\t%s", "one", "two");
    
    fhl->thl->log(fhl->thl, fhl->fd, "this is a debug log", LOG_LEVELS_DEBUG, __FILENAME__, __LINE__);
    fhl->thl->logf(fhl->thl, fhl->fd, LOG_LEVELS_DEBUG, __FILENAME__, __LINE__, "%s\t%s", "one", "two");
    // commenting this out seems to get rid of memleaks reported by valgrind
    // pthread_exit(NULL);
    return NULL;
}

#pragma GCC diagnostic ignored "-Wunused-parameter"
void test_thread_logger(void **state) {
    bool args[2] = {false, true};
    for (int i = 0; i < 2; i++) {
        thread_logger *thl = new_thread_logger(args[i]);
        assert(thl != NULL);
        thl->log(thl, 0, "this is an info log", LOG_LEVELS_INFO, __FILENAME__, __LINE__);
        thl->log(thl, 0, "this is a warn log", LOG_LEVELS_WARN, __FILENAME__, __LINE__);
        thl->log(thl, 0, "this is an error log", LOG_LEVELS_ERROR, __FILENAME__, __LINE__);
        thl->log(thl, 0, "this is a debug log", LOG_LEVELS_DEBUG, __FILENAME__, __LINE__);
        pthread_t threads[4];
        pthread_attr_t attrs[4];
        for (int i = 0; i < 4; i++) {
            pthread_attr_init(&attrs[i]);
            pthread_create(&threads[i], &attrs[i], test_thread_log, thl);
        }
        for (int i = 0; i < 4; i++) {
            pthread_join(threads[i], NULL);
            pthread_attr_destroy(&attrs[i]);
        }
        clear_thread_logger(thl);
    }
}

#pragma GCC diagnostic ignored "-Wunused-parameter"
void test_file_logger(void **state) {
    bool args[2] = {false, true};
    for (int i = 0; i < 2; i++) {
        file_logger *fhl = new_file_logger("file_logger_test.log", args[i]);
        assert(fhl != NULL);
        fhl->thl->log(fhl->thl, fhl->fd, "this is an info log", LOG_LEVELS_INFO, __FILENAME__, __LINE__);
        fhl->thl->log(fhl->thl, fhl->fd, "this is a warn log", LOG_LEVELS_WARN, __FILENAME__, __LINE__);
        fhl->thl->log(fhl->thl, fhl->fd, "this is an error log", LOG_LEVELS_ERROR, __FILENAME__, __LINE__);
        fhl->thl->log(fhl->thl, fhl->fd, "this is a debug log", LOG_LEVELS_DEBUG, __FILENAME__, __LINE__);
        pthread_t threads[4];
        pthread_attr_t attrs[4];
        for (int i = 0; i < 4; i++) {
            pthread_attr_init(&attrs[i]);
            pthread_create(&threads[i], &attrs[i], test_file_log, fhl);
        }
        for (int i = 0; i < 4; i++) {
            pthread_join(threads[i], NULL);
            pthread_attr_destroy(&attrs[i]);
        }
        clear_file_logger(fhl);
    }
}

typedef struct args {
    COLORS test_color;
    char *want_ansi;
} args;
typedef struct test {
    args args;
    char *name;
} test;

void test_print_color(void **state);
void test_get_ansi_color_scheme(void **state);
void validate_test_args(test testdata);
// TODO(bonedaddy): test format_colored

void validate_test_args(test testdata) {
    //   printf("%s\n", format_colored(testdata.args.test_color, testdata.name));
    char *scheme = get_ansi_color_scheme(testdata.args.test_color);
    if (strcmp(scheme, testdata.args.want_ansi) == 0) {

        printf("%stest %s passed\n", scheme, testdata.name);
        return;
    }
    printf("test %s failed\n", testdata.name);
}

#pragma GCC diagnostic ignored "-Wunused-parameter"
void test_get_ansi_color_scheme(void **state) {

    test tests[8] = {
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


void test_demo_log_thread(void **state) {
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
}

void test_demo_log_file(void **state) {
    file_logger *fhl = new_file_logger("testfile.log", true);
    
    fLOG_INFO(fhl, "this is an info log");
    fLOG_WARN(fhl, "this is a warn log");
    fLOG_ERROR(fhl, "this is an error log");
    fLOG_DEBUG(fhl, "this is a debug log");


    fLOGF_INFO(fhl, "this is a %s style info log", "printf");
    fLOGF_WARN(fhl, "this is a %s style warn log", "printf");
    fLOGF_ERROR(fhl, "this is a %s style error log", "printf");
    fLOGF_DEBUG(fhl, "this is a %s style debug log", "printf");

    // if you dont want to log to a file you will want to use the LOG_ and LOGF_ macros
    LOG_INFO(fhl->thl, 0, "this will only log to stdout");
    LOGF_INFO(fhl->thl, 0, "this will only log to %s", "stdout");

    clear_file_logger(fhl);
}


void test_write_colored(void **state) {
    int fd = open("testfile", O_CREAT, 0640);
    write_colored(COLORS_BLUE, fd, "hello world");
    close(fd);
}

int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_thread_logger),
        cmocka_unit_test(test_file_logger),
        cmocka_unit_test(test_demo_log_thread),
        cmocka_unit_test(test_demo_log_file),
        cmocka_unit_test(test_write_colored)
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}