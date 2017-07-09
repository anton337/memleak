#ifndef ALLOCATE_H
#define ALLOCATE_H

#include "dbg_defs.h"

void * operator new [] (std::size_t size) throw (std::bad_alloc)
{
#ifdef __DBG__
  return dbg_malloc(size);
#else
  return malloc(size);
#endif
}

void * operator new (std::size_t size) throw (std::bad_alloc)
{
#ifdef __DBG__
  return dbg_malloc(size);
#else
  return malloc(size);
#endif
}

void operator delete [] (void * ptr)
{
#ifdef __DBG__
  dbg_free(ptr);
#else
  free(ptr);
#endif
}

void operator delete (void * ptr)
{
#ifdef __DBG__
  dbg_free(ptr);
#else
  free(ptr);
#endif
}

#endif
