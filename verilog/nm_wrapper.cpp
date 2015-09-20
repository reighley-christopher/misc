#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "obj_dir/Vnoisemunger.h"

int read_bit_vector()
  {
  int i = 0;
  int c = 0;
  int out = 0;
  for(i = 0; i < 8; i++)
    {
    out << 1;
    c = getc(stdin);
    if(c == EOF) { return EOF; }
    out = out + (c-'0');
    }
  c = getc(stdin);
  if(c != '\n') { return EOF; }
  return out;
  }


void seed_rand()
  {
  int seed;
  seed = time(0);
  srand(seed);
  }

int main(int argv, char **argc)
  {
  Vnoisemunger* V = new Vnoisemunger;
  int byte_in, bit_out;
  long int clock = 0;
  seed_rand();
  printf("%d\n", rand());
  while(true)
    {
    switch(clock%8)
      {
      case 0 : 
        byte_in = read_bit_vector();
        V->nois = rand()&15;
        V->in = byte_in;
        break;
      case 1 : 
      case 2 : break;
      case 3 : V->clk = 1; break;
      case 4 : 
      case 5 : break; 
      case 6 : V->clk = 0; break; 
      case 7 : bit_out = V->out; putc(bit_out + '0', stdout); break;
      }
    V->eval();
    if(byte_in == EOF) {break;}
    clock++;
    }
  delete V;
  exit(0);
  }
