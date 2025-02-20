all: fun.o
	gcc fun.o -o fun.out -nostdlib -g -m32 -no-pie -O3 -Wall -Werror -fno-omit-frame-pointer

fun.o: fun.S
	gcc -m32 -c fun.S -o fun.o 

clean:
	rm *.o *.out 
