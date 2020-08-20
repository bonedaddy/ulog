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

/*! @file logger.h
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

#pragma once

#include "colors.h"
#include <pthread.h>
#include <stdbool.h>
#include <string.h>

/*!
 * @brief strips leading path from __FILE__
 */
#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

/*!
 * @brief used to emit a standard INFO log
 * @param thl an instance of thread_logger, passing anything other than an
 * initialized thread_logger will result in undefined benhavior
 * @param fd the file descriptor to log to, set to 0 if you just want stdout logging
 * @param msg the actual message to log
 */
#define LOG_INFO(thl, fd, msg) \
    thl->log(thl, fd, msg, LOG_LEVELS_INFO, __FILENAME__, __LINE__);

/*!
 * @brief used to emit a standard WARN log
 * @param thl an instance of thread_logger, passing anything other than an
 * initialized thread_logger will result in undefined benhavior
 * @param fd the file descriptor to log to, set to 0 if you just want stdout logging
 * @param msg the actual message to log
 */
#define LOG_WARN(thl, fd, msg) \
    thl->log(thl, fd, msg, LOG_LEVELS_WARN, __FILENAME__, __LINE__);

/*!
 * @brief used to emit a standard ERROR log
 * @param thl an instance of thread_logger, passing anything other than an
 * initialized thread_logger will result in undefined benhavior
 * @param fd the file descriptor to log to, set to 0 if you just want stdout logging
 * @param msg the actual message to log
 */
#define LOG_ERROR(thl, fd, msg) \
    thl->log(thl, fd, msg, LOG_LEVELS_ERROR, __FILENAME__, __LINE__);

/*!
 * @brief used to emit a standard DEBUG log
 * @param thl an instance of thread_logger, passing anything other than an
 * initialized thread_logger will result in undefined benhavior
 * @param fd the file descriptor to log to, set to 0 if you just want stdout logging
 * @param msg the actual message to log
 * @note if logger is created without debug enabled, this is a noop
 */
#define LOG_DEBUG(thl, fd, msg) \
    thl->log(thl, fd, msg, LOG_LEVELS_DEBUG, __FILENAME__, __LINE__);

/*!
 * @brief used to emit a printf INFO log
 * @param thl an instance of thread_logger, passing anything other than an
 * initialized thread_logger will result in undefined benhavior
 * @param fd the file descriptor to log to, set to 0 if you just want stdout logging
 * @param msg the actual message to log
 * @param msg the printf styled message to format
 * @param ... the arguments to use for formatting
 */
#define LOGF_INFO(thl, fd, msg, ...) \
    thl->logf(thl, fd, LOG_LEVELS_INFO, __FILENAME__, __LINE__, msg, __VA_ARGS__);

/*!
 * @brief used to emit a printf WARN log
 * @param thl an instance of thread_logger, passing anything other than an
 * initialized thread_logger will result in undefined benhavior
 * @param fd the file descriptor to log to, set to 0 if you just want stdout logging
 * @param msg the actual message to log
 * @param msg the printf styled message to format
 * @param ... the arguments to use for formatting
 */
#define LOGF_WARN(thl, fd, msg, ...) \
    thl->logf(thl, fd, LOG_LEVELS_WARN, __FILENAME__, __LINE__, msg, __VA_ARGS__);

/*!
 * @brief used to emit a printf ERROR log
 * @param thl an instance of thread_logger, passing anything other than an
 * initialized thread_logger will result in undefined benhavior
 * @param fd the file descriptor to log to, set to 0 if you just want stdout logging
 * @param msg the actual message to log
 * @param msg the printf styled message to format
 * @param ... the arguments to use for formatting
 */
#define LOGF_ERROR(thl, fd, msg, ...) \
    thl->logf(thl, fd, LOG_LEVELS_ERROR, __FILENAME__, __LINE__, msg, __VA_ARGS__);

/*!
 * @brief used to emit a printf DEBUG log
 * @param thl an instance of thread_logger, passing anything other than an
 * initialized thread_logger will result in undefined benhavior
 * @param fd the file descriptor to log to, set to 0 if you just want stdout logging
 * @param msg the printf styled message to format
 * @param ... the arguments to use for formatting
 * @note if logger is created without debug enabled, this is a noop
 */
