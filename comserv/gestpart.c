#include <stdio.h>
#include <stdlib.h>
//#include <protoserv.h>
#include "gestpart.h"

// pour mes tests
// gcc -Wall -o gestpart gestpart.c 

typedef struct partie partie; 

struct partie {
	int nbjoueurs; 
	int nbcoup;
	int nbcoupmax; // la partie s'arrête lorsque le nombre de coups maximal a été atteint
	int l; // largeur 
	int h; // hauteur
	char **map;
	int *score; 
	int xd; // abscisse du dernier coup joué
	int yd; // ordonnée du dernier coup joué
	int *canplay; // canplay[i] vaut 0 si le joueur i+1 ne peut plus jouer (ne peut plus aligner 5 croix), 1 sinon
};

// ##########################################################################################################################
// init initialise la partie
// retourne la partie initialisée
partie *init(int l, int h, int nbjoueurs, int nbcoupmax)
{
	if (l < 5)
	{
		fprintf(stderr, "largeur %d non valide\n", l);
		exit(0);
	} 
	if (h < 5)
	{
		fprintf(stderr, "hauteur %d non valide\n", h);
		exit(0);
	} 
	if ((nbjoueurs < 1) || (nbjoueurs > 9))
	{
		fprintf(stderr, "nbjoueurs %d non valide\n", nbjoueurs);
		exit(0);
	} 

	int i, j, nbcoup;
	nbcoup = 0;
	partie *p = (partie*) malloc(sizeof(struct partie));
	char ** map = (char**) malloc(l * sizeof(char*));
	int * score = (int*) malloc(nbjoueurs * sizeof(int));
	int * canplay = (int *) malloc(nbjoueurs * sizeof(int));
	for (i=0 ; i < l ; i++)
	{
		map[i] = (char*) malloc(h*sizeof(char));
	}
	for (i=0 ; i < l ; i++)
	{
		for (j=0 ; j < h ; j++)
		{
			map[i][j] = 'O';
		}
	}
	for (i=0 ; i < nbjoueurs ; i++)
	{
		score[i] = 0;
	}
	for (i=0 ; i < nbjoueurs ; i++)
	{
		canplay[i] = 1;
	}
	p->nbcoup = nbcoup;
	p->nbcoupmax = nbcoupmax;
	p->map = map;
	p->score = score;
	p->canplay = canplay;
	p->nbjoueurs = nbjoueurs;
	p->l = l;
	p->h = h; 
	p->xd = -1;
	p->yd = -1;
	return p;
}

// ##########################################################################################################################
// calcul_score met à jour le score des joueurs
// score[joueur] = nombre de cases portant le numéro de ce joueur

void calcul_score(partie * p, int joueur)
{
	char s[1];
	int i, j;
	sprintf(s, "%d", joueur);
	p->score[joueur-1] = 0;
	for (j = 0 ; j < p->h ; j++)
	{
		for (i = 0 ; i < p->l ; i++)
		{
			if (p->map[i][j] == *s)
			{
				p->score[joueur-1]++;
			}
		}
	} 
}

// ##########################################################################################################################
// alignement5 détecte les alignements de 5 ou plus autour de la case (x, y) et met à jour la map avec le numero du joueur 
// et le score du joueur si alignement de 5 ou plus
// retourne 
// 0 si alignement de 5 autour de la case (x, y) et met à jour la map avec le numero du joueur
// 1 sinon

