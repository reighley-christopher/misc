
dij : main.o tree.o scanner.o y.tab.o scheme.o semantic.o constants.o events.o
	cc -o dij -pedantic tree.o constants.o main.o scanner.o y.tab.o scheme.o semantic.o events.o ../util/util.a -lfl -lguile-2.0

tree.o : tree.c tree.h

scanner.c : scanner.l y.tab.h
	flex -o scanner.c scanner.l

scanner.o : scanner.c
	gcc -c -ggdb scanner.c

semantic.o : semantic.c semantic.h

events.o : events.c events.h

constants.o : constants.c

scheme.o : scheme.c

y.tab.o : y.tab.c
	gcc -c -ggdb y.tab.c

y.tab.c y.tab.h: parser.y semantic.h scheme.h
	yacc -dv parser.y

clean :
	rm *.o
	rm y.tab.h
	rm y.tab.c
	rm scanner.c
	rm *~
	rm dij
