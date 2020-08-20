#include "logger.h"

int main(void) {
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