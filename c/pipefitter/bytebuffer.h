typedef struct 
  {
  int (*func)(void *d, unsigned char *data, int length);
  void *(*alloc)();
  void (*init)(void *m); 
  void (*free)(void *m);
  } completer_module;

typedef struct _bytebuffer
  {
  struct bytebuffer_block *first_block;
  struct bytebuffer_block *last_block;
  unsigned int write_offset; /*write_offset is the end of the data to be appended to*/
  unsigned int read_offset; /*read offset is the beginning of the data to be read (perhaps all the way to write offset but no further)*/
  struct bytebuffer_block *complete_block; 
  int complete_mark; /*index into the complete_block of the end of the next completed record, -1 if there is no completed record*/
  struct bytebuffer_block *offset_block; 
  completer_module *completer; 
  void *completer_data; 
  } bytebuffer; 

struct bytebuffer_block
  {
  int size;
  struct bytebuffer_block *next;
  unsigned char *data;
  };

typedef struct _bytebuffer_iter
  {
  bytebuffer *header;
  struct bytebuffer_block *current_block; 
  } bytebuffer_iter;

#define BYTEBUFFER_BLOCK_SIZE 1024

void bytebuffer_init(bytebuffer *buffer);

void bytebuffer_append_start(bytebuffer *buffer, unsigned char **data, unsigned int *length);
void bytebuffer_append_trim(bytebuffer *buffer, unsigned int length);

void bytebuffer_set_complete_mark(bytebuffer *buffer, int mark);
void bytebuffer_set_completer(bytebuffer *buffer, completer_module *completer);
int bytebuffer_record_available(bytebuffer *buffer);

void bytebuffer_iter_init(bytebuffer_iter *iter, bytebuffer *buffer);
unsigned int bytebuffer_iter_next(bytebuffer_iter *iter, unsigned char **buffer, unsigned int *length);

void bytebuffer_rotate_record(bytebuffer *buffer);