#define LOGF_DEBUG(thl, fd, msg, ...) \
    thl->logf(thl, fd, LOG_LEVELS_DEBUG, __FILENAME__, __LINE__, msg, __VA_ARGS__);

#ifdef __cplusplus
extern "C" {
#endif

/*! @struct base struct used by the thread_logger
 */
struct thread_logger;

/*! @typedef specifies log_levels, typically used when determining function
 * invocation by log_fn
 */
typedef enum {
    /*! indicates the message we are logging is of type info (color green) */
    LOG_LEVELS_INFO,
    /*! indicates the message we are logging is of type warn (color yellow) */
    LOG_LEVELS_WARN,
    /*! indicates the message we are logging is of type error (color red) */
    LOG_LEVELS_ERROR,
    /*! indicates the message we are logging is of type debug (color soft red) */
    LOG_LEVELS_DEBUG
} LOG_LEVELS;

/*! @typedef signature of pthread_mutex_unlock and pthread_mutex_lock used by the
 * thread_logger
 * @param mx pointer to a pthread_mutex_t type
 */
typedef int (*mutex_fn)(pthread_mutex_t *mx);

#ifdef __cplusplus
/*! @typedef signature used by the thread_logger for log_fn calls
 * @param thl pointer to an instance of thread_logger
 * @param file_descriptor file descriptor to write log messages to, if 0 then only
 * stdout is used
 * @param message the actual message we want to log
 * @param level the log level to use (effects color used)
 */
typedef void (*log_fn)(struct thread_logger *thl, int file_descriptor,
                       const char *message, LOG_LEVELS level, const char *file,
                       int line);
#else
/*! @typedef signature used by the thread_logger for log_fn calls
 * @param thl pointer to an instance of thread_logger
 * @param file_descriptor file descriptor to write log messages to, if 0 then only
 * stdout is used
 * @param message the actual message we want to log
 * @param level the log level to use (effects color used)
 */
typedef void (*log_fn)(struct thread_logger *thl, int file_descriptor, char *message,
                       LOG_LEVELS level, char *file, int line);
#endif

#ifdef __cplusplus
/*! @typedef signatured used by the thread_logger for printf style log_fn calls
 * @param thl pointer to an instance of thread_logger
 * @param file_descriptor file descriptor to write log messages to, if 0 then only
 * stdout is used
 * @param level the log level to use (effects color used)
 * @param message format string like `%sFOO%sBAR`
 * @param ... values to supply to message
 */
typedef void (*log_fnf)(struct thread_logger *thl, int file_descriptor,
                        LOG_LEVELS level, const char *file, int line,
                        const char *message, ...);
#else
/*! @typedef signatured used by the thread_logger for printf style log_fn calls
 * @param thl pointer to an instance of thread_logger
 * @param file_descriptor file descriptor to write log messages to, if 0 then only
 * stdout is used
 * @param level the log level to use (effects color used)
 * @param message format string like `%sFOO%sBAR`
 * @param ... values to supply to message
 */
typedef void (*log_fnf)(struct thread_logger *thl, int file_descriptor,
                        LOG_LEVELS level, char *file, int line, char *message, ...);
#endif

/*! @typedef a thread safe logger
 * @brief guards all log calls with a mutex lock/unlock
 * recommended usage is to call thread_logger:log(instance_of_thread_logger,
 * char*_of_your_log_message, log_level) alternatively you can call the `*_log`
 * functions directly
 */
typedef struct thread_logger {
    bool debug; /*! @brief indicates whether we will action on debug logs */
    pthread_mutex_t mutex; /*! @brief used for synchronization across threads */
    mutex_fn lock;         /*! @brief helper function for pthread_mutex_lock */
    mutex_fn unlock;       /*! @brief helper function for pthread_mutex_unlock */
    log_fn log; /*! @brief function that gets called for all regular logging */
    log_fnf
        logf; /*! @brief function that gets called for all printf style logging */
} thread_logger;

/*! @typedef a wrapper around thread_logger that enables file logging
 * @brief like thread_logger but also writes to a file
 * @todo
 *  - enable log rotation
 */
typedef struct file_logger {
    int fd; /*! @brief the file descriptor used for sending log information to */
    thread_logger *thl; /*! @brief the underlying threadsafe logger used for
                           sycnhronization and the actual logging */
} file_logger;

/*! @brief returns a new thread safe logger
 * if with_debug is false, then all debug_log calls will be ignored
 * @param with_debug whether to enable debug logging, if false debug log calls will
 * be ignored
 */
thread_logger *new_thread_logger(bool with_debug);

#ifdef __cplusplus
/*! @brief returns a new file_logger
 * Calls new_thread_logger internally
 * @param output_file the file we will dump logs to. created if not exists and is
 * appended to
 */
file_logger *new_file_logger(const char *output_file, bool with_debug);
#else
/*! @brief returns a new file_logger
 * Calls new_thread_logger internally
 * @param output_file the file we will dump logs to. created if not exists and is
 * appended to
 */
file_logger *new_file_logger(char *output_file, bool with_debug);
#endif

/*! @brief free resources for the threaded logger
 * @param thl the thread_logger instance to free memory for
 */
void clear_thread_logger(thread_logger *thl);

/*! @brief free resources for the file ogger
 * @param fhl the file_logger instance to free memory for. also frees memory for the
 * embedded thread_logger and closes the open file
 */
void clear_file_logger(file_logger *fhl);

/*! @brief main function you should call, which will delegate to the appopriate *_log
 * function
 * @param thl pointer to an instance of thread_logger
 * @param file_descriptor file descriptor to write log messages to, if 0 then only
 * stdout is used
 * @param message the actual message we want to log
 * @param level the log level to use (effects color used)
 */
void log_func(thread_logger *thl, int file_descriptor, char *message,
              LOG_LEVELS level, char *file, int line);

/*! @brief like log_func but for formatted logs
 * @param thl pointer to an instance of thread_logger
 * @param file_descriptor file descriptor to write log messages to, if 0 then only
 * stdout is used
 * @param level the log level to use (effects color used)
 * @param message format string like `<percent-sign>sFOO<percent-sign>sBAR`
 * @param ... values to supply to message
 */
void logf_func(thread_logger *thl, int file_descriptor, LOG_LEVELS level, char *file,
               int line, char *message, ...);

/*! @brief logs a debug styled message - called by log_fn
 * @param thl pointer to an instance of thread_logger
 * @param file_descriptor file descriptor to write log messages to in addition to
 * stdout logging. if 0 only stdout is used
 * @param message the actuall message to log
 */
void debug_log(thread_logger *thl, int file_descriptor, char *message);

/*! @brief logs a warned styled message - called by log_fn
 * @param thl pointer to an instance of thread_logger
 * @param file_descriptor file descriptor to write log messages to in addition to
 * stdout logging. if 0 only stdout is used
 * @param message the actuall message to log
 */
void warn_log(thread_logger *thl, int file_descriptor, char *message);

/*! @brief logs an error styled message - called by log_fn
 * @param thl pointer to an instance of thread_logger
 * @param file_descriptor file descriptor to write log messages to in addition to
 * stdout logging. if 0 only stdout is used
 * @param message the actuall message to log
 */
void error_log(thread_logger *thl, int file_descriptor, char *message);

/*! @brief logs an info styled message - called by log_fn
 * @param thl pointer to an instance of thread_logger
 * @param file_descriptor file descriptor to write log messages to in addition to
 * stdout logging. if 0 only stdout is used
 * @param message the actuall message to log
 */
void info_log(thread_logger *thl, int file_descriptor, char *message);

/*! @brief used to write a log message to file although this really means a file
 * descriptor
 * @param thl pointer to an instance of thread_logger
 * @param file_descriptor file descriptor to write log messages to in addition to
 * stdout logging. if 0 only stdout is used
 * @param message the actuall message to log
 */
int write_file_log(int file_descriptor, char *message);

/*! @brief returns a timestamp of format `Jul 06 10:12:20 PM`
 * @warning providing an input buffer whose length isnt at least 76 bytes will result
 * in undefined behavior
 * @param date_buffer the buffer to write the timestamp into
 * @param date_buffer_len the size of the buffer
 */
void get_time_string(char *date_buffer, size_t date_buffer_len);

#ifdef __cplusplus
}
#endif
