void *f1( void **R )
  {
  long int a, b;
  return (void *)(long int)R[0] + (long int)R[1];
  }

void *f2( void *(*F)( void **R ) )
  {
  void *r[2] = {(void *)1, (void *)2};
  return F(r);
  }
