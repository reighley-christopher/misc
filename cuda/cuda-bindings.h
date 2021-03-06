void alloc_one_mem(int cell, int height, int width);
void init_memory_cells(int how_many);
void load_mem(int cell, float *mem);
void unload_mem(int cell, float **ptr, int *rows, int *columns);
void unload_row(int cell, int row_index, float **ptr, int *columns);
void matrix_multiply(int cellA, int cellB, int cellC);
void rectify(int cell);
void partition_function(int inp, int outp); 
void softmax(int activations, int partition_function);
void cross_entropy_loss(int model, int actual, int loss);
void init_weights_from_sampler(int cell_id, float (*sampler)());
