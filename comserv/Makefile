SRC = comserv.c function.c
EXEC = servjeu
CC=g++

# options de compilation ################"""
GTK= `pkg-config --cflags --libs gtk+-2.0`
FLAGS = -Wall -pthread -ggdb
########################

OBJ=$(SRC:.c=.o)

all: $(EXEC)

$(EXEC): $(OBJ)
	@echo linking ...
	@ $(CC) -o $@ $^ $(FLAGS);
	@echo $(EXEC) compilé avec succés !
	
comserv.o: function.h

%.o: %.c
	@echo compilation de $<
	@ $(CC) -o $@ -c $< $(FLAGS);

clear : clean

clean :
	@rm *.o
