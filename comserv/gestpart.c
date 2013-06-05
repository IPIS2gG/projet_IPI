#include <stdio.h>
#include <stdlib.h>
//#include <protoserv.h>
#include "gestpart.h"

extern bool aff_debug;

// pour mes tests
// gcc -Wall -o gestpart gestpart.c 

typedef struct partie partie; 

struct partie {
	int nbjoueurs; // nombre de joueurs
	int nbcoup; // nombre de coups joués
	int nbcoupmax; // la partie s'arrête lorsque le nombre de coups maximal a été atteint
	int l; // largeur 
	int h; // hauteur
	char **map; // grille
	int *score; // tableau de scores
	int xd; // abscisse du dernier coup joué
	int yd; // ordonnée du dernier coup joué
	int *canplay; // canplay[i] vaut 0 si le joueur i+1 ne peut plus jouer (ne peut plus aligner 5 croix), >0 sinon
};

typedef struct Coordonnees Coordonnees;

struct Coordonnees {
	int x;
	int y;
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
	char s[1];
	sprintf(s, "%d", joueur);


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
			// si deux cases adjacentes portent le numéro du joueur, on n'en compte qu'une seule
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
			// si deux cases adjacentes portent le numéro du joueur, on n'en compte qu'une seule
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
	// on compte le nombre de cases marquées "en dessous" de la case (x, y)
	while ((compteur_diag_bas != 1) && ((x+i >= 0) && (x+i < largeur) && (y+i >=0) && (y+i < hauteur) && ((map[x+i][y+i] == 'X') || (map[x+i][y+i] == *s))))
	{
		compteur_diag++;
		if ((x+i+1 >= 0) && (x+i+1 < largeur) && (y+i+1 >= 0) && (y+i+1 < hauteur) && (map[x+i][y+i] == *s) && (map[x+i+1][y+i+1] == *s))
		{
			compteur_diag_bas = 1;
		}
		i++;
	}
	// on compte le nombre de cases marquées "au dessus" de la case (x, y)
	while ((compteur_diag_haut != 1) && ((x+j >= 0) && (x+j < largeur) && (y+j >=0) && (y+j < hauteur) && ((map[x+j][y+j] == 'X') || (map[x+j][y+j] == *s))))
	{
		compteur_diag++;
		if ((x+j-1 >= 0) && (x+j-1 < largeur) && (y+j-1 >= 0) && (y+j-1 < hauteur) && (map[x+j][y+j] == *s) && (map[x+j-1][y+j-1] == *s))
		{
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
	// on compte le nombre de cases marquées "en dessous" de la case (x, y)
	while ((compteur_antidiag_bas != 1) && ((x-i >= 0) && (x-i < largeur) && (y+i >=0) && (y+i < hauteur) && ((map[x-i][y+i] == 'X') || (map[x-i][y+i] == *s))))
	{
		compteur_antidiag++;
		if ((x-(i+1) >= 0) && (x-(i+1) < largeur) && (y+i+1 >= 0) && (y+i+1 < hauteur) && (map[x-i][y+i] == *s) && (map[x-(i+1)][y+i+1] == *s))
		{
			compteur_antidiag_bas = 1;
		}
		i++;
	}
	// on compte le nombre de cases marquées "au dessus" de la case (x, y)
	while ((compteur_antidiag_haut != 1) && ((x-j >= 0) && (x-j < largeur) && (y+j >=0) && (y+j < hauteur) && ((map[x-j][y+j] == 'X') || (map[x-j][y+j] == *s))))
	{
		compteur_antidiag++;
		if ((x-(j-1) >= 0) && (x-(j-1) < largeur) && (y+j-1 >= 0) && (y+j-1 < hauteur) && (map[x-j][y+j] == *s) && (map[x-(j-1)][y+j-1] == *s))
		{
			compteur_antidiag_haut = 1;
		}
		j--;
	}
	


	/* mise à jour des map */

	// maj : sur les lignes
	if (compteur_ligne >= 5)
	{
		compteur_ligne_droit = 0;
		compteur_ligne_gauche = 0;
		i = 0;
		j = -1;
		while ((compteur_ligne_droit != 1) && (((x+i) >= 0) && ((x+i) < largeur) && ((map[x+i][y] == 'X')||(map[x+i][y] == *s))))
		{
			if ((x+i+1 >= 0) && (x+i+1 < largeur) && (map[x+i][y] == *s) && (map[x+i+1][y] == *s))
			{
				compteur_ligne_droit = 1;
			}
			map[x+i][y] = *s;
			i++;
		}
		while ((compteur_ligne_gauche != 1) && (((x+j) >= 0) && ((x+j) < largeur) && ((map[x+j][y] == 'X') || (map[x+j][y] == *s))))
		{
			if ((x+j-1 >= 0) && (x+j-1 < largeur) && (map[x+j][y] == *s) && (map[x+j-1][y] == *s))
			{
				compteur_ligne_gauche = 1;
			}
			map[x+j][y] = *s;		
			j--;
		}
		//printf("alignement ligne %d %d\n", x, y);
	}    


	// maj : sur une colonne
	if (compteur_col >= 5)
	{
		compteur_col_haut = 0;
		compteur_col_bas = 0;
		i=0;
		j=-1;
		while ((compteur_col_bas !=1) && ((y+i >=0) && (y+i < hauteur) && ((map[x][y+i] == 'X') || (map[x][y+i] == *s))))
		{
			if ((y+i+1 >= 0) && (y+i+1 < hauteur) && (map[x][y+i] == *s) && (map[x][y+i+1] == *s))
			{
				compteur_col_bas = 1;
			}
			map[x][y+i] = *s;
			i++;
		}
		while ((compteur_col_haut != 1) && ((y+j >=0) && (y+j < hauteur) && ((map[x][y+j] == 'X') || (map[x][y+j] == *s))))
		{
			if ((y+j-1 >= 0) && (y+j-1 < hauteur) && (map[x][y+j] == *s) && (map[x][y+j-1] == *s))
			{
				// 
				compteur_col_haut = 1;
			}
			map[x][y+j] = *s;
			j--;
		}
		//printf("alignement col %d %d\n", x, y);
	}


	// maj : sur une diagonale
	if (compteur_diag >= 5)
	{
		compteur_diag_haut = 0;
		compteur_diag_bas = 0;
		i = 0;
		j = -1;
		while ((compteur_diag_bas != 1) && ((x+i >= 0) && (x+i < largeur) && (y+i >=0) && (y+i < hauteur) && ((map[x+i][y+i] == 'X') || (map[x+i][y+i] == *s))))
		{
			if ((x+i+1 >= 0) && (x+i+1 < largeur) && (y+i+1 >= 0) && (y+i+1 < hauteur) && (map[x+i][y+i] == *s) && (map[x+i+1][y+i+1] == *s))
			{
				compteur_diag_bas = 1;
			}
			map[x+i][y+i] = *s;
			i++;
		}
		while ((compteur_diag_haut != 1) && ((x+j >= 0) && (x+j < largeur) && (y+j >=0) && (y+j < hauteur) && ((map[x+j][y+j] == 'X') || (map[x+j][y+j] == *s))))
		{
			if ((x+j-1 >= 0) && (x+j-1 < largeur) && (y+j-1 >= 0) && (y+j-1 < hauteur) && (map[x+j][y+j] == *s) && (map[x+j-1][y+j-1] == *s))
			{
				compteur_diag_haut = 1;
			}
			map[x+j][y+j] = *s;
			j--;
		}
		//printf("alignement diag %d %d\n", x, y);
	}


	// maj : sur une antidiagonale
	if (compteur_antidiag >= 5)
	{
		compteur_antidiag_haut = 0;
		compteur_antidiag_bas = 0;
		i = 0;
		j = -1;
		while ((compteur_antidiag_bas != 1) && ((x-i >= 0) && (x-i < largeur) && (y+i >=0) && (y+i < hauteur) && ((map[x-i][y+i] == 'X') || (map[x-i][y+i] == *s))))
		{
			if ((x-(i+1) >= 0) && (x-(i+1) < largeur) && (y+i+1 >= 0) && (y+i+1 < hauteur) && (map[x-i][y+i] == *s) && (map[x-(i+1)][y+i+1] == *s))
			{
				compteur_antidiag_bas = 1;
			}
			map[x-i][y+i] = *s;
			i++;
		}
		while ((compteur_antidiag_haut != 1) && ((x-j >= 0) && (x-j < largeur) && (y+j >=0) && (y+j < hauteur) && ((map[x-j][y+j] == 'X') || (map[x-j][y+j] == *s))))
		{
			if ((x-(j-1) >= 0) && (x-(j-1) < largeur) && (y+j-1 >= 0) && (y+j-1 < hauteur) && (map[x-j][y+j] == *s) && (map[x-(j-1)][y+j-1] == *s))
			{
				compteur_antidiag_haut = 1;
			}
			map[x-j][y+j] = *s;
			j--;
		}
		//printf("alignement antidiag %d %d\n", x, y);
	}
	

	/* au moins un alignement */
	if ((compteur_ligne >= 5) || (compteur_col >= 5) || (compteur_diag >=5) || (compteur_antidiag >= 5))
	{
		return 0;
	}


	/* aucun alignement */
	return 1;
}



// ##########################################################################################################################
// alignement_ligne retourne
// 0 s'il y a un alignement contenant la case (x, y) sur la ligne y, réalisé par le joueur
// 1 sinon

int alignement_ligne(partie *p, int x, int y, int joueur)
{
	int largeur, i, j;
	largeur = p->l;
	char **map = p->map;
	char s[1];
	sprintf(s, "%d", joueur);

	/* alignement sur une ligne */
	int compteur_ligne;
	compteur_ligne = 0;
	i = 0;
	j = -1;
	// on compte le nombre de cases marquées par le numéro du joueur à droite de la case (x, y), case (x, y) incluse
	while (((x+i) >= 0) && ((x+i) < largeur) && (map[x+i][y] == *s))
	{
		compteur_ligne++;
		i++;
	}
	// on compte le nombre de cases marquées par le numéro du joueur à gauche de la case (x, y)
	while (((x+j) >= 0) && ((x+j) < largeur) && (map[x+j][y] == *s))
	{
		compteur_ligne++;
		j--;
	}
	if (compteur_ligne >= 5)
	{
		// il y a un alignement contenant la case (x, y) sur la ligne y
		return 0;
	}
	return 1;
}



// ##########################################################################################################################
// alignement_colonne retourne
// 0 s'il y a un alignement contenant la case (x, y) sur la colonne x, réalisé par le joueur
// 1 sinon

int alignement_col(partie *p, int x, int y, int joueur)
{
	int hauteur;
	int i, j;
	hauteur = p->h;
	char **map = p->map;
	char s[1];
	sprintf(s, "%d", joueur);

	/* alignement sur une colonne */
	int compteur_col;
	compteur_col = 0;
	i=0;
	j=-1;
	// on compte le nombre de cases marquées par le numéro du joueur en dessous de la case (x, y), case (x, y) incluse
	while ((y+i >=0) && (y+i < hauteur) && (map[x][y+i] == *s))
	{
		compteur_col++;
		i++;
	}
	// on compte le nombre de cases marquées par le numéro du joueur au dessus de la case (x, y)
	while ((y+j >=0) && (y+j < hauteur) && (map[x][y+j] == *s))
	{
		compteur_col++;
		j--;
	}
	if (compteur_col >= 5)
	{
		// il y a un alignement contenant la case (x, y) sur la colonne x
		return 0;
	}
	return 1;	
}



// ##########################################################################################################################
// alignement_diag retourne
// 0 s'il y a un alignement contenant la case (x, y) sur la diagonale, réalisé par le joueur
// 1 sinon

int alignement_diag(partie *p, int x, int y, int joueur)
{
	int largeur, hauteur;
	int i, j;
	largeur = p->l;
	hauteur = p->h;
	char **map = p->map;
	char s[1];
	sprintf(s, "%d", joueur);

	/* alignement sur une diagonale */
	int compteur_diag;
	compteur_diag = 0;
	i = 0;
	j = -1;
	while ((x+i >= 0) && (x+i < largeur) && (y+i >=0) && (y+i < hauteur) && (map[x+i][y+i] == *s))
	{
		compteur_diag++;
		i++;
	}
	while ((x+j >= 0) && (x+j < largeur) && (y+j >=0) && (y+j < hauteur) && (map[x+j][y+j] == *s))
	{
		compteur_diag++;
		j--;
	}
	if (compteur_diag >= 5)
	{
		// il y a un alignement contenant la case (x, y) sur la diagonale 
		return 0;
	}
	return 1;
}



// ##########################################################################################################################
// alignement_antidiag retourne
// 0 s'il y a un alignement contenant la case (x, y) sur l'antidiagonale, réalisé par le joueur
// 1 sinon

int alignement_antidiag(partie *p, int x, int y, int joueur)
{
	int largeur, hauteur;
	int i, j;
	largeur = p->l;
	hauteur = p->h;
	char **map = p->map;
	char s[1];
	sprintf(s, "%d", joueur);

	/* alignement sur une antidiagonale */
	int compteur_antidiag;
	compteur_antidiag = 0;
	i = 0;
	j = -1;
	while ((x-i >= 0) && (x-i < largeur) && (y+i >=0) && (y+i < hauteur) && (map[x-i][y+i] == *s))
	{
		compteur_antidiag++;
		i++;
	}

	while ((x-j >= 0) && (x-j < largeur) && (y+j >=0) && (y+j < hauteur) && (map[x-j][y+j] == *s))
	{
		compteur_antidiag++;
		j--;
	}
	if (compteur_antidiag >= 5)
	{
		// il y a un alignement contenant la case (x, y) sur l'antidiagonale 
		return 0;
	}
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



	/* alignement possible sur une ligne ??? */
	int compteur_ligne, compteur_ligne_droit, compteur_ligne_gauche;
	compteur_ligne = 0;
	compteur_ligne_droit = 0;
	compteur_ligne_gauche = 0;
	i = 0;
	j = -1;
	// s'il n'y a pas déjà un alignement contenant la case (x, y) sur la ligne
	if (alignement_ligne(p, x, y, joueur) == 1)
	{
		// on compte le nombre de cases à droite de la case (x, y) qui peuvent éventuellement faire partie d'un alignement de 5
		while ((compteur_ligne_droit != 1) && (((x+i) >= 0) && ((x+i) < largeur) && ((map[x+i][y] == 'X')||(map[x+i][y] == 'O')||(map[x+i][y] == *s))))
		{
			compteur_ligne++;
			if ((x+i+1 >= 0) && (x+i+1 < largeur) && (map[x+i][y] == *s) && (map[x+i+1][y] == *s))
			{
				// si deux cases adjacentes portent le numéro du joueur, on n'en compte qu'une seule
				compteur_ligne_droit = 1;
			}
			i++;
		}
		// on compte le nombre de cases à gauche de la case (x, y) qui peuvent éventuellement faire partie d'un alignement de 5
		while ((compteur_ligne_gauche != 1) && (((x+j) >= 0) && ((x+j) < largeur) && ((map[x+j][y] == 'X') || (map[x+j][y] == 'O') || (map[x+j][y] == *s))))
		{
			compteur_ligne++;
			if ((x+j-1 >= 0) && (x+j-1 < largeur) && (map[x+j][y] == *s) && (map[x+j-1][y] == *s))
			{
				// si deux cases adjacentes portent le numéro du joueur, on n'en compte qu'une seule
				compteur_ligne_gauche = 1;
			}		
			j--;
		}
		if (compteur_ligne >= 5)
		{
			// un alignement de 5 peut être réalisé par le joueur
			return 0;
		}  
	}


	/* alignement possible sur une colonne ??? */ 
	int compteur_col, compteur_col_haut, compteur_col_bas;
	compteur_col = 0;
	compteur_col_haut = 0;
	compteur_col_bas = 0;
	i=0;
	j=-1;
	// s'il n'y a pas déjà un alignement contenant la case (x, y) sur la colonne
	if (alignement_col(p, x, y, joueur) == 1)
	{
		// on compte le nombre de cases "en dessous" de la case (x, y) qui peuvent éventuellement faire partie d'un alignement de 5
		while ((compteur_col_bas !=1) && ((y+i >=0) && (y+i < hauteur) && ((map[x][y+i] == 'X') || (map[x][y+i] == 'O') || (map[x][y+i] == *s))))
		{
			compteur_col++;
			if ((y+i+1 >= 0) && (y+i+1 < hauteur) && (map[x][y+i] == *s) && (map[x][y+i+1] == *s))
			{
				// si deux cases adjacentes portent le numéro du joueur, on n'en compte qu'une seule
				compteur_col_bas = 1;
			}
			i++;
		}
		// on compte le nombre de cases "au dessus" de la case (x, y) qui peuvent éventuellement faire partie d'un alignement de 5
		while ((compteur_col_haut != 1) && ((y+j >=0) && (y+j < hauteur) && ((map[x][y+j] == 'X') || (map[x][y+j] == 'O') || (map[x][y+j] == *s))))
		{
			compteur_col++;
			if ((y+j-1 >= 0) && (y+j-1 < hauteur) && (map[x][y+j] == *s) && (map[x][y+j-1] == *s))
			{
				// si deux cases adjacentes portent le numéro du joueur, on n'en compte qu'une seule
				compteur_col_haut = 1;
			}
			j--;
		}
		if (compteur_col >= 5)
		{
			// un alignement de 5 peut être réalisé par le joueur
			return 0;
		}
	}


	/* alignement possible sur une diagonale ??? */
	int compteur_diag, compteur_diag_haut, compteur_diag_bas;
	compteur_diag = 0;
	compteur_diag_haut = 0;
	compteur_diag_bas = 0;
	i = 0;
	j = -1;
	// s'il n'y a pas déjà un alignement contenant la case (x, y) sur la diagonale
	if (alignement_diag(p, x, y, joueur) == 1)
	{
		// on compte le nombre de cases "en dessous" de la case (x, y) qui peuvent éventuellement faire partie d'un alignement de 5
		while ((compteur_diag_bas != 1) && ((x+i >= 0) && (x+i < largeur) && (y+i >=0) && (y+i < hauteur) && ((map[x+i][y+i] == 'X') || (map[x+i][y+i] == 'O') || (map[x+i][y+i] == *s))))
		{
			compteur_diag++;
			if ((x+i+1 >= 0) && (x+i+1 < largeur) && (y+i+1 >= 0) && (y+i+1 < hauteur) && (map[x+i][y+i] == *s) && (map[x+i+1][y+i+1] == *s))
			{
				// si deux cases adjacentes portent le numéro du joueur, on n'en compte qu'une seule
				compteur_diag_bas = 1;
			}
			i++;
		}
		// on compte le nombre de cases "au dessus" de la case (x, y) qui peuvent éventuellement faire partie d'un alignement de 5
		while ((compteur_diag_haut != 1) && ((x+j >= 0) && (x+j < largeur) && (y+j >=0) && (y+j < hauteur) && ((map[x+j][y+j] == 'X') || (map[x+j][y+j] == 'O') || (map[x+j][y+j] == *s))))
		{
			compteur_diag++;
			if ((x+j-1 >= 0) && (x+j-1 < largeur) && (y+j-1 >= 0) && (y+j-1 < hauteur) && (map[x+j][y+j] == *s) && (map[x+j-1][y+j-1] == *s))
			{
				// si deux cases adjacentes portent le numéro du joueur, on n'en compte qu'une seule
				compteur_diag_haut = 1;
			}
			j--;
		}
		if (compteur_diag >= 5)
		{
			// un alignement de 5 peut être réalisé par le joueur
			return 0;
		}
	}


	/* alignement possible sur une antidiagonale ??? */
	int compteur_antidiag, compteur_antidiag_haut, compteur_antidiag_bas;
	compteur_antidiag = 0;
	compteur_antidiag_haut = 0;
	compteur_antidiag_bas = 0;
	i = 0;
	j = -1;
	// s'il n'y a pas déjà un alignement contenant la case (x, y) sur l'antidiagonale
	if (alignement_antidiag(p, x, y, joueur) == 1)
	{
		// on compte le nombre de cases "en dessous" de la case (x, y) qui peuvent éventuellement faire partie d'un alignement de 5
		while ((compteur_antidiag_bas != 1) && ((x-i >= 0) && (x-i < largeur) && (y+i >=0) && (y+i < hauteur) && ((map[x-i][y+i] == 'X') || (map[x-i][y+i] == 'O') || (map[x-i][y+i] == *s))))
		{
			compteur_antidiag++;
			if ((x-(i+1) >= 0) && (x-(i+1) < largeur) && (y+i+1 >= 0) && (y+i+1 < hauteur) && (map[x-i][y+i] == *s) && (map[x-(i+1)][y+i+1] == *s))
			{
				// si deux cases adjacentes portent le numéro du joueur, on n'en compte qu'une seule
				compteur_antidiag_bas = 1;
			}
			i++;
		}
		// on compte le nombre de cases "au dessus" de la case (x, y) qui peuvent éventuellement faire partie d'un alignement de 5
		while ((compteur_antidiag_haut != 1) && ((x-j >= 0) && (x-j < largeur) && (y+j >=0) && (y+j < hauteur) && ((map[x-j][y+j] == 'X') || (map[x-j][y+j] == 'O') || (map[x-j][y+j] == *s))))
		{
			compteur_antidiag++;
			if ((x-(j-1) >= 0) && (x-(j-1) < largeur) && (y+j-1 >= 0) && (y+j-1 < hauteur) && (map[x-j][y+j] == *s) && (map[x-(j-1)][y+j-1] == *s))
			{
				// si deux cases adjacentes portent le numéro du joueur, on n'en compte qu'une seule
				compteur_antidiag_haut = 1;
			}
			j--;
		}
		if (compteur_antidiag >= 5)
		{
			// un alignement de 5 peut être réalisé par le joueur
			return 0;
		}
	}


	/* aucun alignement possible */ 
	return 1;
}



// #############################################################################################################
// is_fin_du_jeu 
// retourne 1 si aucun alignement de 5 est possible
// retourne 0 sinon

int is_fin_de_partie(partie *p)
{
	int largeur, hauteur, test;
	int i, j, k;
	largeur = p->l;
	hauteur = p->h;
	test = 0; // ce n'est pas la fin de la partie

	/* on parcourt la grille : pour chaque joueur et pour chaque case vide, on teste si ce joueur peut aligner au moins 5 croix */
	for (k = 1 ; k <= p->nbjoueurs ; k++)
	{
		char s[1];
		sprintf(s, "%d", k+1);

		p->canplay[k-1] = 0;
		for (i=0 ; i < hauteur ; i++)
		{
			for (j=0 ; j< largeur  ; j++)
			{
				if ((p->map[j][i] == 'O') && (alignement5possible(p, j, i, k) == 0))
				{
					// le joueur k peut aligner 5 croix autour de la case (i, j)
					// mise à jour de canplay
					p->canplay[k-1]++;
					test = 0;
				}				
			}
		}
	}

	// 2 conditions pour fin du jeu
	// soit plus aucun joueur ne peut aligner 5 croix
	// soit le nombre de coups maximal a été atteint
	if ((test == 1) || (p->nbcoup >= p->nbcoupmax) || (p->nbcoup >= (p->l)*(p->h)))
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
// joueur_suivant retourne 
// le numéro du joueur suivant qui a encore la possibilité de réaliser un alignement de 5
// 0 si aucun joueur ne peut aligner 5 croix 

int joueur_suivant(partie *p, int joueur)
{
	int i;

	/* SI dernier joueur du tableau */
	// on regarde le tableau à partir du début
	if (joueur == p->nbjoueurs)
	{
		for (i = 0 ; i < p->nbjoueurs ; i++)
		{ 
			if (p->canplay[i] > 0)
			{
				return (i+1);
			}
			else 
			{
				printf("          le joueur %d ne peut plus jouer car il ne peut plus aligner 5 croix.\n", i+1);
			}
		}   
	}

	/* SINON */
	// on regarde le tableau à partir du joueur joueur+1 (donc canplay[joueur])
	for (i = joueur ; i < p->nbjoueurs ; i++)
	{
		if (p->canplay[i] > 0)
		{
			return (i+1);
		}
		else 
		{
			printf("          le joueur %d ne peut plus jouer car il ne peut plus aligner 5 croix.\n", i+1);
		}
	}
	// si on est arrivé à la fin du tableau et qu'aucun joueur ne peut encore jouer, on revient au début
	for (i = 0 ; i < joueur ; i++)
	{
		if (p->canplay[i] > 0)
		{
			return (i+1);
		}
		else 
		{
			printf("          le joueur %d ne peut plus jouer car il ne peut plus aligner 5 croix.\n", i+1);
		}
	}

	/* fin de partie */
	return 0;
}



// ####################################################################################
// destroy libère la mémoire allouée pour la partie

void destroy(partie *p)
{
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
// getmap retourne la grille sous la forme d'un char*
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
// getscore retourne le score du joueur

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



// ###################################################################################################################
// dernier_joueur complète la grille à la place du joueur (seulement les cases qui lui permettent d'aligner 5 croix)
// et met à jour le score
// à utiliser si joueur_suivant == joueur

void dernier_joueur(partie *p, int joueur)
{
	int i, j, k, compteur_case_vide, compteur;
	compteur = 0;
	compteur_case_vide = 0;
	char s[1];
	sprintf(s, "%d", joueur);


	/* on compte le nombre de cases vides que le joueur peut marquer pour aligner 5 croix */
	for (j = 0 ; j < p->h ; j++)
	{
		for (i= 0 ; i < p->l ; i++)
		{
			if (p->map[i][j] == 'O' && (alignement5possible(p, i, j, joueur) == 0))
			{
				compteur_case_vide++;
			}
		}
	}
	//printf("nb de cases vides : %d\n", compteur_case_vide);
	Coordonnees *case_vide = (Coordonnees *) calloc(compteur_case_vide, sizeof(struct Coordonnees));

	/* on marque les cases qui peuvent éventuellement faire partie d'un alignement de 5
	   et on retient les coordonnées de ces cases dans le tableau case_vide */
	for (j = 0 ; j < p->h ; j++)
	{
		for (i= 0 ; i < p->l ; i++)
		{
			if ((p->map[i][j] == 'O') && (alignement5possible(p, i, j, joueur) == 0))
			{
				// on marque la case
				p->map[i][j] = 'X';
				// on retient les coordonnées de la case marquée dans le tableau case_vide
				case_vide[compteur].x = i;
				case_vide[compteur].y = j;
				compteur++;
			}
		}
	}
	affiche_map(getmap(p), p->l, p->h);

	/* on teste si les cases marquées font partie d'un alignement de 5 */
	for (k = 0 ; k < compteur_case_vide ; k++ )
	{
		if ((p->map[case_vide[k].x][case_vide[k].y] != *s) && (alignement5(p, case_vide[k].x, case_vide[k].y, joueur) != 0))
		{
			//printf("pas d'alignement autour de la case (%d, %d)\n", case_vide[k].x, case_vide[k].y);
			// si la case ne fait pas partie d'un alignement de 5 : la case est de nouveau considérée comme vide
			p->map[case_vide[k].x][case_vide[k].y] = 'O';
		}
	}
	/* on met à jour le score */
	calcul_score(p, joueur);
	free(case_vide);
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
		if (aff_debug)
		{
			printf("           joueur %d : coup (%d, %d) non valide\n", joueur, x, y); 
		}  
		return (-1);
	}

	/* case déjà marquée */
	if (p->map[x][y] != 'O')
	{
		if (aff_debug)
		{
			printf("          joueur %d : case (%d, %d) déjà marquee\n", joueur, x, y);
		}
		return (-1);
	}

	/* case vide */
	else 
	{
		// on marque la case 
		p->map[x][y] = 'X';
		// on met à jour le nombre de coups joués
		p->nbcoup++;
		// on met à jour les coordonnées du dernier coup
		p->xd = x;
		p->yd = y;
		if (aff_debug)
		{
			printf("          le joueur %d a joué le coup (%d, %d).\n", joueur, x, y);
		}
		if (alignement5(p, x, y, joueur) == 0) // mise a jour de la map 
		{
			// mise a jour du score
			calcul_score(p, joueur);
			if (aff_debug)
			{
				printf("          joueur %d : alignement de 5 : (%d, %d)\n", joueur, x, y); 
			}
		}

		if (is_fin_de_partie(p) == 1)
		{
			printf("          fin de la partie !\n");
			return 0;
		}

		int suivant;
		suivant = joueur_suivant(p, joueur);
		if (suivant == 0)
		{
			printf("          fin de la partie !\n");
			return 0;
		}
		if (suivant == joueur)
		{
			// il ne reste qu'un seul joueur qui peut encore aligner 5 croix
			// on complète la grille à la place du joueur
			dernier_joueur(p, joueur);
			printf("          fin de la partie \n");
			return 0;
		}
		if (aff_debug)
		{
			printf("          C'est au joueur %d de jouer !\n", suivant);
		}
		return suivant;
	}
} 
