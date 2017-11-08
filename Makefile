build: pacman task1

pacman: pacman.c
	gcc -Wall pacman.c -o pacman

task1: task1.c
	gcc -Wall task1.c -o task1

clean:
	rm pacman task1
