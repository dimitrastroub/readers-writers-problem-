all: project

main.o: main.c
	gcc -c main.c 

semaphores.o: semaphores.c
	gcc -c semaphores.c 

sharedMem.o: sharedMem.c
	gcc -c sharedMem.c

project: main.o semaphores.o sharedMem.o
	gcc main.o semaphores.o sharedMem.o -o project -lm

clean:
	rm -f *.o project
