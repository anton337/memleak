#include <iostream>
#include <map>
#include <set>

#include <stdio.h>
#include <signal.h>
#include <stdlib.h>

#include "location.h"

struct location_compare {
	bool operator()
		(const location_definition& lhs
		, const location_definition& rhs
		) const
	{
		return lhs.ram_addr < rhs.ram_addr;
	}
};

struct info_compare {
	bool operator()
		(const std::pair<location_definition, mem_info>& lhs
		, const std::pair<location_definition, mem_info>& rhs
		) const
	{
		if (lhs.first.bt_size<rhs.first.bt_size)return true;
		for (int i = 0; i<BT_BUF_SIZE&&i<lhs.first.bt_size&&i<rhs.first.bt_size; i++)
		{
			int j = 0;
			while (lhs.first.arrays[i][j] != '\0'&&rhs.first.arrays[i][j] != '\0'&&j<MAX_LEN)
			{
				if ((int)lhs.first.arrays[i][j] < (int)rhs.first.arrays[i][j])
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

template<typename T, typename compare>
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
		if (root == NULL&&tail == NULL)
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
	bool find(T& a, bool stop = true)
	{
		linked_node<T>* tmp = root;
		int iter = 0;
		while (tmp != NULL)
		{
			if (comp(a, *tmp->elem) == false && comp(*tmp->elem, a) == false)
			{
				// found it
				if (stop)
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
		while (tmp != NULL)
		{
			if (comp(a, *tmp->elem) == false && comp(*tmp->elem, a) == false)
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
		while (tmp != NULL)
		{
			nxt = tmp->next;
			remove(tmp);
			tmp = nxt;
		}
	}
	void remove(linked_node<T>* node)
	{
		if (node->prev != NULL)
		{
			node->prev->next = node->next;
		}
		if (node->next != NULL)
		{
			node->next->prev = node->prev;
		}
		if (root == node)
		{
			root = node->next;
		}
		if (tail == node)
		{
			tail = node->prev;
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
		while (tmp != NULL)
		{
			if (comp(a, *tmp->elem) == false && comp(*tmp->elem, a) == false)
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

linked_list<location_definition, location_compare> Q;
linked_list<std::pair<location_definition, mem_info>, info_compare> M;

// need a tonn of error checking 
void push(location_definition * loc)
{
	
	location_definition * L = (location_definition *)loc;
	location_definition * cpy_L = (location_definition *)malloc(sizeof(location_definition));
	
	cpy_L->copy(*L);
	
	Q.insert_last(cpy_L);
	mem_info _m;
	std::pair<location_definition, mem_info> P(*L, _m);
	if (M.find(P))
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
		std::pair<location_definition, mem_info> * ptr = (std::pair<location_definition, mem_info>*)malloc(sizeof(std::pair<location_definition, mem_info>));
		ptr->first.copy(*L);
		ptr->second = m_info;
		M.insert_last(ptr);
	}
	free(loc);
	
}

// need a tonn of error checking 
location_definition * pop(void * loc)
{
	location_definition * L = (location_definition *)malloc(sizeof(location_definition));
	L->ram_addr = (size_t)loc;
	if (Q.find(*L))
	{
		location_definition * R = Q.get(*L);
		location_definition * T = (location_definition*)malloc(sizeof(location_definition));
		T->copy(*R);
		mem_info _m;
		std::pair<location_definition, mem_info> P(*T, _m);
		if (M.find(P))
		{
			M.get(P)->second.total_size -= T->size;
		}
		else
		{
			std::cout << "deallocating non-allocated memory\n";
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
	std::cout << "map size:" << M.get_size() << std::endl;
	typedef linked_node<std::pair<location_definition, mem_info> > linked_node;
	linked_node * tmp = M.root;
	while (tmp != NULL)
	{
		std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << std::endl;
		tmp->elem->first.print("");
		std::cout << tmp->elem->second.total_size << "\t" << tmp->elem->second.num_invoked << "\t" << tmp->elem->second.total_allocated << std::endl;
		tmp = tmp->next;
	}
	M.clear();
}


#include <iostream>

#include "dbg_defs.h"

#include "location.h"

#include <process.h>
#include <iostream>
#include <Windows.h>
#include "dbghelp.h"

#define TRACE_MAX_STACK_FRAMES 32
#define TRACE_MAX_FUNCTION_NAME_LENGTH 1024

using namespace std;

int printStackTrace(location_definition * loc)
{
	
	void *stack[TRACE_MAX_STACK_FRAMES];
	HANDLE process = GetCurrentProcess();
	SymInitialize(process, NULL, TRUE);
	WORD numberOfFrames = CaptureStackBackTrace(0, TRACE_MAX_STACK_FRAMES, stack, NULL);
	SYMBOL_INFO *symbol = (SYMBOL_INFO *)malloc(sizeof(SYMBOL_INFO) + (TRACE_MAX_FUNCTION_NAME_LENGTH - 1) * sizeof(TCHAR));
	symbol->MaxNameLen = TRACE_MAX_FUNCTION_NAME_LENGTH;
	symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
	DWORD displacement;
	IMAGEHLP_LINE64 *line = (IMAGEHLP_LINE64 *)malloc(sizeof(IMAGEHLP_LINE64));
	line->SizeOfStruct = sizeof(IMAGEHLP_LINE64);
	loc->bt_size = numberOfFrames;
	for (int i = 0; i < BT_BUF_SIZE; i++)
	{
		loc->arrays[i][0] = '\0';
	}
	for (int i = 0; i < numberOfFrames && i < BT_BUF_SIZE; i++)
	{
		DWORD64 address = (DWORD64)(stack[i]);
		SymFromAddr(process, address, NULL, symbol);
		if (SymGetLineFromAddr64(process, address, &displacement, line))
		{
			sprintf_s(loc->arrays[i],MAX_LEN,"\tat %s in %s: line: %lu: address: 0x%0X\n", symbol->Name, line->FileName, line->LineNumber, symbol->Address);
		}
	}
	
	return 0;
}

void INFO(const char * msg,size_t size,size_t addr,void * dat)
{
	
	std::cout << msg << "\t" << size << "\t" << addr << std::endl;;
  
  location_definition* loc_dat = (location_definition*)dat;
  loc_dat->ram_addr = (size_t)addr;
  loc_dat->size = size;
  printStackTrace(loc_dat);
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
    std::cout << "not found\n";
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

