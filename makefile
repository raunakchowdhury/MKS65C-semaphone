all: main.o write.o setup.o
	gcc main.o write.o
	gcc -o setup setup.o

main.o: main.c
	gcc -c -g main.c

setup.o: setup.c setup.h
	gcc -c -g setup.c

write.o: write.c write.h
	gcc -c -g write.c

setup:
	./setup.out $(args)

run:
	./a.out

clean:
	rm -rf *.o *.out