int alignement5(partie *p, int x, int y, int joueur)
{
	int largeur, hauteur;
	int i, j;
	largeur = p->l;
	hauteur = p->h;
	char **map = p->map;
	int ret;
	char s[1];
	ret = sprintf(s, "%d", joueur);
	if (ret < 0)
	{
		fprintf(stderr, "erreur de conversion int -> string.\n");
	}

	/* alignement sur une ligne */
	int compteur_ligne, compteur_ligne_droit, compteur_ligne_gauche;
	compteur_ligne = 0;
	compteur_ligne_droit = 0;
	compteur_ligne_gauche = 0;
	i = 0;
	j = -1;
	// on compte le nombre de cases marquées à droite de la case (x, y), case (x, y) incluse
	while ((compteur_ligne_droit != 1) && (((x+i) >= 0) && ((x+i) < largeur) && ((map[x+i][y] == 'X')||(map[x+i][y] == *s))))
	{
		compteur_ligne++;
		if ((x+i+1 >= 0) && (x+i+1 < largeur) && (map[x+i][y] == *s) && (map[x+i+1][y] == *s))
		{
			// on ne prend pas en compte une case portant le numéro du joueur si elle est "précédée" d'une case portant le numéro du joueur
			compteur_ligne_droit = 1;
		}
		i++;
	}
	// on compte le nombre de cases marquées à gauche de la case (x, y)
	while ((compteur_ligne_gauche != 1) && (((x+j) >= 0) && ((x+j) < largeur) && ((map[x+j][y] == 'X') || (map[x+j][y] == *s))))
	{
		compteur_ligne++;
		if ((x+j-1 >= 0) && (x+j-1 < largeur) && (map[x+j][y] == *s) && (map[x+j-1][y] == *s))
		{

			compteur_ligne_gauche = 1;
		}		
		j--;
	}

	/* alignement sur une colonne */
	int compteur_col, compteur_col_haut, compteur_col_bas;
	compteur_col = 0;
	compteur_col_haut = 0;
	compteur_col_bas = 0;
	i=0;
	j=-1;
	// on compte le nombre de cases marquées en dessous de la case (x, y), case (x, y) incluse
	while ((compteur_col_bas !=1) && ((y+i >=0) && (y+i < hauteur) && ((map[x][y+i] == 'X') || (map[x][y+i] == *s))))
	{
		compteur_col++;
		if ((y+i+1 >= 0) && (y+i+1 < hauteur) && (map[x][y+i] == *s) && (map[x][y+i+1] == *s))
		{
			// 
			compteur_col_bas = 1;
		}
		i++;
	}
	// on compte le nombre de cases marquées au dessus de la case (x, y)
	while ((compteur_col_haut != 1) && ((y+j >=0) && (y+j < hauteur) && ((map[x][y+j] == 'X') || (map[x][y+j] == *s))))
	{
		compteur_col++;
		if ((y+j-1 >= 0) && (y+j-1 < hauteur) && (map[x][y+j] == *s) && (map[x][y+j-1] == *s))
		{
			// 
			compteur_col_haut = 1;
		}
		j--;
	}

	
	/* alignement sur une diagonale */
	int compteur_diag, compteur_diag_haut, compteur_diag_bas;
	compteur_diag = 0;
	compteur_diag_haut = 0;
	compteur_diag_bas = 0;
	i = 0;
	j = -1;
	while ((compteur_diag_bas != 1) && ((x+i >= 0) && (x+i < largeur) && (y+i >=0) && (y+i < hauteur) && ((map[x+i][y+i] == 'X') || (map[x+i][y+i] == *s))))
	{
		compteur_diag++;
		if ((x+i+1 >= 0) && (x+i+1 < largeur) && (y+i+1 >= 0) && (y+i+1 < hauteur) && (map[x+i][y+i] == *s) && (map[x+i+1][y+i+1] == *s))
		{
			// 
			compteur_diag_bas = 1;
		}
		i++;
	}
	while ((compteur_diag_haut != 1) && ((x+j >= 0) && (x+j < largeur) && (y+j >=0) && (y+j < hauteur) && ((map[x+j][y+j] == 'X') || (map[x+j][y+j] == *s))))
	{
		compteur_diag++;
		if ((x+j-1 >= 0) && (x+j-1 < largeur) && (y+j-1 >= 0) && (y+j-1 < hauteur) && (map[x+j][y+j] == *s) && (map[x+j-1][y+j-1] == *s))
		{
			// 
			compteur_diag_haut = 1;
		}
		j--;
	}


	/* alignement sur une antidiagonale */
	int compteur_antidiag, compteur_antidiag_haut, compteur_antidiag_bas;
	compteur_antidiag = 0;
	compteur_antidiag_haut = 0;
	compteur_antidiag_bas = 0;
	i = 0;
	j = -1;
	while ((compteur_antidiag_bas != 1) && ((x-i >= 0) && (x-i < largeur) && (y+i >=0) && (y+i < hauteur) && ((map[x-i][y+i] == 'X') || (map[x-i][y+i] == *s))))
	{
		compteur_antidiag++;
		if ((x-(i+1) >= 0) && (x-(i+1) < largeur) && (y+i+1 >= 0) && (y+i+1 < hauteur) && (map[x-i][y+i] == *s) && (map[x-(i+1)][y+i+1] == *s))
		{
			// 
			compteur_antidiag_bas = 1;
		}
		i++;
	}

	while ((compteur_antidiag_haut != 1) && ((x-j >= 0) && (x-j < largeur) && (y+j >=0) && (y+j < hauteur) && ((map[x-j][y+j] == 'X') || (map[x-j][y+j] == *s))))
	{
		compteur_antidiag++;
		if ((x-(j-1) >= 0) && (x-(j-1) < largeur) && (y+j-1 >= 0) && (y+j-1 < hauteur) && (map[x-j][y+j] == *s) && (map[x-(j-1)][y+j-1] == *s))
		{
			// 
			compteur_antidiag_haut = 1;
		}
		j--;
	}
	
	/* mise à jour des map */

	// sur les lignes
	if (compteur_ligne >= 5)
	{
		i = 0;
		j = -1;
		while (((x+i) >= 0) && ((x+i) < largeur) && ((map[x+i][y] == 'X') || (map[x+i][y] == *s)))
		{
			map[x+i][y] = *s;
			i++;
		}
		while (((x+j) >= 0) && ((x+j) < largeur) && ((map[x+j][y] == 'X') || (map[x+j][y] == *s)))
		{
			map[x+j][y] = *s;
			j--;
		}
	}    

	// sur une colonne2
	if (compteur_col >= 5)
	{
		i=0;
		j = -1;
		while ((y+i >=0) && (y+i < hauteur) && ((map[x][y+i] == 'X') || (map[x][y+i] == *s)))
		{
			map[x][y+i] = *s;
			i++;
		}  
		while ((y+j >=0) && (y+j < hauteur) && ((map[x][y+j] == 'X') || (map[x][y+j] == *s)))
		{
			map[x][y+j] = *s;
			j--;
		}
	}

	// sur une diagonale
	if (compteur_diag >= 5)
	{
		i = 0;
		j = -1;
		while ((x+i >= 0) && (x+i < largeur) && (y+i >=0) && (y+i < hauteur) && ((map[x+i][y+i] == 'X') || (map[x+i][y+i] == *s)))
		{
			map[x+i][y+i] = *s;
			i++;
		}
		while ((x+j >= 0) && (x+j < largeur) && (y+j >=0) && (y+j < hauteur) && ((map[x+j][y+j] == 'X') || (map[x+j][y+j] == *s)))
		{
			map[x+j][y+j] = *s;
			j--;
		} 
	}

	// sur une antidiagonale
	if (compteur_antidiag >= 5)
	{
		i = 0;
		j = -1;
		while ((x-i >= 0) && (x-i < largeur) && (y+i >=0) && (y+i < hauteur) && ((map[x-i][y+i] == 'X') || (map[x-i][y+i] == *s)))
		{
			map[x-i][y+i] = *s;
			i++;
		}
		while ((x-j >= 0) && (x-j < largeur) && (y+j >=0) && (y+j < hauteur) && ((map[x-j][y+j] == 'X') || (map[x-j][y+j] == *s)))
		{
			map[x-j][y+j] = *s;
			j--;
		} 
	}
	
	if ((compteur_ligne >= 5) || (compteur_col >= 5) || (compteur_diag >=5) || (compteur_antidiag >= 5))
	{
		return 0;
	}

	/* aucun alignement */
	return 1;
}


