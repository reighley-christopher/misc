#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <cuda.h>
#include <cuda_runtime.h> 

typedef struct 
  {
  void *device_pointer;
  int rows;
  int columns;
  } memory_cell;

memory_cell *memory_cells = (memory_cell *)0;

extern "C" {
void init_memory_cells(int how_many)
  {
  int i;
  memory_cells = (memory_cell *)malloc(sizeof(memory_cell) * how_many);
  for(i = 0; i < how_many; i++) 
    {
    memory_cells[i].device_pointer = (void *)0;
    }
  }
}

typedef struct
  {
  int input_units;
  int hidden_units;
  int output_units;
  } network;

/* first figure out the capacity of the device, we want to use as much memory as possible, without necessarily touching the bus at all */

void handleCudaErrors(cudaError_t err)
  {
  if (err != cudaSuccess)
    {
    printf("CUDA ERROR: %s\n", cudaGetErrorString(err));
    }
  }

void get_device_statistics( cudaDeviceProp *deviceProp )
  {
  cudaSetDevice(0);
  cudaGetDeviceProperties(deviceProp, 0);
  };

/* then we will need a block of memory for data, and one for weights */
typedef struct
  {
  void *layer1;
  void *layer2;
  void *data;
  } mem_map;

extern "C" {
void alloc_one_mem(int cell, int height, int width)
  {
  handleCudaErrors ( cudaMalloc
    (
    (void **) &(memory_cells[cell].device_pointer), 
    height * width * sizeof(float) 
    ));
  memory_cells[cell].columns = width;
  memory_cells[cell].rows = height;
  printf("%x\n", memory_cells[cell].device_pointer);
  }
}

void alloc_all_the_mems
  ( 
  mem_map *mmap, 
  network network,
  int num_records, 
  cudaDeviceProp deviceProp 
  )
  {
  /*memory is allocated on 256 byte boundaries
    TODO: how consistent is this*/
  /*input * hidden for the first layer of weights*/
  
  handleCudaErrors ( cudaMalloc
    (
    (void **) &(mmap->layer1), 
    network.input_units * network.hidden_units * sizeof(float) 
    ));
  /*hidden * output for the second layer of weights*/
  handleCudaErrors ( cudaMalloc
    (
    (void **) &(mmap->layer2), 
    network.hidden_units * network.input_units * sizeof(float) 
    ));
  handleCudaErrors ( cudaMalloc
    (
    (void **) &(mmap->data), 
    num_records * (network.input_units + network.output_units) 
      * sizeof(float) 
    )); 
  }

extern "C" {
void load_mem(int cell, float *mem)
  {
  handleCudaErrors ( cudaMemcpy 
    ( 
    memory_cells[cell].device_pointer, mem,
    memory_cells[cell].rows*memory_cells[cell].columns*sizeof(float), 
    cudaMemcpyHostToDevice 
    ));
  }

void unload_mem(int cell, float **ptr, int *rows, int *columns)
  {
  float *ret = (float *)malloc(sizeof(float)*memory_cells[cell].columns*memory_cells[cell].rows);
  handleCudaErrors ( cudaMemcpy
    (
    ret,
    memory_cells[cell].device_pointer,
    memory_cells[cell].rows*memory_cells[cell].columns*sizeof(float), 
    cudaMemcpyDeviceToHost 
    ));
  *ptr = ret;
  *rows = memory_cells[cell].rows;
  *columns = memory_cells[cell].columns;
  }
}

extern "C" {
void unload_row(int cell, int row_index, float **ptr, int *columns)
  {
  float *ret = (float *)malloc(sizeof(float)*memory_cells[cell].columns);
  handleCudaErrors ( cudaMemcpy
    (
    ret,
    (float *)memory_cells[cell].device_pointer + 
      (memory_cells[cell].columns * row_index),
    memory_cells[cell].columns*sizeof(float), 
    cudaMemcpyDeviceToHost 
    ));
  *ptr = ret;
  *columns = memory_cells[cell].columns;
  }
}


extern "C" {
void init_weights_from_sampler(int cell_id, float (*sampler)())
  {
  int height, width, i;
  float *host_memory;
  height = memory_cells[cell_id].rows;
  width = memory_cells[cell_id].columns;
  host_memory = (float *)malloc(sizeof(float)*height*width);
  for(i = 0; i < height*width; i++) {host_memory[i] = sampler();};
  load_mem(cell_id, host_memory);
  free(host_memory); 
  }
}

