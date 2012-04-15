dij_control.a : apply_icode.o ichannel.o ifgraph.o iprocess.o spawn_icode.o
	ar -r dij_control.a apply_icode.o ichannel.o ifgraph.o iprocess.o spawn_icode.o 

apply_icode.o : apply_icode.c
	gcc -c -ggdb apply_icode.c

ichannel.o : ichannel.c
	gcc -c -ggdb ichannel.c

ifgraph.o : ifgraph.c
	gcc -c -ggdb ifgraph.c

iprocess.o : iprocess.c
	gcc -c -ggdb iprocess.c

spawn_icode.o : spawn_icode.c
	gcc -c -ggdb spawn_icode.c