// ##########################################################################################################################
// alignement5possible détecte les alignements de 5 ou plus possibles autour de la case (x, y) pour un joueur donné
// retourne 
// 0 si alignement de 5 possible autour de la case (x, y) 
// 1 sinon

int alignement5possible(partie *p, int x, int y, int joueur){
	int largeur, hauteur;
	char s[1];
	int i, j;
	char **map;
	largeur = p->l;
	hauteur = p->h;
	map = p->map;
	sprintf(s, "%d", joueur);

	/* alignement possible sur une ligne ? */
	int compteur_ligne;
	compteur_ligne = 0;
	i = 0;
	j = -1;
	while (((x+i) >= 0) && ((x+i) < largeur) && ((map[x+i][y] == 'X') || (map[x+i][y] == 'O') || (map[x+i][y] == *s)))
	{
		compteur_ligne++;
		i++;
	}
	while (((x+j) >= 0) && ((x+j) < largeur) && ((map[x+j][y] == 'X') || (map[x+j][y] == 'O') || (map[x+j][y] == *s)))
	{
		compteur_ligne++;
		j--;
	}
	//printf("          compteur ligne : %d\n", compteur_ligne);
	if (compteur_ligne >= 5)
	{
		return 0;
	}  

	/* alignement possible sur une colonne ? */
	int compteur_col;
	compteur_col = 0;
	i = 0;
	j = -1;
	while ((y+i >=0) && (y+i < hauteur) && ((map[x][y+i] == 'X') || (map[x][y+i] == 'O') || (map[x][y+i] == *s)))
	{
		compteur_col++;
		i++;
	}
	while ((y+j >=0) && (y+j < hauteur) && ((map[x][y+j] == 'X') || (map[x][y+j] == 'O') ||  (map[x][y+j] == *s)))
	{
		compteur_col++;
		j--;
	}
	if (compteur_col >= 5)
	{
		return 0;
	}

	/* alignement possible sur une diagonale ? */
	int compteur_diag;
	compteur_diag = 0;
	i = 0;
	j = -1;
	while ((x+i >= 0) && (x+i < largeur) && (y+i >=0) && (y+i < hauteur) && ((map[x+i][y+i] == 'X') || (map[x+i][y+i] == 'O') || (map[x+i][y+i] == *s)))
	{
		compteur_diag++;
		i++;
	}
	while ((x+j >= 0) && (x+j < largeur) && (y+j >=0) && (y+j < hauteur) && ((map[x+j][y+j] == 'X') || (map[x+j][y+j] == 'O') || (map[x+j][y+j] == *s)))
	{
		compteur_diag++;
		j--;
	}
	if (compteur_diag >= 5)
	{
		return 0;
	}

	/* alignement possible sur une antidiagonale ? */
	int compteur_antidiag;
	compteur_antidiag = 0;
	i = 0;
	j = -1;
	while ((x-i >= 0) && (x-i < largeur) && (y+i >=0) && (y+i < hauteur) && ((map[x-i][y+i] == 'X') || (map[x-i][y+i] == 'O') || (map[x-i][y+i] == *s)))
	{
		compteur_antidiag++;
		i++;
	}
	while ((x-j >= 0) && (x-j < largeur) && (y+j >=0) && (y+j < hauteur) && ((map[x-j][y+j] == 'X') || (map[x-j][y+j] == 'O') ||  (map[x-j][y+j] == *s)))
	{
		compteur_antidiag++;
		j--;
	}
	if (compteur_antidiag >= 5)
	{
		return 0;
	}

	/* aucun alignement possible */
	return 1;
}

