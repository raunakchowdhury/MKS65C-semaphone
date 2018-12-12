all: write.c setup.c
	gcc -c -g write.c
	gcc setup.c -o setup

setup:
	./setup.out $(args)

run:
	./a.out

clean:
	rm -rf *.o *.out
