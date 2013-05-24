#include <stdio.h>
#include <stdlib.h>
//#include <protoserv.h>
#include "gestpart.h"

// pour mes tests
// gcc -Wall -o gestpart gestpart.c 

typedef struct partie partie; 

struct partie {
  int nbjoueurs; 
  int l; // largeur 
  int h; // hauteur
  char **map;
  int *score; //
};



partie *init(int l, int h, int nbjoueurs)
{
  // verifier l, h et nbjoueurs
  int i, j;
  partie *p = malloc(sizeof(struct partie));
  char ** map = malloc(l * sizeof(char*));
  int * score = malloc(nbjoueurs * sizeof(int));

  for (i=0 ; i < l ; i++)
    {
      map[i] = malloc(h*sizeof(char));
    }
  for (i=0 ; i < l ; i++)
    {
      for (j=0 ; j < h ; j++){
	map[i][j] = 'O';
      }
    }
  for (i=0 ; i < nbjoueurs ; i++)
    {
      score[i] = 0;
    }
  p->map = map;
  p->score = score;
  p->nbjoueurs = nbjoueurs;
  p->l = l;
  p->h = h;
  return p;
}

/*
// ########################################################################################################
// alignement détecte les alignements de 5 (seulement des alignements de 5) autour de la case (x, y) et met à jour la map avec le numero du joueur si alignement de 5 
// retourne 
// 0 si alignement de 5 autour de la case (x, y) et met à jour la map avec le numero du joueur
// 1 sinon
int alignement5(partie *p, int x, int y, int joueur)
{
  int largeur, hauteur;
  int i, j;
  largeur = p->l;
  hauteur = p->h;
  char **map = p->map;//
  int ret;
  char s[1];
  ret = sprintf(s, "%d", joueur);
  if (ret < 0)
    {
      fprintf(stderr, "erreur de conversion int -> string.\n");
    }
  
 
  // alignement sur une ligne 
  for (i=-4 ; i<1 ; i++)
    {
      int compteur;
      compteur = 0;
      for (j=i ; j<i+5 ; j++)
	{
	  // rester sur la meme ligne ??
	  if ((x-1+j >=0) && (x-1+j < largeur ))
	    {
	      if ((map[x-1][y-1] == 'X') && (map[x-1+j][y-1] == map[x-1][y-1]))
		{
		  compteur++;
		}
	    }
	}
      if (compteur == 5)
	{// si on a un alignement de 5
	  // mise a jour de la map
	  for (j=i ; j<i+5 ; j++)
	    {
	      map[x-1][y-1] = *s;
	      map[x-1+j][y-1] = *s;
	    }
	  return 0; 
	}
    }
      

  // alignement sur une colonne 

  for (i = -4 ; i<1 ; i++)
    {
      int compteur;
      compteur = 0;
      for (j=i ; j < i+5 ; j++)
	{
	  if ((y-1+j >= 0) && (y-1+j < hauteur))
	    {
	      if ((map[x-1][y-1] == 'X') && (map[x-1][y-1+j] == map[x-1][y-1]))
		{
		  compteur++;
		}
	    }
	}
      if (compteur == 5)
	{ // si alignement de 5 
	  // mise a jour de la map
	  for (j=i ; j < i+5 ; j++)
	    {
	      map[x-1][y-1] = *s;
	      map[x-1][y-1+j] = *s;
	    }
	  return 0;
	}
    }

  
  // alignement sur une diagonale ou antidiagonale 
  for (i = -4 ; i < 1 ; i++)
    {
      int compteur1, compteur2;
      compteur1 = 0;
      compteur2 = 0;
      
      for (j = i ; j < i+5 ; j++)
	{
	  // alignement sur une diagonale ?
	  if ((x-1+j >= 0) && (x-1+j < largeur) && (y-1+j >=0) && (y-1+j < hauteur))
	    {
	      if ((map[x-1][y-1] == 'X') && (map[x-1+j][y-1+j] == map[x-1][y-1]))
		{
		  compteur1++;
		}
	    }
	  
	  // alignement sur une antidiagonale ?
	  if ((x-1-j >= 0) && (x-1-j < largeur ) && (y-1+j >= 0) && (y-1+j < hauteur))
	    {
	      if ((map[x-1][y-1] == 'X') && (map[x-1-j][y-1+j] == map[x-1][y-1]))
		{
		  compteur2 += 1;
		}
	    }
	}

      // si alignement de 5
      if (compteur1 == 5){
	for (j = i ; j < i+5 ; j++)
	  {
	    map[x-1][y-1] = *s;
	    map[x-1+j][y-1+j] = *s;
	  }
	return 0;
      }

      if (compteur2 == 5){
	for (j = i ; j < i+5 ; j++)
	  {
	    map[x-1][y-1] = *s;
	    map[x-1-j][y-1+j] = *s;
	  }
	return 0;
      }

    }
  
  // aucun alignement 
  // on retourne 1
  return 1;
}
*/