// #############################################################################################################
// is_fin_du_jeu 
// retourne 1 si aucun alignement de 5 est possible
// retourne 0 sinon

int is_fin_de_partie(partie *p){
	int largeur, hauteur;
	int i, j, k;
	largeur = p->l;
	hauteur = p->h;

	/* on parcourt la map : pour chaque case et pour chaque joueur, on teste si ce joueur peut aligner au moins 5 croix */
	for (i=0 ; i < hauteur ; i++)
	{
		for (j=0 ; j< largeur  ; j++)
		{
			for (k = 1 ; k <= p->nbjoueurs ; k++)
			{
				if (alignement5possible(p, j, i, k) == 0)
				{
					// le joueur k peut aligner 5 croix autour de la case (i, j)
					// mise à jour de canplay
					p->canplay[k-1] = 1;
				}
				else 
				{
					// le joueur k ne peut pas aligner 5 croix autour de la case (i, j)
					// mise à jour de canplay
					p->canplay[k-1] = 0;
				}
			}
		}
	}

	/* on parcourt canplay pour vérifier si au moins un joueur peut encore jouer */
	int compteur;
	compteur = 0;
	for (k = 0; k < p->nbjoueurs ; k++)
	{
		if (p->canplay[k] == 1)
		{
			compteur++;
		}
	}
	if ((compteur == 0) || (p->nbcoup >= p->nbcoupmax))
	{ 
		// c'est la fin de la partie
		return 1;
	}
	else 
	{
		// ce n'est pas la fin de la partie
		return 0;
	}  
}

