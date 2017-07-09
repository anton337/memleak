#include <iostream>

#include "dbg_defs.h"

#include "dbg_map.h"

#include "location.h"

void INFO(const char * msg,size_t size,size_t addr,void * dat)
{
  fprintf(stderr, "%s:%zu:0x%zx\n", msg , size , addr );
  void *arrays[BT_BUF_SIZE];
  char **strings;
  size_t bt_size;
  bt_size = backtrace(arrays, BT_BUF_SIZE);
  strings = backtrace_symbols(arrays,bt_size);
  location_definition* loc_dat = (location_definition*)dat;
  loc_dat->ram_addr = (size_t)addr;
  loc_dat->size = size;
  loc_dat->bt_size = bt_size;
  for(int i=0;i<BT_BUF_SIZE&&i<bt_size;i++)
  {
    int j=0;
    while(j<MAX_LEN)
    {
      loc_dat->array[i][j] = strings[i][j];
      if(strings[i][j]=='\0')break;
      j++;
    }
  }
  free(strings);
  //backtrace_symbols_fd(array, bt_size, STDERR_FILENO);
}

void * dbg_malloc(size_t num_bytes)
{
  void * addr = malloc(num_bytes);
  location_definition * loc = (location_definition*)malloc(sizeof(location_definition));
  INFO("malloc",num_bytes,(size_t)addr,loc);
  push(loc);
  return addr;
}

void dbg_free(void * dat)
{
  location_definition * ret = (location_definition*)pop(dat);
  if(ret==NULL)
  {
    printf("not found\n");
    //exit(1);
  }
  else
  {
    location_definition * loc = (location_definition*)malloc(sizeof(location_definition));
    INFO("free",ret->size,ret->ram_addr,loc);
    free(loc);
    free(ret);
  }
  free(dat);
}

