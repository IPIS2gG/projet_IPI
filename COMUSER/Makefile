SRC = interface_graphique.c fonctions_utiles.c fonctions_connexion.c fonctions_fenetres.c comuser.c fonctions_joueur.c fonctions_admin.c admin_windows.c
EXEC = comuser
CC=gcc

# options de compilation
GTK= `pkg-config --cflags --libs gtk+-2.0`
SDL= -lSDLmain -lSDL -lSDL_ttf
FLAGS = -Wall $(SDL) $(GTK) -ggdb


########################
all: programme tests


# Programme
programme : $(EXEC)

OBJ=$(SRC:.c=.o)
$(EXEC): $(OBJ)
	$(CC) -o $@ $^ $(FLAGS);

%.o: %.c
	$(CC) -o $@ -c $< $(FLAGS);

interface_graphique.o : interface_graphique.h
comuser.h : interface_graphique.h
comuser.o : comuser.h
fonctions_connexion.o : comuser.h
fonctions_fenetres.o : comuser.h
fonctions_joueurs.o : comuser.h
fonctions_admin.o : comuser.h

# Tests
tests : test_interface_graphique

test_interface_graphique : interface_graphique.o test_interface_graphique.o
	$(CC) -o $@ $^ $(FLAGS);

test_interface_graphique.o : interface_graphique.h


# Nettoyage
clear:
	rm *.o $(EXEC) test_interface_graphique;

clean:
	rm *.o;
