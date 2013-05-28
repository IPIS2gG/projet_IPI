SRC = authserv.c
EXEC = authserv
CC=gcc

# options de compilation ################"""
GTK= `pkg-config --cflags --libs gtk+-2.0`
FLAGS = -Wall -I /usr/include/postgresql -lpq -lcrypto
########################

OBJ=$(SRC:.c=.o)

all: $(EXEC)

$(EXEC): $(OBJ)
	@ echo linking ...
	@ $(CC) -o $@ $^ $(FLAGS);
	@ echo $@ compilé 

%.o: %.c
	@ echo compilation de $< ...
	@ $(CC) -o $@ -c $< $(FLAGS);

clear: clean

clean:
	@rm *.o $(EXEC);
