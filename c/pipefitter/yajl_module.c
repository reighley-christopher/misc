#include <yajl/yajl_parse.h>
#include <stdlib.h>
#include "bytebuffer.h"

#include <stdio.h>

struct yajl_module
  {
  yajl_handle handle;
  long int character_count;
  int object_depth;
  int list_type;
  int end_marker;
  };

int yajl_module_object_start_callback(void *ctx)
  {
  struct yajl_module *m = (struct yajl_module *)ctx;
  m->object_depth = m->object_depth + 1; 
  printf("%d down\n", yajl_get_bytes_consumed(m->handle)); 
  return 1;
  }

int push_list_callback(void *ctx, int type)
  {
  struct yajl_module *m = (struct yajl_module *)ctx;
  if(m->list_type == 0) m->list_type = type;
  if(m->list_type == type)
    {
    m->object_depth = m->object_depth + 1; 
    printf("%d down\n", yajl_get_bytes_consumed(m->handle)); 
    } 
  return 1;
  } 

int pop_list_callback(void *ctx, int type)
  {
  struct yajl_module *m = (struct yajl_module *)ctx;
  if( type != m->list_type) return 1;
  m->object_depth = m->object_depth - 1;
  if(m->object_depth == 0) 
    {
    m->list_type = 0;
    return 0;
    } else {
    return 1;
    }  
  }

int scalar_callback(void *ctx)
  {
  struct yajl_module *m = (struct yajl_module *)ctx;
  printf("scalar callback\n");
  if(m->list_type == 0)
    {
    return 0;
    } else {
    return 1;
    }
  }

int boolean_callback(void *ctx, int b) { return scalar_callback(ctx); } 
int integer_callback(void *ctx, long long i ) { return scalar_callback(ctx); }
int double_callback(void *ctx, double d ) { return scalar_callback(ctx); }
int string_callback(void *ctx, const unsigned char *c, size_t l) { return scalar_callback(ctx); }

int start_map_callback(void *ctx)
  {
  return push_list_callback(ctx, 1); 
  }

int end_map_callback(void *ctx)
  {
  return pop_list_callback(ctx, 1); 
  }

int start_array_callback(void *ctx)
  {
  return push_list_callback(ctx, 2); 
  }

int end_array_callback(void *ctx)
  {
  return pop_list_callback(ctx, 2); 
  }

int yajl_module_object_end_callback(void *ctx) 
  {
  struct yajl_module *m = (struct yajl_module *)ctx;
  int consumed = yajl_get_bytes_consumed(m->handle); 
  m->object_depth = m->object_depth - 1;
  printf("%d up\n", consumed );
  if( m->object_depth == 0) 
    {
    return 0;
    } else {
    return 1;
    }
  }

void *yajl_module_alloc()
  {
  struct yajl_module *m = malloc(sizeof(struct yajl_module)); 
  m->handle = 0;  
  }  

yajl_callbacks callbacks;

void yajl_module_init(void *m)
  {
  struct yajl_module *h = (struct yajl_module *)m;
  if(h->handle != 0) yajl_free(h->handle);
  callbacks.yajl_null = scalar_callback ; 
  callbacks.yajl_boolean = boolean_callback;
  callbacks.yajl_integer = integer_callback;
  callbacks.yajl_double = double_callback;
  callbacks.yajl_number = NULL;
  callbacks.yajl_string = string_callback;
  callbacks.yajl_start_map = start_map_callback;
  callbacks.yajl_map_key = NULL; 
  callbacks.yajl_end_map = end_map_callback;
  callbacks.yajl_start_array = start_array_callback;
  callbacks.yajl_end_array = end_array_callback;
  h->handle = yajl_alloc(&callbacks, NULL, m);
  }

void yajl_module_free(void *m)
  {
  struct yajl_module *h = (struct yajl_module *)m;
  yajl_free(h->handle);
  free(m); 
  }

int yajl_module_complete(void *handle, unsigned char *data, int length)
  {
  struct yajl_module *h = (struct yajl_module *)handle;
  yajl_status status;
  int consumed; 
  int c;
  status = yajl_parse(h->handle, data, length );
  consumed = yajl_get_bytes_consumed(h->handle); /*will include any trailing whitespace and the first byte of the next record (because it might be a whitespace who knows?*/
  if(status == yajl_status_error) {
    yajl_free(h->handle);
    h->handle = yajl_alloc(&callbacks, NULL, handle); 
    consumed = consumed; //? - 1;
    }
  printf(">");
  for(c = 0; c < length; c++) printf("%c", data[c]);
  printf("<\n");
  printf(" %s %d of %d\n", yajl_status_to_string(status), consumed, length); 
  if(status == yajl_status_client_canceled ) return consumed; else return -1; //TODO does consumed include the bytes that caused the cancelation? 
                                                                              //if so this is a bug
  }

completer_module yajl_completer_module;
completer_module *get_yajl_completer_module()
  {
  yajl_completer_module.func = yajl_module_complete;
  yajl_completer_module.init = yajl_module_init;
  yajl_completer_module.alloc = yajl_module_alloc;
  yajl_completer_module.free = yajl_module_free;
  return &yajl_completer_module; 
  }