/*we need to initialize the weight matrix the first time*/
void init_weights_without_file(network net, mem_map mmap)
  {
  float layer1[net.input_units*net.hidden_units];
  float layer2[net.hidden_units*net.output_units];
  int i;
  for(i = 0; i < net.input_units*net.hidden_units; i++)
    {
    layer1[i] = (float)rand()/RAND_MAX;
    }
  for(i = 0; i < net.hidden_units*net.output_units; i++)
    {
    layer2[i] = (float)rand()/RAND_MAX;
    }
  handleCudaErrors ( cudaMemcpy 
    ( 
    mmap.layer1, &layer1, 
    net.input_units*net.hidden_units, cudaMemcpyHostToDevice 
    ));
  handleCudaErrors ( cudaMemcpy 
    ( 
    mmap.layer2, &layer2, 
    net.hidden_units*net.output_units, cudaMemcpyHostToDevice 
    ));
 
  }

/*we need to save the file*/
void save_weights(network net, mem_map mmap)
  {
  float layer1[net.input_units*net.hidden_units];
  float layer2[net.hidden_units*net.output_units];
  FILE *fp;
  handleCudaErrors ( cudaMemcpy
    (
    &layer1,
    mmap.layer1,
    net.output_units*net.hidden_units, cudaMemcpyDeviceToHost 
    ));
  handleCudaErrors ( cudaMemcpy
    (
    &layer2,
    mmap.layer2,
    net.hidden_units*net.output_units, cudaMemcpyDeviceToHost 
    ));
  fp = fopen("save.file", "w");
  fwrite(layer1, sizeof(float), net.input_units*net.hidden_units, fp);
  fwrite(layer2, sizeof(float), net.hidden_units*net.output_units, fp);
  fclose(fp);
  }

/*we need to initialize the weights from a file*/
void init_weights_with_file(FILE *fp, network net, mem_map mmap)
  {
  float layer1[net.input_units*net.hidden_units];
  float layer2[net.hidden_units*net.output_units];
  handleCudaErrors ( cudaMemcpy 
    ( 
    mmap.layer1, &layer1, 
    net.input_units*net.hidden_units, cudaMemcpyHostToDevice 
    ));
  handleCudaErrors ( cudaMemcpy 
    ( 
    mmap.layer2, &layer2, 
    net.hidden_units*net.output_units, cudaMemcpyHostToDevice 
    ));
  fread(layer1, sizeof(float), net.input_units*net.hidden_units, fp);
  fread(layer2, sizeof(float), net.hidden_units*net.output_units, fp);
  fclose(fp);
  }

/*we need to load a block of data (can we do this while the kernel is working?*/
void load_data(void *fp, mem_map *mmap)
  {
  }

__global__ void matrix_multiply_kernel(int dim1, float *matA, int dim2, float *matB, int dim3, float *matOut)
  {
  /*matOut[x,y] = sum over dim2 matA[x, i] * matB[i, y]*/
  /* the dataset will be row major so the other matrices should be too

     (0,0) (0,1) (0,2) * (0,0) =  
                         (1,0) 
                         (2,0) */
  int myX = blockIdx.x*blockDim.x + threadIdx.x;
  int myY = blockIdx.y*blockDim.y + threadIdx.y;
  float acc = 0.0;
  int i;
  if(myX < dim3 && myY < dim1) for(i = 0; i < dim2; i++)
    { 
    acc += matA[myY*dim2+i]*matB[i*dim3+myX];
    }
    matOut[myY*dim3+myX] = acc;
  }

__global__ void rectify_kernel(float *array, int rows, int columns)
  {
  int myX = blockIdx.x*blockDim.x + threadIdx.x;
  int myY = blockIdx.y*blockDim.y + threadIdx.y;
  if(myX < columns && myY < rows && array[myY*columns + myX] <= 0.0)
    array[myY*rows + myX] = 0.0;
  }

extern "C" {
void rectify(int cell)
  {
  int h, w;
  float *p;
  h = memory_cells[cell].rows;
  w = memory_cells[cell].columns;
  p = (float *)memory_cells[cell].device_pointer;
  int block_width, block_height, grid_width, grid_height;
  block_width=32;
  block_height=32;
  grid_width=ceil(h/block_width);
  grid_height=ceil(w/block_height);
  /*end block size thoughts*/
  dim3 dimBlock(block_width, block_height, 1);
  dim3 dimGrid(grid_width, grid_height ,1);
  rectify_kernel<<<dimBlock, dimGrid>>>(p, h, w);  
  }
}

