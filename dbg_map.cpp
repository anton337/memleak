#include <iostream>
#include <map>
#include <set>

#include <stdio.h>
#include <execinfo.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include "dbg_map.h"

#include "location.h"

struct location_compare {
  bool operator() 
    ( const location_definition& lhs
    , const location_definition& rhs
    ) const 
  {
    return lhs.ram_addr < rhs.ram_addr;
  }
};

struct info_compare {
  bool operator() 
    ( const std::pair<location_definition,mem_info>& lhs
    , const std::pair<location_definition,mem_info>& rhs
    ) const 
  {
    if(lhs.first.bt_size<rhs.first.bt_size)return true;
    for(int i=0;i<BT_BUF_SIZE&&i<lhs.first.bt_size&&i<rhs.first.bt_size;i++)
    {
      int j=0;
      while(lhs.first.array[i][j]!='\0'&&rhs.first.array[i][j]!='\0'&&j<MAX_LEN)
      {
        if((int)lhs.first.array[i][j] < (int)rhs.first.array[i][j])
        {
          return true;
        }
        j++;
      }
    }
    return false;
  }
};

template<typename T>
struct linked_node
{
  T * elem;
  linked_node<T> * next;
  linked_node<T> * prev;
};

template<typename T,typename compare>
struct linked_list
{
  size_t size;
  linked_node<T>* root;
  linked_node<T>* tail;
  compare comp;
  linked_list()
  {
    size = 0;
    root = NULL;
    tail = NULL;
  }
  void insert_last(T * a)
  {
    linked_node<T> * node = (linked_node<T>*)malloc(sizeof(linked_node<T>));
    node->elem = a;
    node->next = NULL;
    node->prev = NULL;
    if(root==NULL&&tail==NULL)
    {
      root = node;
      tail = node;
    }
    else
    {
      tail->next = node;
      node->prev = tail;
      tail = node;
    }
    size++;
  }
  bool find(T& a,bool stop=true)
  {
    linked_node<T>* tmp = root;
    int iter = 0;
    while(tmp!=NULL)
    {
      if(comp(a,*tmp->elem)==false&&comp(*tmp->elem,a)==false)
      {
        // found it
        if(stop)
          return true;
      }
      tmp = tmp->next;
      iter++;
    }
    return false;
  }
  T * get(T& a)
  {
    linked_node<T>* tmp = root;
    while(tmp!=NULL)
    {
      if(comp(a,*tmp->elem)==false&&comp(*tmp->elem,a)==false)
      {
        // found it
        return tmp->elem;
      }
      tmp = tmp->next;
    }
    return NULL;
  }
  void clear()
  {
    linked_node<T>* tmp = root;
    linked_node<T>* nxt = NULL;
    while(tmp!=NULL)
    {
      nxt = tmp->next;
      remove(tmp);
      tmp = nxt;
    }
  }
  void remove(linked_node<T>* node)
  {
    if(node->prev!=NULL)
    {
      node->prev->next = node->next;
    }
    if(node->next!=NULL)
    {
      node->next->prev = node->prev;
    }
    if(root==node)
    {
      root=node->next;
    }
    if(tail==node)
    {
      tail=node->prev;
    }
    node->prev = NULL;
    node->next = NULL;
    free(node->elem);
    node->elem = NULL;
    free(node);
    size--;
  }
  void find_and_remove(T& a)
  {
    linked_node<T>* tmp = root;
    while(tmp!=NULL)
    {
      if(comp(a,*tmp->elem)==false&&comp(*tmp->elem,a)==false)
      {
        // found it
        remove(tmp);
        break;
      }
      tmp = tmp->next;
    }
  }
  size_t get_size()
  {
    return size;
  }
};

linked_list<location_definition,location_compare> Q;
linked_list<std::pair<location_definition,mem_info>,info_compare> M;

// need a tonn of error checking 
void push(void * loc)
{
  location_definition * L = (location_definition *)loc;
  location_definition * cpy_L = (location_definition *)malloc(sizeof(location_definition));
  cpy_L->copy(*L);
  Q.insert_last(cpy_L);
  mem_info _m;
  std::pair<location_definition,mem_info> P(*L,_m);
  if(M.find(P))
  {
    M.get(P)->second.total_size += L->size;
    M.get(P)->second.total_allocated += L->size;
    M.get(P)->second.num_invoked++;
  }
  else
  {
    mem_info m_info;
    m_info.num_invoked = 1;
    m_info.total_size = L->size;
    m_info.total_allocated = L->size;
    std::pair<location_definition,mem_info> * ptr = (std::pair<location_definition,mem_info>*)malloc(sizeof(std::pair<location_definition,mem_info>));
    ptr->first.copy(*L);
    ptr->second = m_info;
    M.insert_last(ptr);
  }
  free(loc);
}

// need a tonn of error checking 
void * pop(void * loc)
{
  location_definition * L = (location_definition *)malloc(sizeof(location_definition));
  L->ram_addr = (size_t)loc;
  if(Q.find(*L))
  {
    location_definition * R = Q.get(*L);
    location_definition * T = (location_definition*)malloc(sizeof(location_definition));
    T->copy(*R);
    mem_info _m;
    std::pair<location_definition,mem_info> P(*T,_m);
    if(M.find(P))
    {
      M.get(P)->second.total_size -= T->size;
    }
    else
    {
      printf("deallocating non-allocated memory\n");
      exit(1);
    }
    Q.find_and_remove(*L);
    free(L);
    return T;
  }
  free(L);
  return NULL;
}

void print_summary()
{
  printf("map size:%zu\n",M.get_size());
  typedef linked_node<std::pair<location_definition,mem_info> > linked_node;
  linked_node * tmp = M.root;
  while(tmp!=NULL)
  {
    tmp->elem->first.print("");
    printf("%zu\t%zu\t%zu\n",tmp->elem->second.total_size,tmp->elem->second.num_invoked,tmp->elem->second.total_allocated);
    tmp = tmp->next;
  }
  M.clear();
}


