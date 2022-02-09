#include "bytebuffer.h"

int simple_func(void *d, unsigned char *data, int length)
  {
  int i;
  for(i = 0; i < length; i ++)
    {
    if(data[i] == ';') return i+1; //TODO it looks like a 0 in this place is an error case completer should never return 0 bytebuffer needs to deal 
    }
  return -1;
  }

void *simple_alloc()
  {
  return 0;
  }

void simple_init(void *m)
  {
  }

void simple_free(void *m)
  {
  }

completer_module simple_completer_module;
completer_module *get_simple_completer_module()
  {
  simple_completer_module.func = simple_func;
  simple_completer_module.alloc = simple_alloc;
  simple_completer_module.init = simple_init;
  simple_completer_module.free = simple_free;
  return &simple_completer_module;
  }


