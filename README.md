# ulog

![](./example.png)

ulog (uber log) is a lightweight and threadsafe logging library for C based programs. It features color coded output, with the ability to send logs to stdout and a file. File and line information indicating what fired the log is also included. It has INFO, WARN, ERROR, and DEBUG log levels, and is thoroughly tested with cmocka and valgrind.

# usage

The following code produces the results shown in the aforementioned screenshot:

```C

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