// ##########################################################################################################################
// alignement 5 bis détecte les alignements de 5 ou plus autour de la case (x, y) et met à jour la map avec le numero du joueur si alignement de 5 ou plus 
// retourne 
// 0 si alignement de 5 autour de la case (x, y) et met à jour la map avec le numero du joueur
// 1 sinon

int alignement5bis(partie *p, int x, int y, int joueur)
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
  
  // alignement sur une ligne
  int compteur_ligne;
  compteur_ligne = 0;
  i = 0;
  j = -1;
  while (((x+i) >= 0) && ((x+i) < largeur) && (map[x+i][y] == 'X')){
    compteur_ligne++;
    i++;
  }
  while (((x+j) >= 0) && ((x+j) < largeur) && (map[x+j][y] == 'X')){
    compteur_ligne++;
    j--;
  }

  if (compteur_ligne >= 5){
    // mise à jour de la map
    i = 0;
    j = -1;
    while (((x+i) >= 0) && ((x+i) < largeur) && (map[x+i][y] == 'X')){
      map[x+i][y] = *s;
      i++;
    }
    while (((x+j) >= 0) && ((x+j) < largeur) && (map[x+j][y] == 'X')){
      map[x+j][y] = *s;
      j--;
    }
    return 0;
  }    

  // alignement sur une colonne 
  int compteur_col;
  compteur_col = 0;
  i=0;
  j=-1;
  while ((y+i >=0) && (y+i < hauteur) && (map[x][y+i] == 'X')){
    compteur_col++;
    i++;
  }
  while ((y+j >=0) && (y+j < hauteur) && (map[x][y+j] == 'X')){
    compteur_col++;
    j--;
  }

  if (compteur_col >= 5){
    i=0;
    j = -1;
    while ((y+i >=0) && (y+i < hauteur) && (map[x][y+i] == 'X')){
      map[x][y+i] = *s;
      i++;
    }  
    while ((y+j >=0) && (y+j < hauteur) && (map[x][y+j] == 'X')){
      map[x][y+j] = *s;
      j--;
    }
    return 0;
  }
   
  // alignement sur une diagonale  
  int compteur_diag;
  compteur_diag = 0;
  i = 0;
  j = -1;
  while ((x+i >= 0) && (x+i < largeur) && (y+i >=0) && (y+i < hauteur) && (map[x+i][y+i] == 'X')){
    compteur_diag++;
    i++;
  }
  while ((x+j >= 0) && (x+j < largeur) && (y+j >=0) && (y+j < hauteur) && (map[x+j][y+j] == 'X')){
    compteur_diag++;
    j--;
  }
  if (compteur_diag >= 5){
    i = 0;
    j = -1;
    while ((x+i >= 0) && (x+i < largeur) && (y+i >=0) && (y+i < hauteur) && (map[x+i][y+i] == 'X')){
      map[x+i][y+i] = *s;
      i++;
    }
    while ((x+j >= 0) && (x+j < largeur) && (y+j >=0) && (y+j < hauteur) && (map[x+j][y+j] == 'X')){
      map[x+j][y+j] = *s;
      j--;
    } 
    return 0;
  }


  // alignement sur une antidiagonale
  int compteur_antidiag;
  compteur_antidiag = 0;
  i = 0;
  j = -1;
  while ((x-i >= 0) && (x-i < largeur) && (y+i >=0) && (y+i < hauteur) && (map[x-i][y+i] == 'X')){
    compteur_antidiag++;
    i++;
  }
  while ((x-j >= 0) && (x-j < largeur) && (y+j >=0) && (y+j < hauteur) && (map[x-j][y+j] == 'X')){
    compteur_antidiag++;
    j--;
  }
  if (compteur_antidiag >= 5){
    i = 0;
    j = -1;
    while ((x-i >= 0) && (x-i < largeur) && (y+i >=0) && (y+i < hauteur) && (map[x-i][y+i] == 'X')){
      map[x-i][y+i] = *s;
      i++;
    }
    while ((x-j >= 0) && (x-j < largeur) && (y+j >=0) && (y+j < hauteur) && (map[x-j][y+j] == 'X')){
      map[x-j][y+j] = *s;
      j--;
    } 
    return 0;
  }

  
  /* aucun alignement */
  // on retourne 1
  return 1;

}



