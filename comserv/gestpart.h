typedef struct partie partie;

partie * init(int l, int h, int nb_joueur);

int play(partie *, int x, int y, int joueur);

void destroy(partie *);

char * getmap(partie *);
//alloué dynamiquement

int getscore(partie *, int joueur);

int getvainqueur(partie *);
