#ifndef LOCATION_H
#define LOCATION_H

#include <stdio.h>

#define BT_BUF_SIZE 32

#define MAX_LEN 1024

struct location_definition
{
  char arrays[BT_BUF_SIZE][MAX_LEN];
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
        arrays[i][j] = cp.arrays[i][j];
        if(cp.arrays[i][j]=='\0')break;
        j++;
      }
    }
	
    ram_addr = cp.ram_addr;
    size = cp.size;
  }
  void print(std::string msg)
  {
	std::cout << msg.c_str() << std::endl;
    for(int i=0;i<BT_BUF_SIZE&&i<bt_size;i++)
    {
	  std::cout << arrays[i] << std::endl;
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

