all: write.c setup.c
	gcc -g write.c
	gcc setup.c -o setup.out

setup:
	./setup.out $(args)

run:
	./a.out

clean:
	rm -rf *.o *.out