__global__ void bp_rectify(float *weights, float *acts, float *prior_losses, int inputs, int outputs, int samples, float *losses)
  {
  /*
  weights is the weights matrix, its dimension is (inputs, outputs)
  acts is the activations at the output layer, its dimension is (samples, outputs)
  prior_losses is the losses measured at the output layer, it's dimension is (samples, outputs)
  loss is the loss measured at the input layer, its dimension is (samples, inputs)
  */
  int myX = blockIdx.x*blockDim.x + threadIdx.x;
  int myY = blockIdx.y*blockDim.y + threadIdx.y;
  float weight, act, loss, derv;
  int i;
  if(myX < inputs && myY < samples )
    {
    weight = weights[myY*outputs + myX];
    act = acts[myY*inputs + myX];
    derv = 0.0?(act <= 0.0):1.0;
    loss = 0;
    for(i = 0; i < outputs; i++)
      {
      /*the weights matrix will be for outgoing weights,
        so the number of rows will be the size of the output
        the number of columns will match the total number
        of activations. So we are summing over columns, myY indicates
        the sample we are looking at 
      */
      loss = loss + weight*derv*prior_losses[myY*outputs + i];
      }
    losses[myY*inputs + myX] = loss;
    }
  }

__global__ void bp_softmax_cross_entropy_kernel(float *probs, float *truths, int columns, int rows, float *loss)
  {
  /*want to compute the derivative of the error E 
    with respect to the weights W
    E = crossentropy(softmax(sum(W*inputs)), truth)
    d/dx crossentripy(softmax(x), truth) = x-truth ;; (what?)
    so dE/dW = (x-truth) * d/dW (sum(W*inputs)) = 
    (x-truth) * input
  */
  int myX = blockIdx.x*blockDim.x + threadIdx.x;
  int myY = blockIdx.y*blockDim.y + threadIdx.y;
  float prob, truth;
  if(myX < columns && myY < rows)
    {
    prob = probs[myY*columns + myX];
    truth = truths[myY*columns + myX];
    loss[myY*columns + myX] = prob-truth;
    }
  }

__global__ void sum_over_activations_kernel(float *activations, float *coefficients, int columns, int rows, int sample_size, float *output)
  {
  /*coefficients contains a series of errors at the output layer from
    backprop (sample_size x outputs) ;
    activations contains the activations of the 
    previous layer (sample_size x inputs) ;
    for the result I need to multiply each coefficient by its associated
    activation. All coefficients times all activations for a 
    (inputs x outputs) matrix with which to adjust the weight matrix
    these will be summed for the output 
    the rows are */
  int myX = blockIdx.x*blockDim.x + threadIdx.x;
  w = memory_cells[activations].columns;
  int myY = blockIdx.y*blockDim.y + threadIdx.y;
  int i;
  float acc;
  acc = 0.0;
  if(myX < columns && myY < rows)
    {
    for(i = 0; i < sample_size; i++)
      {
      acc = acc + 
        (activations[rows * i + myY] *
         coefficients[columns * i + myX])
      }
    output[myY*columns + myX] = acc/sample_size;
    }
  } 