// ###############################################################################################################
// joueur_suivant retourne le numéro du joueur suivant qui a encore la possibilité de réaliser un alignement de 5
// retourne 0 si aucun joueur ne peut aligner 5 croix (-> fin de la partie)

int joueur_suivant(partie *p, int joueur){
	int i;
	/* SI dernier joueur du tableau */
	// on regarde le tableau à partir du début
	if (joueur == p->nbjoueurs)
	{
		for (i = 0 ; i < p->nbjoueurs ; i++)
		{ 
			if (p->canplay[i] == 1)
			{
				return (i+1);
			}
		}   
	}
	/* SINON */
	// on regarde le tableau à partir du joueur joueur+1 (donc canplay[joueur])
	for (i = joueur ; i < p->nbjoueurs ; i++)
	{
		if (p->canplay[i] == 1)
		{
			return (i+1);
		}
	}
	// si on est arrivé à la fin du tableau et qu'aucun joueur ne peut encore jouer, on revient au début
	for (i = 0 ; i < joueur ; i++)
	{
		if (p->canplay[i] == 1)
		{
			return (i+1);
		}
	}
	/* fin de partie */
	return 0;
}

// ###############################################################################################################
// play met à jour la map et le score si coup correct et 
// retourne
// -1 si coup incorrect
// 0 si partie terminee 
// numero du joueur suivant si coup correct 

int play(partie *p, int x, int y, int joueur)
{
	int largeur, hauteur;
	largeur = p->l;
	hauteur = p->h;

	/* coordonnees non valides */
	if ((x<0) || (x >= largeur) || (y<0) || (y >= hauteur))
	{
		printf("           joueur %d : coup (%d, %d) non valide\n", joueur, x, y);     
		return (-1);
	}

	/* case déjà marquée */
	if (p->map[x][y] != 'O')
	{
		printf("          joueur %d : case (%d, %d) marquee\n", joueur, x, y);
		return (-1);
	}

	/* case vide */
	else 
	{
		// on marque la case 
		p->map[x][y] = 'X';
		// mise à jour du nombre de coups
		p->nbcoup++;
		// on met à jour les coordonnées du dernier coup
		p->xd = x;
		p->yd = y;
		printf("          le joueur %d a joué le coup (%d, %d).\n", joueur, x, y);
		if (alignement5(p, x, y, joueur) == 0) // mise a jour de la map 
		{
			// mise a jour du score
			calcul_score(p, joueur);
			printf("          joueur %d : alignement de 5 : (%d, %d)\n", joueur, x, y); 
		}

		if (is_fin_de_partie(p) == 1)
		{
			printf("          fin de la partie !\n");
			return 0;
		}
		int suivant;
		suivant = joueur_suivant(p, joueur);
		printf("          C'est au joueur %d de jouer !\n", suivant);
		return suivant;
	}
} 

