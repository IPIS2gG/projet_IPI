all : test

test: interface_graphique.o test.o
	gcc -Wall -o test interface_graphique.o test.o -lSDLmain -lSDL -lSDL_ttf

test.o: test.c interface_graphique.h
	gcc -c -Wall test.c

interface_graphique.o : interface_graphique.c
	gcc -pthread -Wall -c interface_graphique.c

clean :
	rm *.o