// ##########################################################################################
// alignement5possible détecte les alignements de 5 possibles autour de la case (x, y)
// retourne 0 si alignement de 5 possible
// 1 sinon
int alignement5possiblebis(partie *p, int x, int y){
  int largeur, hauteur;
  int i, j;
  char **map;
  largeur = p->l;
  hauteur = p->h;
  map = p->map;

 
  /* alignement possible sur une ligne ? */
  for (i=-4 ; i<1 ; i++)
    {
      int compteur;
      compteur = 0;
      for (j=i ; j<i+5 ; j++)
	{
	  // rester sur la meme ligne 
	  if ((x+j >=0) && (x+j < largeur ))
	    {
	      if ((map[x+j][y] == 'X') || (map[x+j][y] == 'O'))
		{
		  compteur++;
		}
	    }
	}
      if (compteur == 5)
	{
	  return 0; 
	}
    }
      

  /* alignement possible sur une colonne ? */
  for (i = -4 ; i<1 ; i++)
    {
      int compteur;
      compteur = 0;
      for (j=i ; j < i+5 ; j++)
	{
	  if ((y+j >= 0) && (y+j < hauteur))
	    {
	      if ((map[x][y+j] == 'X') || (map[x][y+j] == 'O'))
		{
		  compteur++;
		}
	    }
	}
      if (compteur == 5)
	{ 
	  return 0;
	}
    }


  /* alignement possible sur une diagonale ou antidiagonale ? */
  for (i = -4 ; i < 1 ; i++)
    {
      int compteur1, compteur2;
      compteur1 = 0;
      compteur2 = 0;
      
      for (j = i ; j < i+5 ; j++)
	{
	  // alignement possible sur une diagonale 
	  if ((x+j >= 0) && (x+j < largeur) && (y+j >=0) && (y+j < hauteur))
	    {
	      if ((map[x+j][y+j] == 'X') || (map[x+j][y+j] == 'O'))
		{
		  compteur1++;
		}
	    }

	  
	  // alignement possible sur une antidiagonale 
	  if ((x-j >= 0) && (x-j < largeur ) && (y+j >= 0) && (y+j < hauteur))
	    {
	      if ((map[x-j][y+j] == 'X') || (map[x-j][y+j] == 'O'))
		{
		  compteur2 += 1;
		}
	    }
	  
	}

      if (compteur1 == 5)
	{
	  return 0;
	}

      if (compteur2 == 5)
	{
	  return 0;
	}
    }
  

  /* aucun alignement possible */
  // on retourne 1
  return 1;

}