// ####################################################################################
// destroy libère la mémoire
void destroy(partie *p){
	int i;
	for (i=0 ; i < p->l ; i++)
	{
		free(p->map[i]);
	}
	free(p->map);
	free(p->score);
	free(p->canplay);
	free(p); 
}

// ####################################################################################
// getmap retourne la map sous la forme d'un char*
// la dernière case marquée est remplacée par le char 'D'
char * getmap(partie *p){
	int i, j, k;
	char *s = (char*) calloc(((p->l)*(p->h))+1, sizeof(char));
	k = 0;
	for (j=0 ; j < p->h ; j++)
	{
		for (i=0 ; i < p->l ; i++)
		{
			if (((i == p->xd) && (j == p->yd)) && ((p->map[i][j] == 'O') || (p->map[i][j] == 'X')))
			{
				s[k] = 'D';
			}
			else 
			{
				s[k] = p->map[i][j];
			}
			k++;
		}
	}
	s[k] = '\0';
	return s;
}

// ###################################################################################
// getscore retourne le tableau des scores
int getscore(partie *p, int joueur)
{
	return p->score[joueur-1];
}

// ###################################################################################
// getvainqueur retourne le numéro du joueur qui a remporté la partie
int getvainqueur(partie *p)
{
	int max, vainqueur, i;
	max = p->score[0];
	vainqueur =1;
	for (i=0 ; i < (p->nbjoueurs) ; i++)
	{
		if (p->score[i] > max)
		{
			max = p->score[i];
			vainqueur = i+1;
		} 
	}
	return vainqueur;
}

// ############################# fonctions affichage #################################
/*
void affiche_map_bis(char ** map, int l, int h){
  int i, j;
  printf("map : \n");  
  for (j=0 ; j< h; j++){
    for (i=0 ; i< l ; i++){
      printf("%c", map[i][j]);     
    }
    printf("\n");
  }
  printf("\n");

}
*/

void affiche_map(char * map, int l, int h)
{
	int i, j;
	printf("map : \n");  
	for (j = 0 ; j < h ; j++)
	{
		for (i=0 ; i< l; i++)
		{
			printf("%c", map[i+j*l]);
		}
		printf("\n");
	}
}


void affiche_score(int * score, int nbjoueurs)
{
	int i;
	printf("score :\n");
	for (i=0 ; i< nbjoueurs ; i++)
	{
		printf("joueur %d : %d\n", i+1, score[i]);
	}
	printf("\n");
}


// ########################### test des fonctions ##################################

