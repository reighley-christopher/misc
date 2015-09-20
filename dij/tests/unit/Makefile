njoin.o : njoin.c 

fixtures.o : fixtures.c

njoin : njoin.o fixtures.o ../../control/dij_control.a ../../util/util.a

test : njoin
	./njoin

clean : 
	rm *.o

