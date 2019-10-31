cuda-obj.o : mlp.cu
	nvcc -arch=sm_12 -ccbin gcc-4.8 -c -I/usr/local/cuda-6.0/include mlp.cu -o cuda-obj.o

guile-main.o :  guile-main.c cuda-bindings.h
	gcc guile-main.c -c -I/usr/local/include/guile/2.0/ -lguile-2.0 -o guile-main.o

test : guile-main.o
	nvcc -ccbin gcc-4.8 -I/usr/local/cude-6.0/include -I/usr/local/include/guile/2.0/ mlp.cu guile-main.c -lguile-2.0

test2 : guile-main.o cuda-obj.o simple-draw.o 
	nvcc -arch=sm_12 -ccbin gcc-4.8 cuda-obj.o guile-main.o simple-draw.o -lcudart -lguile-2.0 -lm

cursing : ascii-graphics.c
	g++ ascii-graphics.c -o cursing -lncursesw

simple-draw.o : simple-draw.c
	gcc -c simple-draw.c -o simple-draw.o
