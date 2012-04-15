/*
the first parameter is a function pointer
the second parameter is the size in bytes of the array of parameters
the third parameter is the address of an array of parameters
*/
dij_c_call:
push %ebp
mov %esp,%ebp
mov 0xc(%ebp), %eax
add 0x10(%ebp), %eax
dij_c_call_unfinished:
cmp %eax, 0x10(%ebp)
jge dij_c_call_finished
sub $4, %eax
push (%eax)
jmp dij_c_call_unfinished
dij_c_call_finished:
mov 0x8(%ebp), %eax
call *%eax
leave
ret

.globl dij_c_call
