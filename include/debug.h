#ifndef __DEBUG_H__
#define __DEBUG_H__

#include "common.h"
#include "x86.h"
#include "assert.h"

#define INTR assert(readf() & FL_IF)
#define NOINTR assert(~readf() & FL_IF)


void init_debug(void);
int printk(const char *fmt, ...);

#endif