int alignement5possible(partie *p, int x, int y){
  int largeur, hauteur;
  int i, j;
  char **map;
  largeur = p->l;
  hauteur = p->h;
  map = p->map;

 
  /* alignement possible sur une ligne ? */
  int compteur_ligne;
  compteur_ligne = 0;
  i = 0;
  j = -1;
  while (((x+i) >= 0) && ((x+i) < largeur) && ((map[x+i][y] == 'X') || (map[x+i][y] == 'O'))){
    compteur_ligne++;
    i++;
  }
  while (((x+j) >= 0) && ((x+j) < largeur) && ((map[x+j][y] == 'X') || (map[x+j][y] == 'O'))){
    compteur_ligne++;
    j--;
  }
  if (compteur_ligne >= 5){// mettre cette condition dans les while
    return 0;
  }  
    
  /* alignement possible sur une colonne ? */

  int compteur_col;
  compteur_col = 0;
  i = 0;
  j = -1;
  while ((y+i >=0) && (y+i < hauteur) && ((map[x][y+i] == 'X') || (map[x][y+i] == 'O'))){
    compteur_col++;
    i++;
  }
  while ((y+j >=0) && (y+j < hauteur) && ((map[x][y+j] == 'X') || (map[x][y+j] == 'O'))){
    compteur_col++;
    j--;
  }
  if (compteur_col >= 5){
    return 0;
  }


  /* alignement possible sur une diagonale */
  int compteur_diag;
  compteur_diag = 0;
  i = 0;
  j = -1;
  while ((x+i >= 0) && (x+i < largeur) && (y+i >=0) && (y+i < hauteur) && ((map[x+i][y+i] == 'X') || (map[x+i][y+i] == 'O'))){
    compteur_diag++;
    i++;
  }
  while ((x+j >= 0) && (x+j < largeur) && (y+j >=0) && (y+j < hauteur) && ((map[x+j][y+j] == 'X') || (map[x+j][y+j] == 'O'))){
    compteur_diag++;
    j--;
  }
  if (compteur_diag >= 5){
    return 0;
  }

  // alignement possible sur une antidiagonale ?
  int compteur_antidiag;
  compteur_antidiag = 0;
  i = 0;
  j = -1;
  while ((x-i >= 0) && (x-i < largeur) && (y+i >=0) && (y+i < hauteur) && ((map[x-i][y+i] == 'X') || (map[x-i][y+i] == 'O'))){
    compteur_antidiag++;
    i++;
  }
  while ((x-j >= 0) && (x-j < largeur) && (y+j >=0) && (y+j < hauteur) && ((map[x-j][y+j] == 'X') || (map[x-j][y+j] == 'O'))){
    compteur_antidiag++;
    j--;
  }
  if (compteur_antidiag >= 5){
    return 0;
  }
 

  /* aucun alignement possible */
  // on retourne 1
  return 1;

}

// #############################################################################################################
// is_fin_du_jeu 
// retourne 0 si aucun alignement de 5 est possible
// retourne 1 sinon

int is_fin_de_partie(partie *p){
  int largeur, hauteur;
  int i, j;
  largeur = p->l;
  hauteur = p->h;
  for (i=0 ; i < hauteur ; i++){
    for (j=0 ; j< largeur  ; j++){
      if (((p->map[j][i] == 'O') || (p->map[j][i] == 'X')) && (alignement5possible(p, j, i) == 0)){
	// 1 alignement de 5 possible
	return 1;
      }
    }
  }
  // pas d'alignement de 5 possible
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
  int largeur, hauteur, nbjoueurs;
  largeur = p->l;
  hauteur = p->h;
  nbjoueurs = p->nbjoueurs;
  
  // coordonnees non valides
  if ((x<0) || (x >= largeur) || (y<0) || (y >= hauteur) )
    {
      printf("           joueur %d : coup (%d, %d) non valide\n", joueur, x, y);     
      return (-1);
    }

  // case déjà marquée
  if (p->map[x][y] != 'O')
    {
      printf("          joueur %d : case (%d, %d) marquee\n", joueur, x, y);
      return (-1);
    }
  // case vide
  else 
    {
      // on marque la case 
      p->map[x][y] = 'X';
      printf("          le joueur %d a joué le coup (%d, %d).\n", joueur, x, y);
      if (alignement5bis(p, x, y, joueur) == 0) // mise a jour de la map ####################
	{
	  printf("          joueur %d : alignement de 5 : (%d, %d)\n", joueur, x, y);
	  // mise a jour du score du joueur
	  p->score[joueur-1]++;	    
	}

      if (is_fin_de_partie(p) == 0)
	{
	  printf("          fin de la partie !\n");
	  return 0;
	}
      if (joueur == nbjoueurs)
	{
	  return 1;
	}
      else 
	{
	  return (joueur + 1);
	}

    }

} 

// ####################################################################################
void destroy(partie *p){
  int i;
  for (i=0 ; i < p->l ; i++)
    {
      free(p->map[i]);
    }
  free(p->map);
  free(p->score);
  free(p); 
}

