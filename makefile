all: sema.c control.c
	gcc -g sema.c
	gcc control.c -o control.out

setup:
	./control.out $(args)

run:
	./a.out

clean:
	rm -rf *.o *.out *.txt
