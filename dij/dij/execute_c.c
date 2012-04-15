/*
generic code for executing a C function.

push %ebp                    55 
mov %esp, %ebp               89 (move word) e5=11 (register) 100 (esp) 101 (ebp)
sub ???, %esp                83 (subtract word) ec= 11 (register) 101 (immediate 8) 100 (esp) 18 = 24 = 6 words
movl ????, ???(%esp)         c7 44 24 08 03 00 00 00 
...                          c7 44 24 04 02 00 00 00 
movl ????, (%esp)            c7 04       01 00 00 00 
call ????                    e8 d6 ff ff ff c9     (calling 0x1f7a from 0x1f9f), with
leave                        c9
ret                          c3 (immediate near)
*/

int execute_C2( struct _parameter *p, int code_size )
{
   int code[code_size];
   struct _parameter *sploop = p->next;
   int f = p->value.value;
      
}

int execute_C1( struct _parameter *p )
{
   
}
