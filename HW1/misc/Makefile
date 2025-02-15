all: fun.o
	gcc fun.o -o fun.out -nostdlib -g -no-pie -O3 -Wall -Werror -fno-omit-frame-pointer

fun.o: fun.S
	as fun.S -o fun.o

clean:
	rm *.o *.out 
