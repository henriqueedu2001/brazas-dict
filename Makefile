CFLAGS = -Wall -Wextra -O2

run: compile
	./program

compile:
	gcc -c ./src/main.c -o main.o $(CFLAGS)
	gcc -c ./src/kdtree.c -o kdtree.o $(CFLAGS)
	gcc main.o kdtree.o -o program $(CFLAGS)

clean:
	rm main.o kdtree.o program