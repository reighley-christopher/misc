#define BUFFER_SIZE 2048

typedef struct _term
  {
  int state;
  int writepipe, readpipe;
  pthread_t threadid;
  void (*display_callback)(char c, void *p);
  void *display_parameter; 
  pthread_mutex_t buffer_mutex;
  int buffer_offset;
  char buffer[BUFFER_SIZE];
  int echo; /*TODO make this a bitmask of good flags*/ 
  } *term; 

extern pthread_mutex_t buffer_mutex;
extern char buffer[BUFFER_SIZE]; /*buffering the input makes curses rendering worse!! WTF*/

term termset[26];

term new_term();
void kill_term( term target );

void readwrite( int readpipe, int writepipe );
term fork_a_thing( int index , WINDOW *start, char *command);

void term_output_buffer(term terminal);
void term_set_display_callback( term terminal, void (*display)(char c, void *p), void *parameter ); 

int term_message(term terminal, char *message);
int term_test_echo(term terminal);
