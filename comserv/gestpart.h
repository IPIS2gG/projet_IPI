typedef struct partie partie;

partie * init(int l, int h, int nb_joueur);

int play(partie *, int x, int y, int joueur);

void destroy(partie *);

char * getmap(partie *);

int getscore(partie *, int joueur);