extern "C" {
void bp_softmax_cross_entropy(int probs, int truths, int loss)
  {
  int h, w;
  float *p, t, l;
  h = memory_cells[probs].rows;
  w = memory_cells[probs].columns;
  if(memory_cells[truths].columns != w){printf("expected %d columns of truth\n", w); return;}
  if(memory_cells[truths].rows != h){printf("expected %d rows of truth\n", h); return;}
  if(memory_cells[loss].rows != h) {printf("expected %d rows for loss\n", h); return;}
  if(memory_cells[loss].columns != w) {printf("expected %d rows for loss\n", w; return;}
  p = (float *)memory_cells[probs].device_pointer;
  t = (float *)memory_cells[truths].device_pointer;
  l = (float *)memory_cells[loss].device_pointer;
  int block_width, block_height, grid_width, grid_height;
  block_width=32;
  block_height=32;
  grid_width=ceil(h/block_width);
  grid_height=ceil(w/block_height);
  /*end block size thoughts*/
  dim3 dimBlock(block_width, block_height, 1);
  dim3 dimGrid(grid_width, grid_height ,1);
  bp_softmax_cross_entropy_kernel<<<dimBlock, dimGrid>>>(p, t, h, w, l);  
  }
}

extern "C" {
void sum_over_activations()
  {
  int h, w;
  float *p, t, l;
  h = memory_cells[probs].rows;
  w = memory_cells[probs].columns;
  if(memory_cells[truths].columns != w){printf("expected %d columns of truth\n", w); return;}
  if(memory_cells[truths].rows != h){printf("expected %d rows of truth\n", h); return;}
  if(memory_cells[loss].rows != h) {printf("expected %d rows for loss\n", h); return;}
  if(memory_cells[loss].columns != w) {printf("expected %d rows for loss\n", w; return;}
  p = (float *)memory_cells[probs].device_pointer;
  t = (float *)memory_cells[truths].device_pointer;
  l = (float *)memory_cells[loss].device_pointer;
  int block_width, block_height, grid_width, grid_height;
  block_width=32;
  block_height=32;
  grid_width=ceil(h/block_width);
  grid_height=ceil(w/block_height);
  /*end block size thoughts*/
  dim3 dimBlock(block_width, block_height, 1);
  dim3 dimGrid(grid_width, grid_height ,1);
  bp_softmax_cross_entropy_kernel<<<dimBlock, dimGrid>>>(p, t, h, w, l);  
    
  }
}

__global__ void update_weights(float *activations, float *weights, float *losses, int rows, int columns, float epsilon)
  {
  int myX = blockIdx.x*blockDim.x + threadIdx.x;
  int myY = blockIdx.y*blockDim.y + threadIdx.y;
  int myIdx = myY*columns + myX;
  if(myX < columns && myY < rows)
    {
    weights[myIdx] = weights[myIdx] + (
                     activations[myIdx] *
                     losses[myIdx] *
                     epsilon );
    }
  }

__global__ void softmax_kernel(float *activations, int rows, int columns, float *partition_function)
  {
  int myX = blockIdx.x*blockDim.x + threadIdx.x;
  int myY = blockIdx.y*blockDim.y + threadIdx.y;
  float act;
  if(myX < columns && myY < rows)
    {
    act = activations[myY*columns + myX];
    act = act/partition_function[myY];
    activations[myY*columns + myX] = act;
    }
  }

extern "C" {
void softmax(int activations, int partition_function)
  {
  int h, w;
  float *acts, *part;
  h = memory_cells[activations].rows;
  w = memory_cells[activations].columns;
  if(memory_cells[partition_function].columns != 1){printf("expected only 1 column of output\n"); return;}
  if(memory_cells[partition_function].rows != h) {printf("expected %d rows for output got %d\n", h, memory_cells[partition_function].rows); return;}
  acts = (float *)memory_cells[activations].device_pointer;
  part = (float *)memory_cells[partition_function].device_pointer;
  int block_width, block_height, grid_width, grid_height;
  block_width=32;
  block_height=32;
  grid_width=ceil(h/block_width);
  grid_height=ceil(w/block_height);
  /*end block size thoughts*/
  dim3 dimBlock(block_width, block_height, 1);
  dim3 dimGrid(grid_width, grid_height ,1);
  softmax_kernel<<<dimBlock, dimGrid>>>(acts, h, w, part);  
   
  }
}

__global__ void partition_function_kernel(float *activations, int rows, int columns, float *partition_function)
  {
  int myIdx = blockIdx.x*blockDim.x+threadIdx.x;
  int i;
  float part = 0.0;
  for(i = 0; i < columns; i++)
    {
    part = part + exp(activations[myIdx*columns + i]);
    }
  partition_function[myIdx] = part;
  }


extern "C" {
void partition_function(int inp, int outp)
  {
  int h, w;
  float *pin, *pout;
  h = memory_cells[inp].rows;
  w = memory_cells[outp].columns;
  if(memory_cells[outp].columns != 1){printf("expected only 1 column of output\n"); return;}
  if(memory_cells[outp].rows != h) {printf("expected %d rows for output got %d\n", h, memory_cells[outp].rows); return;} 
  pin = (float *)memory_cells[inp].device_pointer;
  pout = (float *)memory_cells[outp].device_pointer;
  int block_width, block_height, grid_width, grid_height;
  block_width=32;
  block_height=32;
  grid_width=ceil(h/block_width);
  grid_height=ceil(w/block_height);
  /*end block size thoughts*/
  dim3 dimBlock(block_width, block_height, 1);
  dim3 dimGrid(grid_width, grid_height ,1);
  partition_function_kernel<<<dimBlock, dimGrid>>>(pin, h, w, pout);  
  }
}

__global__ void cross_entropy_loss_kernel
  (
  float *model,
  float *actual,
  int rows,
  int columns,
  float *loss
  )
  {
  int myX = blockIdx.x*blockDim.x+threadIdx.x;
  int myY = blockIdx.y*blockDim.y+threadIdx.y;
  float a, m, l;
  m = model[myY*columns + myX];
  a = actual[myY*columns + myY];
  l = (a)*log(m) + (1-a)*log(1-m);
  loss[myY*columns + myX] = l;
  }

extern "C" {
void cross_entropy_loss(int model, int actual, int loss)
  {
  int h, w;
  float *mod, *act, *l;
  h = memory_cells[model].rows;
  w = memory_cells[model].columns;
  /*all inputs need the same dimensions*/
  if(memory_cells[loss].columns != w){printf("expected %d column of loss got %d\n", w, memory_cells[loss].columns); return;}
  if(memory_cells[loss].rows != h) {printf("expected %d rows for loss got %d\n", h, memory_cells[loss].rows); return;} 
  if(memory_cells[actual].rows != h) {printf("expected %d rows for actuals got %d\n", h, memory_cells[actual].rows); return;} 
  if(memory_cells[actual].columns != w) {printf("expected %d columns for actuals got %d\n", h, memory_cells[actual].columns); return;} 
  mod = (float *)memory_cells[model].device_pointer;
  act = (float *)memory_cells[actual].device_pointer;
  l = (float *)memory_cells[loss].device_pointer;
  
  int block_width, block_height, grid_width, grid_height;
  block_width=32;
  block_height=32;
  grid_width=ceil(h/block_width);
  grid_height=ceil(w/block_height);
  /*end block size thoughts*/
  dim3 dimBlock(block_width, block_height, 1);
  dim3 dimGrid(grid_width, grid_height ,1);
  cross_entropy_loss_kernel<<<dimBlock, dimGrid>>>(mod, act, h, w, l);  
  
  }
}

__device__ void forward_pass()
  {
  /*two matrix multiplications a relu and a softmax*/
  }

__device__ void backward_pass()
  {
  /*measure error using Kulbach-Lieber divergence
    first layer error = */
  }

__device__ void update_weights()
  {
  }

/*this will run on the device :
  a forward pass to compute error
  a backward pass to compute gradient
  an update to the weights
must they all run in the same kernel function?
*/
__global__ void do_the_things()
  {
  forward_pass();
  }
extern "C" {
void matrix_multiply(int cellA, int cellB, int cellC)
  {
  int d1 = memory_cells[cellA].rows; /*should == cellC rows*/
  int d2 = memory_cells[cellB].rows; /*should == cellA columns*/
  int d3 = memory_cells[cellB].columns; /*should == cellC columns*/

  if(d1 != memory_cells[cellC].rows) {printf("Wrong number of output rows expected %d got %d\n", d1, memory_cells[cellC].rows); return; }
  if(d2 != memory_cells[cellA].columns) {printf("Wrong number of columns in first param expected %d got %d\n", d2, memory_cells[cellA].columns); return; }
  if(d3 != memory_cells[cellC].columns) {printf("Wrong number of output columns expected %d got %d\n", d3, memory_cells[cellC].rows); return; }

  float *addrA = (float *)memory_cells[cellA].device_pointer;
  float *addrB = (float *)memory_cells[cellB].device_pointer;
  float *addrC = (float *)memory_cells[cellC].device_pointer;

  /*on NVS 3100M device block can have max 512 threads
    grid can have max 65535 blocks in any one dimension
    I need d1 x d3 threads total, d1 will typically be very large
    the requirements are
    grid_width * block_width >= d3 (as close as possible) 
    grid_height * grid_width >= d1 (as close as possible) 
    thread block dimensions should be multiples of 32.
    I am not using block shared memory so they might as well
    be small.
    */
  int block_width, block_height, grid_width, grid_height;
  block_width=32;
  block_height=32;
  grid_width=ceil(d1/block_width);
  grid_height=ceil(d3/block_height);
  /*end block size thoughts*/
  dim3 dimBlock(block_width, block_height, 1);
  dim3 dimGrid(grid_width, grid_height ,1);
  printf("%d %d\n", d1, d3);
  matrix_multiply_kernel<<<dimBlock, dimGrid>>>(d1, addrA, d2, addrB, d3, addrC);
  }
}

int old_main(int argc, char *argv[])
  {
  cudaDeviceProp dev;
  mem_map mmap;
  network nn;
  FILE *fp;
  nn.input_units = 1;
  nn.hidden_units = 1;
  nn.output_units = 1;

  get_device_statistics(&dev);
  alloc_all_the_mems(&mmap, nn, 100, dev);
  printf("%x %x %x\n", mmap.layer1, mmap.layer2, mmap.data);
  fp = fopen("save.file", "r");
  if(fp == 0)
    {
    printf("weights file not found\n");
    init_weights_without_file(nn, mmap);
    } else {
    printf("found existing weights file\n");
    init_weights_with_file(fp, nn, mmap);
    }
  /*do_the_things();*/
  save_weights(nn, mmap);
  return 0;
  }
