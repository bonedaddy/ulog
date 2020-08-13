# ulog

![](https://gateway.temporal.cloud/ipfs/QmYPyYVqcVjtziJrgsVGzk4mmh7HJ3wtEsh4yE24XLNtFK)

`ulog` (uber log) is a lightweight and threadsafe logging library for C based programs. It features color coded output, with the ability to send logs to stdout and a file. File and line information indicating what fired the log is also included. It has INFO, WARN, ERROR, and DEBUG log levels, and is thoroughly tested with cmocka and valgrind. 

If not using debug logging then any DEBUG level log calls are silently skipped. The logger is threadsafe in that multiple threads can't log at the same time. In practice there is very little lock contention and in all honesty you will probably never have to worry about it.

In terms of memory usage, the only memory allocations conducted by this library are when initializing the logger. During actual logging there is no memory allocations whatsoever, as we use stack allocated variables. In practice logger initialization consumes arounds 7.4 KiB of memory, while regular logger usage general consumes no more than 3 -> 3.4 KiB of memory at any one time.

**Please be aware that after calling `clear_thread_logger` or `clear_file_logger` using the logger results in undefined behavior, likely a panic causing the program to exit. Having one or more threads initiate a log invocation while concurrently calling `clear_thread_logger` or `clear_file_logger` results in undefined behavior. When clearing the logger you must be certain no other threads will attempt to use the logger.**

# why another logging library?

I wanted a simple logging library that didnt leak memory, was well tested, and capable of color coded output. All logging libraries I found were complex code bases, leaked memory, and relied on global variables. Because of that, and as a way to better learn C development `ulog` was born.

Interested in reading more about how `ulog` was born? [I published a blog post detailing the creation](https://bonedaddy.io/blog/misc/ulog_lightweight_threadsafe/).

# versioning

The library follows semver as the versioning scheme. Additinoally the header files have `LOGGER_VERSION` and `COLORS_VERSION` macros which include the current release number. This is to help situations in which you may be using the library my copy and pasting the code into some other repository.

# installation

## manual (broke)

Copy and paste the `logger.h`, `colors.h`, `logger.c`, and `colors.c` files into whatever project you are working on. You will need to make sure that you have pthreads available to link with as the logger library has a pthreads dependency.

## clib (woke)

If you use the [clib package manager](https://github.com/clibs/clib) then you can install `ulog` into your project with a single command:

```shell
$> clib install bonedaddy/ulog
```

# testing

Before testing you'll need to compile the code, for which you have two options

Release mode

```shell
$> make
```

Debug mode

```shell
$> make build-debug
```

Running either of the following two commands will build a test executable `logger-test` within the `build` folder. You can either invoke this manually or use `ctest`.

Regular unit testing:

```shell
$> ctest
```

Valgrind dynamic analysis and unit testing:

```shell
$> ctest -T memcheck
```

# usage

The following code samples produce the output shown in the screenshot at the start of the readme.

## no file logging

```C
#include <stdbool.H>
#include "logger.h"

thread_logger *thl = new_thread_logger(true);

LOG_INFO(thl, 0, "this is an info log");
LOG_WARN(thl, 0, "this is a warn log");
LOG_ERROR(thl, 0, "this is an error log");
LOG_DEBUG(thl, 0, "this is a debug log");

// the LOGF_ prefixed functions can be used for printf styled output
LOGF_INFO(thl, 0, "this is a %s style info log", "printf");
LOGF_WARN(thl, 0, "this is a %s style warn log", "printf");
LOGF_ERROR(thl, 0, "this is a %s style error log", "printf");
LOGF_DEBUG(thl, 0, "this is a %s style debug log", "printf");

// free up memory when you no longer need the logger
// note: after this returns thl is no logner safe to use
clear_thread_logger(thl);
```

## file logging

```C
#include <stdbool.h>
#include "logger.h"


file_logger *fhl = new_file_logger("testfile.log", true);

LOG_INFO(fhl->thl, fhl->fd, "this is an info log");
LOG_WARN(fhl->thl, fhl->fd,"this is a warn log");
LOG_ERROR(fhl->thl, fhl->fd, "this is an error log");
LOG_DEBUG(fhl->thl, fhl->fd, "this is a debug log");

LOGF_INFO(fhl->thl, fhl->fd, "this is a %s style info log", "printf");
LOGF_WARN(fhl->thl, fhl->fd, "this is a %s style warn log", "printf");
LOGF_ERROR(fhl->thl, fhl->fd, "this is a %s style error log", "printf");
LOGF_DEBUG(fhl->thl, fhl->fd, "this is a %s style debug log", "printf");

// if you dont want to loger to a file and just stdout, simply set the `fhl->fd` value to 0
LOG_INFO(fhl->thl, 0, "this will only log to stdout");
LOGF_INFO(fhl->thl, 0, "this will only log to %s", "stdout");

clear_file_logger(fhl);
```

# license

AGPLv3 licensed, although if you want commercial license under MIT that can be aranged for a small fee.