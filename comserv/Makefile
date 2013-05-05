SRC = comserv.c function.c gestpart.c com_ingame.c
EXEC = servjeu
CC=g++

# options de compilation ################"""
GTK= `pkg-config --cflags --libs gtk+-2.0`
FLAGS = -Wall -pthread -ggdb
########################

OBJ=$(SRC:.c=.o)

all: $(EXEC)

bidon: comserv.o function.o com_ingame.o gestpart_bidon.o
	@echo linking bidon ...
	@ $(CC) -o servjeu_bidon $^ $(FLAGS);
	@echo servjeu_bidon compilé avec succés !

$(EXEC): $(OBJ) 
	@echo linking ...
	@ $(CC) -o $@ $^ $(FLAGS);
	@echo $(EXEC) compilé avec succés !
	
comserv.o: function.h gestpart.h com_ingame.h
com_ingame.o : function.h com_ingame.h gestpart.h

%.o: %.c
	@echo compilation de $<
	@$(CC) -o $@ -c $< $(FLAGS);

clear : clean

clean :
	@rm *.o