int main(){
  
  partie *p;
  p = init(6, 7, 3, 40); 
  printf("largeur : %d\n", p->l);
  printf("hauteur : %d\n", p->h);
  printf("nombre de joueurs : %d\n", p->nbjoueurs); 
  
  
  affiche_map(getmap(p), p->l, p->h);
  affiche_score(p->score, p->nbjoueurs);

  /*
  printf(" ######################## \n");
  play(p, 0, 0, 1);
  play(p, 1, 1, 2);
  play(p, 3, 3, 3);
  affiche_map(getmap(p), p->l, p->h);
  play(p, 4, 4, 1);
  play(p, 2, 2, 2);

  printf(" ####### test ####### \n");
  //play(p, 5, 5, 3);
  play(p, 7, -1, 3);

  affiche_map(getmap(p), p->l, p->h);
  affiche_score(p->score, p->nbjoueurs);
  

  printf(" ######################## \n");
  play(p, 0, 5, 3);
  play(p, 1, 5, 1);
  play(p, 2, 5, 2);
  play(p, 3, 5, 3); 
  play(p, 4, 5, 1);

  affiche_map(getmap(p), p->l, p->h);
  affiche_score(p->score, p->nbjoueurs);

  printf(" ######################## \n");
  play(p, 0, 6, 2);
  play(p, 3, 6, 3); 
  play(p, 4, 6, 1);
  play(p, 5, 6, 2);
  affiche_map(getmap(p), p->l, p->h);
  play(p, 1, 6, 3);
  play(p, 2, 6, 2);
  affiche_map(getmap(p), p->l, p->h);
  affiche_score(p->score, p->nbjoueurs);
 
   
  printf(" ######################## \n");
  play(p, 5, 0, 1);
  play(p, 4, 1, 2);
  play(p, 2, 3, 3);
  play(p, 1, 4, 1);
  affiche_map(getmap(p), p->l, p->h);
  play(p, 3, 2, 2); 
  play(p, 4, 2, 2); 
  
  affiche_map(getmap(p), p->l, p->h);
  affiche_score(p->score, p->nbjoueurs);

  printf(" ######################## \n");
  play(p, 5, 1, 3);
  play(p, 5, 2, 1);
  play(p, 5, 3, 2);
  affiche_map(getmap(p), p->l, p->h);
  play(p, 5, 4, 3);
  play(p, 5, 3, 1); 
  play(p, 5, 5, 3);  
  
*/

// ################"

  play(p, 2, 0, 1);
  play(p, 2, 1, 2);
  play(p, 2, 3, 3);
  affiche_map(getmap(p), p->l, p->h);
  play(p, 2, 4, 1);
  play(p, 0, 2, 2); 
  play(p, 1, 2, 3);  
  play(p, 3, 2, 1);  
  play(p, 4, 2, 2);  
  play(p, 2, 2, 3);  
  play(p, 2, 5, 3);  
  play(p, 2, 6, 3); 
  play(p, 5, 2, 3);
   
  play(p, 5, 5, 1);  
  printf("la map est : %s\n", getmap(p));
  affiche_map(getmap(p), p->l, p->h);
  play(p, 0, 3, 3);  
  play(p, 1, 3, 3);  
  play(p, 3, 3, 3); 
  play(p, 4, 3, 3);
 
  play(p, 4, 4, 1); 
  play(p, 4, 5, 2); 
  play(p, 4, 6, 3);
/*
  play(p, 5, 1, 3);
  play(p, 5, 3, 1);
  play(p, 0, 5, 2);
  play(p, 5, 4, 3);
  play(p, 5, 2, 1);
  play(p, 5, 0, 2);
  printf("la map est : %s\n", getmap(p));
  affiche_map(getmap(p), p->l, p->h);

  play(p, 0, 6, 2);
  play(p, 1, 6, 3);
  play(p, 2, 6, 1);
  play(p, 3, 6, 2);
  play(p, 5, 6, 3);
  play(p, 4, 6, 1);
  printf("la map est : %s\n", getmap(p));
  affiche_map(getmap(p), p->l, p->h);

  play(p, 4, 3, 2);
  play(p, 4, 4, 3);
  play(p, 4, 5, 1);
  play(p, 4, 1, 3);
  play(p, 4, 0, 3);
  printf("la map est : %s\n", getmap(p));
  affiche_map(getmap(p), p->l, p->h);

  play(p, 3, 5, 1);
*/
  printf("la map est : %s\n", getmap(p));
  affiche_map(getmap(p), p->l, p->h);




  affiche_score(p->score, p->nbjoueurs);
  //printf("le vainqueur est le joueur %d avec un score de %d\n", getvainqueur(p), getscore(p, getvainqueur(p)));
 
  return 0;
}



