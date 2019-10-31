#include <libguile.h>
#include <stdio.h>
#include <math.h>
#include "cuda-bindings.h"
#include "simple-draw.h"

static SCM _init_device_mem(SCM cell_count)
  {
  printf("init_device_mem %d\n", scm_to_int(cell_count));
  init_memory_cells(scm_to_int(cell_count)); 
  return SCM_UNDEFINED;
  } 

static SCM _import_labels(SCM filename, SCM max_length, SCM cell)
  {
  float *local_mem;
  char header[8];
  int magic_number;
  int item_count;
  char *filen;
  char *sploop;
  char n, c;
  int i, j;
  FILE *fp;
  filen = scm_to_stringn(filename, 0, "UTF-8", 0);
  fp = fopen(filen, "r");
  fread(header, 1, 8, fp);
  sploop = &magic_number;
  for(i = 0; i < 4; i++){sploop[3-i] = header[i];}
  sploop = &item_count;
  for(i =4; i < 8; i++){sploop[7-i] = header[i];}
  printf("%s %x %d\n", filen, magic_number, item_count);
  free(filen);
  /*ha don't use the file size that's too big to fit*/
  item_count = scm_to_int(max_length);
  local_mem = malloc(sizeof(float)*10*item_count); //TODO hardcoded MNIST size, sloppy
  for(i = 0; i < item_count; i++)
    {
    fread(&c, 1, 1, fp);
    for(j = 0; j < 10; j++)
      {
      if(j == c) {local_mem[i*10+j] = 1.0;} else {local_mem[i*10+j] = 0.0;} 
      }
    }
  load_mem(scm_to_int(cell), local_mem);
  free(local_mem);
  fclose(fp);
  return SCM_UNDEFINED;
  }

static SCM _import_images(SCM filename, SCM max_length, SCM cell)
  {
  float *local_mem;
  unsigned char *file_data;
  char header[16];
  int magic_number;
  int item_count;
  int rows, columns;
  char *filen;
  char *sploop;
  char n;
  int i, j;
  FILE *fp;
  filen = scm_to_stringn(filename, 0, "UTF-8", 0);
  fp = fopen(filen, "r");
  fread(header, 1, 16, fp);
  sploop = &magic_number;
  for(i = 0; i < 4; i++){sploop[3-i] = header[i];}
  sploop = &item_count;
  for(i = 4; i < 8; i++){sploop[7-i] = header[i];}
  sploop = &rows;
  for(i = 8; i < 12; i++){sploop[11-i] = header[i];}
  sploop = &columns;
  for(i = 12; i < 16; i++){sploop[15-i] = header[i];}
  printf("%s %x %d %d %d\n", filen, magic_number, item_count, rows, columns);
  free(filen);
  /*ha don't use the file size that's too big to fit*/
  item_count = scm_to_int(max_length);
  local_mem = malloc(sizeof(float)*item_count*rows*columns);
  if(local_mem == 0) {printf("local memory error\n"); return SCM_UNDEFINED;}
  file_data = malloc(item_count*rows*columns);
  if(file_data == 0) {printf("local memory error\n"); return SCM_UNDEFINED;}
  fread(file_data, 1, item_count*rows*columns, fp);
  for(i = 0; i < item_count * rows * columns; i++)
    {
    local_mem[i] = file_data[i]/255.0;
    }
  load_mem(scm_to_int(cell), local_mem);
  free(local_mem);
  free(file_data);
  fclose(fp);
  return SCM_UNDEFINED;
  }

float small_rands()
  {
  return (((float)rand()/(RAND_MAX))-0.5)/16;
  }

static SCM _init_weights(SCM cell_id)
  {
  init_weights_from_sampler
    (
    scm_to_int(cell_id),
    small_rands
    );
  return SCM_UNDEFINED;
  }

static SCM _dim(SCM cell_id, SCM height, SCM width)
  {
  printf("dim %d %d %d\n", scm_to_int(cell_id), scm_to_int(height), scm_to_int(width));
  alloc_one_mem
    (
    scm_to_int(cell_id),
    scm_to_int(height),
    scm_to_int(width)
    );
  return SCM_UNDEFINED;
  }

static SCM
_load_values(SCM cell_id, SCM vector)
  {
  int i;
  scm_t_array_handle h;
  const float *array;
  size_t lenp;
  scm_array_get_handle(vector, &h); 
  lenp = scm_c_array_length(vector);
  printf("load_values %d ; %d elements:\n", scm_to_int(cell_id), lenp);
  array = scm_array_handle_f32_elements(&h);
  printf("%x\n", array);
  load_mem(scm_to_int(cell_id), array);
  for(i = 0; i < lenp; i++) printf("%f ", array[i]);
  printf("\n");
  scm_array_handle_release(&h);
  return SCM_UNDEFINED;
  }

