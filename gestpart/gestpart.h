typedef struct partie * partie;

partie * init(int, int, int);

int play(partie *, int, int, int);

void destroy(partie *);

char * getmap(partie *);

int * getscore(partie *);
