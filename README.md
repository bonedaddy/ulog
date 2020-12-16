# ulog

![](./example.png)

`ulog` (uber log) is a lightweight and threadsafe logging library written in C, with support for C++. It features color coded output, with the ability to send logs to stdout and a file. File and line information indicating what fired the log is also included. It has INFO, WARN, ERROR, and DEBUG log levels, and is thoroughly tested with cmocka and valgrind. 

If not using debug logging then any DEBUG level log calls are silently skipped. The logger is threadsafe in that multiple threads can't log at the same time. In practice there is very little lock contention and in all honesty you will probably never have to worry about it.

In terms of memory usage, the only memory allocations conducted by this library are when initializing the logger. During actual logging there is no memory allocations whatsoever, as we use stack allocated variables. In practice logger initialization consumes arounds 7.4 KiB of memory, while regular logger usage general consumes no more than 3 -> 3.4 KiB of memory at any one time.

**Please be aware that after calling `clear_thread_logger` or `clear_file_logger` using the logger results in undefined behavior, likely a panic causing the program to exit. Having one or more threads initiate a log invocation while concurrently calling `clear_thread_logger` or `clear_file_logger` results in undefined behavior. When clearing the logger you must be certain no other threads will attempt to use the logger.**

# features

* C/C++ support
* lightweight (3 -> 3.4 KiB memory consumption)
* threadsafe
* color coded logs
* stdout and file descriptor logging
* file and line number that emitted the log included

# why another logging library?

I wanted a simple logging library that didnt leak memory, was well tested, and capable of color coded output. All logging libraries I found were complex code bases, leaked memory, and relied on global variables. Because of that, and as a way to better learn C development `ulog` was born.

Interested in reading more about how `ulog` was born? [I published a blog post detailing the creation](https://bonedaddy.io/blog/misc/ulog_lightweight_threadsafe/).

# versioning

The library follows the semver versioning scheme. Additionally a `version.h` header file has the current release version listed as a macro.

# installation

## manual (broke)

Copy and paste the `logger.h`, `colors.h`, `version.h`, `logger.c`, and `colors.c` files into whatever project you are working on. You will need to make sure that you have pthreads available to link with as the logger library has a pthreads dependency.

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

The primary method of interacting with ulog is by using macros. The macros allow you to emit logs at various levels, minimizing the amount of typing required to do so. There are a total of four macros that can be used, the base macros are denoted in the form of `LOG_<LEVEL>` and `LOGF_<LEVEL>` which provide the capabilities to emit logs to standard out. The `LOG_` macros can be used to emit a log message as is, that is to say you provide a single message to emit, while the `LOGF_` macros can be used to emit a log message formatted according to the printf formatting rules leveraging variadic arguments. 

There are two additional macros that mimic `LOG_<LEVEL>` and `LOGF_<LEVEL>` however they will also log to a given file, while also logging to stdout. They are `fLOG_<LEVEL>` and `fLOG_<LEVEL>`. The main difference between these, other than the fact that file logging capabilities are provided, is that the `LOG_<LEVEL>` and `LOGF_<LEVEL>` macros take in an instance of `thread_logger` while the `fLOG_<LEVEL>` and `fLOGF_<LEVEL>` macros take in an instance of `file_logger`.

Below you'll find examples that showcase how to generate the logs that were captured in the screenshot displayed at the beginning of this readme.

## stdout only

```C
#include <stdbool.H>
#include "logger.h"

thread_logger *thl = new_thread_logger(true);

LOG_INFO(thl, "this is an info log");
LOG_WARN(thl, "this is a warn log");
LOG_ERROR(thl, "this is an error log");
LOG_DEBUG(thl, "this is a debug log");

// the LOGF_ prefixed functions can be used for printf styled output
LOGF_INFO(thl, "this is a %s style info log", "printf");
LOGF_WARN(thl, "this is a %s style warn log", "printf");
LOGF_ERROR(thl, "this is a %s style error log", "printf");
LOGF_DEBUG(thl, "this is a %s style debug log", "printf");

// free up memory when you no longer need the logger
// note: after this returns thl is no logner safe to use
clear_thread_logger(thl);
```

## file and stdout

```C
#include <stdbool.h>
#include "logger.h"


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
LOG_INFO(fhl->thl, "this will only log to stdout");
LOGF_INFO(fhl->thl, "this will only log to %s", "stdout");

clear_file_logger(fhl);
```

# license

AGPLv3 licensed, although if you want commercial license under MIT that can be aranged for a small fee.