char name_table[1024];
int name_table_size = 0;
int current_label = 2;

int get_name( char *name )
{
  char *string =  name_table;
  int ret = 0;
  int offset = 0;
  while( offset < name_table_size )
    {
      if( !strcmp( string, name ) ) { 
         return ret; 
         }

      offset = offset + strlen( string ) + 1;
      ret = ret+1;
      string = string + strlen( string ) + 1;
    }
  strcpy( string, name );
  name_table_size = name_table_size + strlen( string ) + 1;
  return ret;
}

char *decode_name( int code )
{
  char *ret = name_table;
  while( code != 0 )
  {
    ret = ret + strlen( ret ) + 1;
    code = code-1;
  }
  return ret;
}


