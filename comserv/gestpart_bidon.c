#include <stdio.h>
#include <stdlib.h>

typedef struct partie partie;
struct partie
{
	int ** map;
	int* scores;
	int nb_joueur;
	int w;
	int h;
	//et pleins d'autres trucs
};

partie * init(int l, int h, int nb_joueurs)
{
	int i,j;
	partie* part = (partie*) calloc(1, sizeof(partie));
	if(part==NULL)
		return NULL;
	part->scores=(int*) calloc(nb_joueurs, sizeof(int));
	if(part->scores==NULL)
		return NULL;
	part->map = (int**) calloc(l, sizeof(int*));
	if(part->map!=NULL)
	{
		for(i=0; i<l; ++i)
		{
			part->map[i]=(int*) calloc(h, sizeof(int));
			if(part->map[i]==NULL)
				return NULL; //on se fiche de désalloué la mémoire
							//le programme terminera de toute manière
			for(j=0; j<h; ++j) //init contenu
			{
				part->map[i][j]=0;
			}
		}
	}
	part->w=l;
	part->h=h;
	part->nb_joueur=nb_joueurs;
	return part;
}

int play(partie * p, int x, int y, int joueur)
{
	if(p->map[x][y]!=0)
	{
		return -1;
	}
	else
	{
		if(x==0 && y==0) //code de test pour terminer la partie
		{
			return 0;
		}
		else
		{
			p->map[x][y]=1; //on joue
			p->scores[joueur-1]+=1; //le joueur marque un pt
			joueur ++;
			return (joueur>p->nb_joueur) ? 1 : joueur;
		}
	}
	return -2; //enlève un warning useless
}

void destroy(partie * p)
{
	int i;
	free(p->scores);
	for(i=0; i<p->w; ++i)
	{
		free(p->map[i]);
	}
	free(p->map);
	free(p);
}

char * getmap(partie * p)
{
	int i,j;
	char* buff;
	buff=(char*) calloc(p->w*p->h+1, sizeof(char)); //buff rempli de \0
	for(j=0; j<p->h; ++j)
	{
		for(i=0; i<p->w; ++i)
		{
			buff[j*p->w+i]=(char) p->map[i][j]+48; //(convertion en char)
		}
	}
	buff[p->w*p->h]='\0'; //pour le principe
	return buff;
}

int * getscore(partie * p)
{
	return p->scores;
}
	
	
