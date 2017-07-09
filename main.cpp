#include <iostream>

#include "allocate.h"

#include "dbg_map.h"


void super_special_alloc(float**arr,int k)
{
  arr[k] = new float[100];
}

void super_special_dealloc(float**arr,int k)
{
  delete [] arr[k];
}

int main()
{
  float** arr = new float*[10];
  std::cout << "hello world" << std::endl;
  float * dat = new float[5];
  float * k = new float[10];
  double * T = new double(50.5f);
  double * D = new double(55.5f);
  for(int i=0;i<10;i++)super_special_alloc(arr,i);
  delete D;
  delete [] dat;
  delete T;
  delete [] k;
  for(int i=0;i<10;i++)super_special_dealloc(arr,i);
  delete [] arr;
  print_summary();
  return 0;
}

