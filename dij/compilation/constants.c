#include <stdio.h>
#include <stdlib.h>

typedef long int DIJ_WORD;

typedef int (*constantizer)( char *rep, DIJ_WORD *constant, DIJ_WORD *segment );

int c_string( char *rep, DIJ_WORD *constant, DIJ_WORD *segment  )
{
  char wordsize = sizeof( DIJ_WORD );
  char *c = rep;
  int i;
  DIJ_WORD accum;
  DIJ_WORD mask;
  int test_endian = 1;
  int ret = 0;
  int termed = 0;
  while( *c != '\0' )
    {
      accum = 0;
      for( i = 0; i < wordsize; i++ )
        {
          if( *c == '\\' )
             {
             c = c + 1;
             switch( *c )
               {
               case 'n': *c = '\n'; break;
               case 't': *c = '\t'; break;
               case '\\': *c = '\\'; break;
               }
             }
          mask = *c; /*this was a hard bug to find, it used to be a single line : mask = *c<<i*8;  that's wrong*/
          mask = mask<<(i*8);
          accum = accum | mask;
          if( *c != '\0' ) { c = c + 1; } else {termed = 1;}
        }

      segment[ret] = accum;
      ret = ret + 1;
    }
  if (!termed ) { segment[ret] = 0; ret = ret+1; }
  return ret;
};

void *constant_table[3] = 
  { "[c-string:", c_string, 0
};

int process_constant( char *kind, char *rep, DIJ_WORD *constant, DIJ_WORD *segment )
{
  constantizer fun;
  void **table = constant_table;
  while( *table )
    {
      if( !strcmp( *table, kind ) )
	{
	  fun = (constantizer)(*(table+1));
          return fun( rep, constant, segment );
        }
      table = table + 2;
    }
  return 0;
}
