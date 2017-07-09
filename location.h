#ifndef LOCATION_H
#define LOCATION_H

#include <stdio.h>

#define BT_BUF_SIZE 20

#define MAX_LEN 100

struct location_definition
{
  char array[BT_BUF_SIZE][MAX_LEN];
  size_t bt_size;
  size_t ram_addr;
  size_t size;
  void copy(location_definition cp)
  {
    bt_size = cp.bt_size;
    for(int i=0;i<BT_BUF_SIZE&&i<bt_size;i++)
    {
      int j=0;
      while(j<MAX_LEN)
      {
        array[i][j] = cp.array[i][j];
        if(cp.array[i][j]=='\0')break;
        j++;
      }
    }
    ram_addr = cp.ram_addr;
    size = cp.size;
  }
  void print(std::string msg)
  {
    printf("%s\n",msg.c_str());
    for(int i=0;i<BT_BUF_SIZE&&i<bt_size;i++)
    {
      printf("%s\n",array[i]);
    }
  }
};

struct mem_info
{
  size_t total_allocated;
  size_t num_invoked;
  size_t total_size;
};

#endif

