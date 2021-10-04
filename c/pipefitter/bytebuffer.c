#include <stdlib.h>
#include "bytebuffer.h"
/*begin bytebuffer*/

/*need this just for debugging TODO delete*/
#include<stdio.h>

/*
to ensure :
completer_func should be called on each byte in sequence (in blocks of course)
completer_func should ignore everything after the complete byte (it will be called again on those bytes later)
*/

void bytebuffer_init(bytebuffer *buffer)
  {
  struct bytebuffer_block *block = (struct bytebuffer_block *)malloc(sizeof(struct bytebuffer_block));
  buffer->first_block = block; 
  buffer->last_block = block;
  buffer->offset_block = block;
  buffer->complete_block = block;
  buffer->write_offset = 0;
  buffer->read_offset = 0;
  buffer->complete_mark = -1;
  block->data = (char *)malloc( BYTEBUFFER_BLOCK_SIZE );
  block->next = block;
  block->size = BYTEBUFFER_BLOCK_SIZE;
  }

void bytebuffer_add_block(bytebuffer *buffer)
  {
  struct bytebuffer_block *new_block = (struct bytebuffer_block *)malloc(sizeof(struct bytebuffer_block));
  new_block->data = (char *)malloc( BYTEBUFFER_BLOCK_SIZE );
  printf("bytebuffer.c:28 new block %x\n", new_block);
  new_block->size = BYTEBUFFER_BLOCK_SIZE; 
  new_block->next = buffer->first_block;
  buffer->last_block->next = new_block;
  buffer->last_block = new_block;
  }

void bytebuffer_set_complete_mark(bytebuffer *buffer, int mark)
  {
  int offset_mark = mark + buffer->write_offset;
  buffer->complete_mark = offset_mark;
  }

void bytebuffer_set_completer(bytebuffer *buffer, int (*complete)(void *d, char *data, int length), void *d)
  {
  buffer->completer_func = complete;
  buffer->completer_data = d;
  }

int bytebuffer_record_available(bytebuffer *buffer)
  {
  return !(buffer->completer_func && buffer->complete_mark == -1);
  } 

void acceptable_intervals(bytebuffer *buffer, struct bytebuffer_block *block, int *front, int *length)
  {
  int complete_mark, back, end_mark;
  *front = (block==buffer->first_block)?buffer->read_offset:0;
  complete_mark = block==buffer->complete_block ? buffer->complete_mark : -1;
  end_mark = block==buffer->offset_block?buffer->write_offset:block->size;
  back = complete_mark>=0?complete_mark : end_mark;
  *length = back-*front; 
  printf("bytebuffer.c:55 %d %d\n", *front, *length);
  }

void bytebuffer_rotate_record(bytebuffer *buffer)
  {
  /*complete_block becomes first block, block before becomes last_block, complete_mark becomes read_offset if it exists, write_offset becomes
    read_offset otherwise, complete_mark is cleared*/
  struct bytebuffer_block *sploop = buffer->first_block;
  int front, length, complete_mark;
  while(sploop->next != buffer->complete_block) sploop = sploop->next;
  buffer->first_block = buffer->complete_block;
  buffer->last_block = sploop;
  if( buffer->complete_mark >= 0 ) buffer->read_offset = buffer->complete_mark; else buffer->read_offset = buffer->write_offset;
  printf("bytebuffer.c:67 rotate record\n");
  buffer->complete_mark = -1;
  sploop = buffer->first_block;
  while(buffer->complete_mark == -1)
    {
    acceptable_intervals(buffer, sploop, &front, &length);
    buffer->complete_mark = buffer->completer_func(buffer->completer_data, sploop->data+front, length);
    if(buffer->complete_mark != -1) buffer->complete_block = sploop; 
    sploop = sploop->next;
    if(sploop == buffer->first_block) break;
    }
  if(buffer->complete_mark != -1) buffer->complete_mark = front + buffer->complete_mark; 
  printf("bytebuffer.c:77 complete mark = %d\n", buffer->complete_mark); 
  }

void bytebuffer_advance_block(bytebuffer *buffer)
  {
  if(buffer->offset_block == buffer->last_block) bytebuffer_add_block(buffer);
  buffer->offset_block = buffer->offset_block->next;
  buffer->write_offset = 0; 
  } 

/*this will give me a block of memory for the next buffer contents*/
void bytebuffer_append_start(bytebuffer *buffer, char **data, unsigned int *length)
  {
  *length = buffer->offset_block->size - buffer->write_offset;
  if(*length <= 0)
    {
    if(buffer->offset_block == buffer->last_block)
      { bytebuffer_add_block(buffer); } buffer->offset_block = buffer->offset_block->next;
    *length = buffer->offset_block->size;
    buffer->write_offset = 0;
    }
  *data = buffer->offset_block->data + buffer->write_offset; 
  }

/*after I have written to a buffer I should tell it how much space I actually used*/
void bytebuffer_append_trim(bytebuffer *buffer, unsigned int length)
  {
  /*once I know the amount of data I can check if this block is the end of the next record*/
  int mark;
  if(length == 0) return; /*this is a noop, otherwise I will rerun completer_func and it should do nothing*/
  if(buffer->completer_func && buffer->complete_mark == -1) 
    {
    printf("bytebuffer.c:107 invoke completer_func write offset = %d\n", buffer->write_offset);
    mark = buffer->completer_func(buffer->completer_data, buffer->offset_block->data + buffer->write_offset, length);
    if(mark >= 0) buffer->complete_mark = mark + buffer->write_offset;
    } else {
    printf("bytebuffer.c:111 do not invoke completer_func complete_mark = %d\n", buffer->complete_mark);
    }
  buffer->write_offset = buffer->write_offset + length; /*this would be an error if we try to trim past the end of the block*/ 
  } 

void bytebuffer_reset(bytebuffer *buffer)
  {
  buffer->offset_block = buffer->first_block;
  buffer->write_offset = 0;
  }

void bytebuffer_iter_init(bytebuffer_iter *iter, bytebuffer *buffer)
  {
  iter->header = buffer;
  iter->current_block = buffer->first_block;
  } 

/*returns length of data block, 0 if EOF*/
unsigned int bytebuffer_iter_next(bytebuffer_iter *iter, char **buffer, unsigned int *length)
  {
  int start_offset;
  int end_offset;
  int last_block_end_offset;
  int is_last_block = (iter->current_block == iter->header->offset_block && iter->header->complete_mark == -1 ) || 
                      (iter->current_block == iter->header->complete_block && iter->header->complete_mark != -1 );
  bytebuffer *bbuf = iter->header;
  last_block_end_offset = iter->header->complete_mark == -1 ? iter->header->write_offset : iter->header->complete_mark; 
  printf("bytebuffer.c:98 %x\n", iter->current_block ); 
  if(iter->current_block == 0) return 0;
  start_offset = iter->current_block == bbuf->first_block ? bbuf->read_offset : 0;
  end_offset = is_last_block ? 
     last_block_end_offset 
     : iter->current_block->size; 
  *buffer = iter->current_block->data + start_offset;
  if( is_last_block  )
    {
    printf("bytebuffer.c:152 %d %d\n", end_offset, start_offset);
    *length = end_offset - start_offset; 
    } else {
    *length = iter->current_block->size - start_offset;
    }
  if(is_last_block) iter->current_block = 0; else
  iter->current_block = iter->current_block->next;
  printf("bytebuffer.c:159 %d\n", *length);
  return *length;
  }

/*end bytebuffer*/

