// Copyright 2020 Bonedaddy (Alexandre Trottier)
//
// licensed under GNU AFFERO GENERAL PUBLIC LICENSE;
// you may not use this file except in compliance with the License;
// You may obtain the license via the LICENSE file in the repository root;
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/*! @file logger.c
 * @brief a thread safe logger with optional printf style logging
 * @details allows writing color coded logs to stdout, with optional file output as
 * well. timestamps all logs, and provides optional printf style logging
 * @note logf_func has a bug where some format is respected and others are not,
 * consider the following from a `%s%s` format:
 *   - [error - Jul 06 10:01:07 PM] one<insert-tab-here>two
 *   - [warn - Jul 06 10:01:07 PM] one	two
 * @note warn, and info appear to not respect format, while debug and error do
 * @todo
 *  - buffer logs and use a dedicated thread for writing (avoid blocking locks)
 *  - handling system signals (exit, kill, etc...)
 */

#include "logger.h"
#include <fcntl.h>
#include <pthread.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#ifdef __cplusplus
extern "C" {
#endif

/*! @brief returns a new thread safe logger
 * if with_debug is false, then all debug_log calls will be ignored
 * @param with_debug whether to enable debug logging, if false debug log calls will
 * be ignored
 */
thread_logger *new_thread_logger(bool with_debug) {

    thread_logger *thl = malloc(sizeof(thread_logger));
    if (thl == NULL) {
        printf("failed to malloc thread_logger\n");
        return NULL;
    }

    thl->lock = pthread_mutex_lock;
    thl->unlock = pthread_mutex_unlock;
    thl->log = log_func;
    thl->logf = logf_func;
    thl->debug = with_debug;
    pthread_mutex_init(&thl->mutex, NULL);

    return thl;
}

/*! @brief returns a new file_logger
 * Calls new_thread_logger internally
 * @param output_file the file we will dump logs to. created if not exists and is
 * appended to
 */
file_logger *new_file_logger(char *output_file, bool with_debug) {

    thread_logger *thl = new_thread_logger(with_debug);
    if (thl == NULL) {
        // dont printf log here since new_thread_logger handles that
        return NULL;
    }

    file_logger *fhl = malloc(sizeof(file_logger));
    if (fhl == NULL) {
        // free thl as it is not null
        free(thl);
        printf("failed to malloc file_logger\n");
        return NULL;
    }

    // append to file, create if not exist, sync write files
    // TODO(bonedaddy): try to use O_DSYNC for data integrity sync
    int file_descriptor =
        open(output_file, O_WRONLY | O_CREAT | O_SYNC | O_APPEND, 0640);
    if (file_descriptor <= 0) {
        // free thl as it is not null
        free(thl);
        // free fhl as it is not null
        free(fhl);
        printf("failed to run posix open function\n");
        return NULL;
    }

    fhl->fd = file_descriptor;
    fhl->thl = thl;

    return fhl;
}

/*! @brief used to write a log message to file although this really means a file
 * descriptor
 * @param thl pointer to an instance of thread_logger
 * @param file_descriptor file descriptor to write log messages to in addition to
 * stdout logging. if 0 only stdout is used
 * @param message the actuall message to log
 */
int write_file_log(int file_descriptor, char *message) {

    char msg[strlen(message) + 2]; // 2 for \n
    memset(msg, 0, sizeof(msg));

    strcat(msg, message);
    strcat(msg, "\n");

    int response = write(file_descriptor, msg, strlen(msg));
    if (response == -1) {
        printf("failed to write file log message");
    } else {
        // this branch will be triggered if write doesnt fail
        // so overwrite the response to 0 as we want to return 0 to indicate
        // no error was received, and returning response directly would return the
        // number of bytes written
        response = 0;
    }

    return response;
}

/*! @brief like log_func but for formatted logs
 * @param thl pointer to an instance of thread_logger
 * @param file_descriptor file descriptor to write log messages to, if 0 then only
 * stdout is used
 * @param level the log level to use (effects color used)
 * @param message format string like `<percent-sign>sFOO<percent-sign>sBAR`
 * @param ... values to supply to message
 */
void logf_func(thread_logger *thl, int file_descriptor, LOG_LEVELS level, char *file,
               int line, char *message, ...) {

    va_list args;
    va_start(args, message);
    char msg[sizeof(args) + (strlen(message) * 2)];
    memset(msg, 0, sizeof(msg));

    int response = vsnprintf(msg, sizeof(msg), message, args);
    if (response < 0) {
        free(msg);
        printf("failed to vsprintf\n");
        return;
    }

    log_func(thl, file_descriptor, msg, level, file, line);
}

/*! @brief main function you should call, which will delegate to the appopriate *_log
 * function
 * @param thl pointer to an instance of thread_logger
 * @param file_descriptor file descriptor to write log messages to, if 0 then only
 * stdout is used
 * @param message the actual message we want to log
 * @param level the log level to use (effects color used)
 */
void log_func(thread_logger *thl, int file_descriptor, char *message,
              LOG_LEVELS level, char *file, int line) {

    char time_str[76];
    memset(time_str, 0, sizeof(time_str));

    get_time_string(time_str, 76);

    char location_info[strlen(file) + sizeof(line) + 4];
    memset(location_info, 0, sizeof(location_info));

    sprintf(location_info, " %s:%i", file, line);

    char
        date_msg[strlen(time_str) + strlen(message) + 2 + sizeof(location_info) + 6];
    memset(date_msg, 0, sizeof(date_msg));

    strcat(date_msg, time_str);
    strcat(date_msg, " -");
    strcat(date_msg, location_info);
    strcat(date_msg, "] ");
    strcat(date_msg, message);

    switch (level) {
        case LOG_LEVELS_INFO:
            info_log(thl, file_descriptor, date_msg);
            break;
        case LOG_LEVELS_WARN:
            warn_log(thl, file_descriptor, date_msg);
            break;
        case LOG_LEVELS_ERROR:
            error_log(thl, file_descriptor, date_msg);
            break;
        case LOG_LEVELS_DEBUG:
            debug_log(thl, file_descriptor, date_msg);
            break;
    }
}

/*! @brief logs an info styled message - called by log_fn
 * @param thl pointer to an instance of thread_logger
 * @param file_descriptor file descriptor to write log messages to in addition to
 * stdout logging. if 0 only stdout is used
 * @param message the actuall message to log
 */
void info_log(thread_logger *thl, int file_descriptor, char *message) {

    // 2, 1 for null terminator and 1 for space after ]
    size_t msg_size = strlen(message) + strlen("[info - ") + 2;
    char msg[msg_size];
    memset(msg, 0, sizeof(msg));

    thl->lock(&thl->mutex);

    strcat(msg, "[info - ");
    strcat(msg, message);

    if (file_descriptor != 0) {
        write_file_log(file_descriptor, msg);
    }

    print_colored(COLORS_GREEN, msg);

    thl->unlock(&thl->mutex);
}

/*! @brief logs a warned styled message - called by log_fn
 * @param thl pointer to an instance of thread_logger
 * @param file_descriptor file descriptor to write log messages to in addition to
 * stdout logging. if 0 only stdout is used
 * @param message the actuall message to log
 */
void warn_log(thread_logger *thl, int file_descriptor, char *message) {

    // 2, 1 for null terminator and 1 for space after ]
    size_t msg_size = strlen(message) + strlen("[warn - ") + 2;
    char msg[msg_size];
    memset(msg, 0, sizeof(msg));

    thl->lock(&thl->mutex);

    strcat(msg, "[warn - ");
    strcat(msg, message);

    if (file_descriptor != 0) {
        write_file_log(file_descriptor, msg);
    }

    print_colored(COLORS_YELLOW, msg);

    thl->unlock(&thl->mutex);
}

/*! @brief logs an error styled message - called by log_fn
 * @param thl pointer to an instance of thread_logger
 * @param file_descriptor file descriptor to write log messages to in addition to
 * stdout logging. if 0 only stdout is used
 * @param message the actuall message to log
 */
void error_log(thread_logger *thl, int file_descriptor, char *message) {

    // 2, 1 for null terminator and 1 for space after ]
    size_t msg_size = strlen(message) + strlen("[error - ") + 2;
    char msg[msg_size];
    memset(msg, 0, sizeof(msg));

    thl->lock(&thl->mutex);

    strcat(msg, "[error - ");
    strcat(msg, message);

    if (file_descriptor != 0) {
        write_file_log(file_descriptor, msg);
    }

    print_colored(COLORS_RED, msg);

    thl->unlock(&thl->mutex);
}

/*! @brief logs a debug styled message - called by log_fn
 * @param thl pointer to an instance of thread_logger
 * @param file_descriptor file descriptor to write log messages to in addition to
 * stdout logging. if 0 only stdout is used
 * @param message the actuall message to log
 */
void debug_log(thread_logger *thl, int file_descriptor, char *message) {

    if (thl->debug == false) {
        return;
    }

    // 2, 1 for null terminator and 1 for space after ]
    size_t msg_size = strlen(message) + strlen("[debug - ") + 2;
    char msg[msg_size];
    memset(msg, 0, sizeof(msg));

    thl->lock(&thl->mutex);

    strcat(msg, "[debug - ");
    strcat(msg, message);

    if (file_descriptor != 0) {
        write_file_log(file_descriptor, msg);
    }

    print_colored(COLORS_SOFT_RED, msg);

    thl->unlock(&thl->mutex);
}

/*! @brief free resources for the threaded logger
 * @param thl the thread_logger instance to free memory for
 */
void clear_thread_logger(thread_logger *thl) {

    pthread_mutex_lock(&thl->mutex); // lock before destroying
    pthread_mutex_destroy(&thl->mutex);
    free(thl);
}

/*! @brief free resources for the file ogger
 * @param fhl the file_logger instance to free memory for. also frees memory for the
 * embedded thread_logger and closes the open file
 */
void clear_file_logger(file_logger *fhl) {

    close(fhl->fd);
    clear_thread_logger(fhl->thl);
    free(fhl);
}

/*! @brief returns a timestamp of format `Jul 06 10:12:20 PM`
 * @warning providing an input buffer whose length isnt at least 76 bytes will result
 * in undefined behavior
 * @param date_buffer the buffer to write the timestamp into
 * @param date_buffer_len the size of the buffer
 */
void get_time_string(char *date_buffer, size_t date_buffer_len) {

    strftime(date_buffer, date_buffer_len, "%b %d %r",
             localtime(&(time_t){time(NULL)}));
}

#ifdef __cplusplus
}
#endif
