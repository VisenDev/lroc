#ifndef LOG_H
#define LOG_H

#include "allocator.h"

struct Logger;

typedef void (*LogFn)(struct Logger, const char * fmt, ...);
typedef void (*LogDeinitFn)(struct Logger, Allocator); 

typedef struct Logger {
    void * ctx;
    LogFn log;
} Logger;

#endif /*LOG_H*/

#ifdef LOG_IMPLEMENTATION

#include <stdio.h>
#include <time.h>
#include <assert.h>
#include <stdarg.h>

void file_log(struct Logger self, const char * fmt, ...) {
    File * file = self.ctx;

    /*log timestamp*/
    time_t rawtime;
    struct tm * timeinfo;

    time (&rawtime);
    timeinfo = localtime (&rawtime);
    printf ("Current local time and date: %s", asctime(timeinfo));

    va_args args;
    va_start(args, fmt);
    vfprintf(file, fmt, args);
    va_end(args);
}

Logger file_logger(const char * filepath) {
    Logger self = {0};
    self.ctx = fopen(filepath, "w");
    assert(self.ctx);

    self.log = file_log;

}



#endif
