#ifndef DBG_DEFS
#define DBG_DEFS

#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include "location.h"

int printStackTrace(location_definition * loc);

void INFO(const char * msg,size_t size,size_t addr,void * dat);

void * dbg_malloc(size_t num_bytes);

void dbg_free(void * dat);

void push(location_definition * loc);

location_definition * pop(void * loc);

void print_summary();


#endif

