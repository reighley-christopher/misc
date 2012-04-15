
dij : main.o tree.o scanner.o y.tab.o scheme.o semantic.o constants.o events.o
	cc -o dij -pedantic tree.o constants.o main.o scanner.o y.tab.o scheme.o semantic.o events.o -lfl -framework PLT_MzScheme

dij-exec : dij-exec.c dij_c_call.s dij-asm.c
	cc -o dij-exec -pedantic dij-exec.c dij_c_call.s dij-asm.c

run : run.o command.o machine.o
	cc -o run run.o command.o machine.o -lfl

dij-asm.c : dij-asm.l
	lex -odij-asm.c dij-asm.l

run.o : run.c

tree.o : tree.c tree.h

scanner.o : scanner.l y.tab.h

command.o : command.l machine.h

machine.o : machine.c machine.h

semantic.o : semantic.c semantic.h

events.o : events.c events.h

constants.o : constants.c

y.tab.o : y.tab.c

y.tab.c y.tab.h: parser.y semantic.h scheme.h
	yacc -dv parser.y

dij-asm.c : dij-asm.l

dij-asm : dij-asm.c
	cc -o dij-asm -lfl dij-asm.c
clean :
	rm *.o
	rm y.tab.h
	rm y.tab.c

dij_memory.o : dij_memory.c
	gcc dij_memory.c -c -ggdb -o dij_memory.o

dij_curry.o : dij_curry.c
	gcc dij_curry.c -c -ggdb -o dij_curry.o

dijrtc.o : dij_memory.o dij_curry.o
	ld dij_memory.o dij_curry.o -o dijrtc.o

add.s : add.d dij dij-asm
	./dij add.d | ./dij-asm > add.s

add.o : add.s dijrt.s
	as add.s dijrt.s -o add.o

add : add.o dijrtc.o
	gcc add.o dijrtc.o -ggdb -o add

	
	