static SCM
_m_star(SCM a, SCM b, SCM c)
  {
  matrix_multiply
    (
    scm_to_int(a),
    scm_to_int(b),
    scm_to_int(c)
    );
  return SCM_UNDEFINED;
  }

static SCM
_unload_values(SCM index)
  {
  float *p;
  int r, c;
  int i, j;
  unload_mem(scm_to_int(index), &p, &r, &c);
  printf("unload_mem %x %d %d\n", p, r, c); 
  for(i = 0; i < r; i++)
    {
    for(j = 0; j < c; j++)
      {
      printf("%f ", p[i*c + j]);
      }
    printf("\n");
    }
  free(p);
  return SCM_UNDEFINED;
  }

/*need some functions to inspect this stuff.*/
/*get a single value by row, column index*/

/*get a row of values and display the index of the highest value*/
static SCM
_check_logit(SCM cell_id, SCM row_index)
  {
  float *p;
  int c, i, ret;
  float max;
  unload_row(scm_to_int(cell_id), scm_to_int(row_index), &p, &c);
  ret = 0;
  max = p[0];
  for(i = 1; i < c; i++) if(p[i] > max) {ret = i; max = p[i];}
  free(p);
  printf("%d\n", ret);
  return SCM_UNDEFINED;
  }


/*get a row of values and display as a rectangle of numbers*/
static SCM
_display_row(SCM cell_id, SCM row_index, SCM display_height, SCM display_width)
  {
  float *p;
  int i,j;
  int c;
  int dh = scm_to_int(display_height);
  int dw = scm_to_int(display_width);
  unload_row(scm_to_int(cell_id), scm_to_int(row_index), &p, &c);
  if(c != dh*dw) {printf("size mismatch expected %d = %d * %d\n", c, dh, dw);}
  else
    {
    for(i = 0; i < dh; i++)
      {
      for(j = 0; j < dw; j++)
        {
        printf("%5.2f", p[i*dw+j]);
        }
      printf("\n");
      }
    }
  free(p);
  return SCM_UNDEFINED;
  }

/*get a row of values and display as a rectangle of ascii art*/
static SCM
_display_row_gscale(SCM cell_id, SCM row_index, SCM display_height, SCM display_width)
  {
  float *p;
  int c;
  int dh = scm_to_int(display_height);
  int dw = scm_to_int(display_width);
  unload_row(scm_to_int(cell_id), scm_to_int(row_index), &p, &c);
  if(c != dh*dw) {printf("size mismatch expected %d = %d * %d\n", c, dh, dw);}
  else
    {
    greymap(dh, dw, 0.0, 1.0, p);  
    }
  free(p);
  return SCM_UNDEFINED;
  }

/*end display tools*/

static SCM
_rectify(SCM cell_id)
  {
  rectify(scm_to_int(cell_id)); 
  return SCM_UNDEFINED;
  }

static SCM
_partition_function(SCM input_cell, SCM output_cell)
  {
  partition_function(scm_to_int(input_cell), scm_to_int(output_cell)); 
  return SCM_UNDEFINED;
  }

static SCM
_softmax(SCM activations, SCM partition_function)
  {
  softmax(scm_to_int(activations), scm_to_int(partition_function));
  return SCM_UNDEFINED;
  }

static SCM
_cross_entropy_loss(SCM model, SCM actual, SCM loss)
  {
  cross_entropy_loss
    (
    scm_to_int(model),
    scm_to_int(actual),
    scm_to_int(loss)
    );
  return SCM_UNDEFINED;
  }

static void *register_functions(void *data)
  {
  scm_c_define_gsubr("init_device_mem", 1, 0, 0, _init_device_mem);
  scm_c_define_gsubr("dim", 3, 0, 0, _dim);
  scm_c_define_gsubr("load_values", 2, 0, 0, _load_values);
  scm_c_define_gsubr("m*", 3, 0, 0, _m_star);
  scm_c_define_gsubr("rect", 1, 0, 0, _rectify); 
  scm_c_define_gsubr("unload_values", 1, 0, 0, _unload_values);
  scm_c_define_gsubr("import_labels", 3, 0, 0, _import_labels);
  scm_c_define_gsubr("import_images", 3, 0, 0, _import_images);
  scm_c_define_gsubr("init_weights", 1, 0, 0, _init_weights);
  scm_c_define_gsubr("display_row", 4, 0, 0, _display_row_gscale);
  scm_c_define_gsubr("check_logit", 2, 0, 0, _check_logit);
  scm_c_define_gsubr("partf", 2, 0, 0, _partition_function);
  scm_c_define_gsubr("softm", 2, 0, 0, _softmax);
  scm_c_define_gsubr("crosse", 3, 0, 0, _cross_entropy_loss);
  scm_c_define_gsubr("softcross_bp", 3, 0, 0, _);
  return NULL;
  }

int main(int argc, char *argv[])
  {
  scm_with_guile( &register_functions, NULL);
  scm_shell(argc, argv);
  }