// ####################################################################################

char ** getmapbis(partie *p){
  return p->map;
}


char * getmap(partie *p){
  int i, j, k;
  char *s =  malloc(((p->l)*(p->h)) * sizeof(char));
  k = 0;
  for (j=0 ; j < p->h ; j++){
    for (i=0 ; i < p->l ; i++){
      s[k] = p->map[i][j];
      k++;
    }
  }
  return s;
}


// ###################################################################################
int getscore(partie *p, int joueur){
  return p->score[joueur-1];
}

int getvainqueur(partie *p){// et pour les ex aequo ? si ex aequo, le premier connecté gagné ??????????????
  int max, vainqueur, i;
  max = p->score[0];
  vainqueur =1;
  for (i=0 ; i < (p->nbjoueurs) ; i++){
    if (p->score[i] > max){
      max = p->score[i];
      vainqueur = i+1;
    } 
  }
  return vainqueur;

}

// ############################# fonctions affichage #################################

void affiche_map(char ** map, int l, int h){
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


void affiche_score(int * score, int nbjoueurs){
  int i;
  printf("score :\n");
  for (i=0 ; i< nbjoueurs ; i++){
    printf("joueur %d : %d\n", i+1, score[i]);
  }
  printf("\n");
}

/*
// ########################### test des fonctions ##################################

int main(){

  partie *p;
  p = init(6, 7, 3); 
  printf("largeur : %d\n", p->l);
  printf("hauteur : %d\n", p->h);
  printf("nombre des joueurs : %d\n", p->nbjoueurs); 

 
  affiche_map(getmapbis(p), p->l, p->h);
  affiche_score(p->score, p->nbjoueurs);

  printf(" ######################## \n");
  play(p, 0, 0, 1);
  play(p, 1, 1, 2);
  play(p, 3, 3, 3);
  affiche_map(getmapbis(p), p->l, p->h);
  play(p, 4, 4, 1);
  play(p, 2, 2, 2);

  printf(" ####### test ####### \n");
  //play(p, 5, 5, 3);
  play(p, 7, -1, 3);

  affiche_map(getmapbis(p), p->l, p->h);
  affiche_score(p->score, p->nbjoueurs);
  

  printf(" ######################## \n");
  play(p, 0, 5, 3);
  play(p, 1, 5, 1);
  play(p, 2, 5, 2);
  play(p, 3, 5, 3); 
  play(p, 4, 5, 1);

  affiche_map(getmapbis(p), p->l, p->h);
  affiche_score(p->score, p->nbjoueurs);

  printf(" ######################## \n");
  play(p, 0, 6, 2);
  play(p, 3, 6, 3); 
  play(p, 4, 6, 1);
  play(p, 5, 6, 2);
  affiche_map(getmapbis(p), p->l, p->h);
  play(p, 1, 6, 3);
  play(p, 2, 6, 2);
  affiche_map(getmapbis(p), p->l, p->h);
  affiche_score(p->score, p->nbjoueurs);
 
   
  printf(" ######################## \n");
  play(p, 5, 0, 1);
  play(p, 4, 1, 2);
  play(p, 2, 3, 3);
  play(p, 1, 4, 1);
  affiche_map(getmapbis(p), p->l, p->h);
  play(p, 3, 2, 2); 
  play(p, 4, 2, 2); 
  
  affiche_map(getmapbis(p), p->l, p->h);
  affiche_score(p->score, p->nbjoueurs);

  printf(" ######################## \n");
  play(p, 5, 1, 3);
  play(p, 5, 2, 1);
  play(p, 5, 3, 2);
  affiche_map(getmapbis(p), p->l, p->h);
  play(p, 5, 4, 3);
  play(p, 5, 3, 1); 
  play(p, 5, 5, 3);  

  printf("la map est : %s\n", getmap(p));
  affiche_map(getmapbis(p), p->l, p->h);
  affiche_score(p->score, p->nbjoueurs);
  printf("le vainqueur est le joueur %d avec un score de %d\n", getvainqueur(p), getscore(p, getvainqueur(p)));
 
  return 0;
}


*/
