util.a : misc.o name_table.o
	ar -r util.a misc.o name_table.o

misc.o : misc.c
	gcc -c -ggdb misc.c

name_table.o : name_table.c
	gcc -c -ggdb name_table.c


