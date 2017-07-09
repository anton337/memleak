#ifndef DBG_DEFS
#define DBG_DEFS

#include <stdio.h>
#include <execinfo.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include "location.h"

void INFO(const char * msg,size_t size,size_t addr,void * dat);

void * dbg_malloc(size_t num_bytes);

void dbg_free(void * dat);


#endif

