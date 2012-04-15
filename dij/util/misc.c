#include <stdlib.h>
/*TODO remove references to standard libraries*/
/*utility functions for editing arrays of int*/

int array_contains( int *array, int size, int element )
{
   int i;
   for(i = 0; i < size; i++ )
   {
      if( array[i] == element ) return -1;
   }
   return 0;
}

int *array_add( int *array, int size, int element )
{
   int *ret = malloc(sizeof(int) * (size + 1 ) );
   int i;
   for(i = 0; i < size; i++ )
   {
      ret[i] = array[i];
   }
   ret[i] = element;
   if( array ) free(array);
   return ret;
}

int *array_remove( int *array, int size, int element )
{
   int *ret = malloc(sizeof(int) * (size - 1 ) );
   int i;
   int j = 0;
   for(i = 0; i < size; i++ )
   {
      if( array[i] != element )
      {
          ret[j] = array[i]; j++;         
      }
   }
   free(array);
   return ret;
}

int *array_copy( int *array, int size )
{
   int *ret = malloc(sizeof(int) * size );
   int i;
   for( i = 0; i < size; i++ ) { ret[i] = array[i]; }
   return ret;
}

/*end utility functions*/
