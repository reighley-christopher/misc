#include <stdio.h>
#include "bytebuffer.h"

int mark;
int test_completer(void *d, char *data, int length)
  {
  printf("updating mark %d\n", *(int *)d);
  return *(int *)d; 
  }

int test_completer2(void *d, char *data, int length)
  {
  int i;
  for(i = 0; i < length; i ++)
    {
    if(data[i] == '\n') { printf("complete %d\n", i+1) ; return i+1; }
    }
  printf("incomplete\n");
  return -1;
  }

int test1(int argc, char **argv)
  {
  char *data;
  unsigned int length;
  int i;
  bytebuffer buffer;
  bytebuffer_iter iter;
  mark = -1;
  bytebuffer_init(&buffer); /*record starts at 0:0*/
  bytebuffer_set_completer(&buffer, test_completer, &mark);
  bytebuffer_append_start(&buffer, &data, &length);
  bytebuffer_append_trim(&buffer, 16); /*0:0-0:16*/

  bytebuffer_append_start(&buffer, &data, &length);
  mark = 2; 
  bytebuffer_append_trim(&buffer, 10); /*0:0-0:18 0:19-0:26*/
 
  bytebuffer_iter_init(&iter, &buffer);
  bytebuffer_iter_next(&iter, &data, &length);
  printf("should be 18 : %d\n", length);

  bytebuffer_rotate_record(&buffer); /*0:18-0:26*/
  bytebuffer_iter_init(&iter, &buffer);
  bytebuffer_iter_next(&iter, &data, &length);
  printf("should be 8 : %d\n", length);

  bytebuffer_append_start(&buffer, &data, &length);
  bytebuffer_append_trim(&buffer, length); /*0:18-0:1023*/

  printf("should be %d : %d\n", BYTEBUFFER_BLOCK_SIZE - 26, length );

  bytebuffer_append_start(&buffer, &data, &length);
  mark = 100;
  printf("write_mark = %d complete_mark = %d\n", buffer.write_offset, buffer.complete_mark);
  /*0:18-1:99*/
  bytebuffer_append_trim(&buffer, length); /*0:18-1:99 1:100-1:1023*/
 
  printf("should be %d : %d\n", BYTEBUFFER_BLOCK_SIZE , length );

  bytebuffer_iter_init(&iter, &buffer);
  printf("%x %x\n", iter.current_block, iter.current_block->next); 
  bytebuffer_iter_next(&iter, &data, &length);
  printf("should not be null : %x\n", iter.current_block); /*this should not be zero we should have another block*/
  printf("should be %d : %d\n", BYTEBUFFER_BLOCK_SIZE-18 ,length );
  
  bytebuffer_iter_next(&iter, &data, &length);
  printf("should be 100: %d\n", length);
  printf("should be 0: %d\n", bytebuffer_iter_next(&iter, &data, &length));
  bytebuffer_rotate_record(&buffer); /*0:100-0:1023*/
  printf("should be true: %d\n", buffer.offset_block == buffer.first_block); 
  
  printf("complete mark : %d\n", buffer.complete_mark);
  bytebuffer_append_start(&buffer, &data, &length);
  printf("complete mark : %d\n", buffer.complete_mark);
  bytebuffer_append_trim(&buffer, 200); /*0:100-1:200*/
  printf("complete mark : %d\n", buffer.complete_mark);
  printf("should be false: %d\n", buffer.offset_block == buffer.first_block);
  printf("should be true: %d\n", buffer.offset_block == buffer.last_block);
   
  /*two records in the same block, set the complete mark and advance the iterator,
    rotate the buffer (to the next record)
    set the complete mark again and create a new iterator*/
  //bytebuffer_set_complete_mark(&buffer, 100); /*set complete mark needs to be set before append_trim, this is a problem*/
  bytebuffer_iter_init(&iter, &buffer); 
  bytebuffer_iter_next(&iter, &data, &length);
  printf("should be 924 : %d\n", length);
  bytebuffer_iter_next(&iter, &data, &length);
  printf("should be 100 : %d\n", length);
  bytebuffer_rotate_record(&buffer); 
  bytebuffer_set_complete_mark(&buffer, 50);
  bytebuffer_iter_init(&iter, &buffer); 
  bytebuffer_iter_next(&iter, &data, &length); 
  printf("should be 50 : %d\n", length); 
  }

char *fill(char *data, int length)
  {
  int i;
  for(i = 0; i < length; i++)
    {
    data[i] = '.';
    }
  data[length-1] = '\n';
  return data+length;
  } 

char *fill2(char *data, int length)
  {
  int i;
  for(i = 0; i < length; i++)
    {
    data[i] = '.';
    }
  return data + length;
  }

int test2(int argc, char **argv)
  {
  char *data;
  unsigned int length;
  int retval;
  bytebuffer buffer;
  bytebuffer_iter iter;
  bytebuffer_init(&buffer); /*record starts at 0:0*/
  bytebuffer_set_completer(&buffer, test_completer2, 0);

  printf("*** 1) append 16\n"); 
  bytebuffer_append_start(&buffer, &data, &length);
  fill(data, 16);
  bytebuffer_append_trim(&buffer, 16); /*0:0-0:16*/

  printf("*** 2) read 16\n");
  bytebuffer_iter_init(&iter, &buffer);
  bytebuffer_iter_next(&iter, &data, &length);
  printf("should be 16 = %d\n", length); 

  printf("*** 3) append 24\n"); 
  bytebuffer_append_start(&buffer, &data, &length);
  fill(data, 24);
  bytebuffer_append_trim(&buffer, 24); /*0:0-0:16*/

  printf("*** 4) read 16\n");
  bytebuffer_iter_init(&iter, &buffer);
  bytebuffer_iter_next(&iter, &data, &length);
  printf("should be 16 = %d\n", length);  /*I haven't rotated the record yet*/

  printf("*** 5) rotate record\n");  
  bytebuffer_rotate_record(&buffer);

  printf("*** 6) read 24\n");
  bytebuffer_iter_init(&iter, &buffer);
  bytebuffer_iter_next(&iter, &data, &length);
  printf("should be 24 = %d\n", length); 
  
  printf("*** 7) write to end\n");
  bytebuffer_append_start(&buffer, &data, &length);
  fill2(data, length);
  bytebuffer_append_trim(&buffer, length);

  printf("*** 8) write some more\n");
  bytebuffer_append_start(&buffer, &data, &length);
  fill(data, 36);
  bytebuffer_append_trim(&buffer, 36);

  printf("*** 9) read 24\n");  
  bytebuffer_iter_init(&iter, &buffer);
  bytebuffer_iter_next(&iter, &data, &length);
  printf("should be 24 = %d\n", length); /*still haven't rotated record*/
  retval = bytebuffer_iter_next(&iter, &data, &length);
  printf("should be 0 = %d\n", retval); 
  }

int main(int argc, char **argv)
  {
  test2(argc, argv);
  }

/*test cases
- try to rotate record before the next record has reached the end (bytebuffer_record_available = false)
  not an error case, we will normally be doing this after the last record has been read
- try to rotate record even though there is no record to rotate to
  this should be a noop
- try to get an iterator on an incomplete record
  not an error, but probably not safe (the iterator should not block on the last record but what could it do?

- edge cases in which there is only one block so first_block == last_block and it is hard to tell when I am done
- edge case in which the complete sequence is 0 bytes long or 1 byte long
*/
