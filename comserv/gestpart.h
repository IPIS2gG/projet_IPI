typedef struct partie partie;

partie* init(int l, int h, int nb_joueurs);

int play(partie * p, int x, int y, int joueur);
	//joueur joue un coup en x,y
	//renvoit -1 si coup incorrect, 0 si partie terminée, joueur suivant si ok

void destroy(partie *);

char * getmap(partie *);
	//renvoit une string de communication de la map [taille 99*99 max]
	//alouée dynamiquement -> free à faire

int * getscore(partie *);
	//renvoi les scores en cours
