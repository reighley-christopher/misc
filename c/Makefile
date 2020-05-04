a.out : test.c menu-term.o
	gcc menu-term.o test.c -lncursesw -lpthread

menu-drive.o : menu-drive.c
	gcc -c menu-drive.c

menu-data.o : menu-data.c
	gcc -c menu-data.c

menu-term.o : menu-term.c
	gcc -c menu-term.c 

menu-drive : menu-drive.o menu-data.o menu-term.o
	gcc -o menu-drive menu-drive.o menu-data.o menu-term.o -lncursesw -lpthread 
