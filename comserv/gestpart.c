#include <stdio.h>
#include <stdlib.h>
#include <protoserv.h>

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

// ########################################################################################################
// alignement détecte les alignements de 5 autour de la case (x, y) et met à jour la map avec le numero du joueur si alignement de 5 
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
  
  /* aucun alignement */
  // on retourne 1
  return 1;

}

// ##########################################################################################
// alignement5possible détecte les alignements de 5 possibles autour de la case (x, y)
// retourne 0 si alignement de 5 possible
// 1 sinon
int alignement5possible(partie *p, int x, int y){
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
	  if ((x-1+j >=0) && (x-1+j < largeur ))
	    {
	      if (((map[x-1][y-1] == 'X') || (map[x-1][y-1] == 'O')) && ((map[x-1+j][y-1] == 'X') || (map[x-1+j][y-1] == 'O')))
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
	  if ((y-1+j >= 0) && (y-1+j < hauteur))
	    {
	      if (((map[x-1][y-1] == 'X') || (map[x-1][y-1] == 'O')) && ((map[x-1][y-1+j] == 'X') || (map[x-1][y-1+j] == 'O')))
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
	  if ((x-1+j >= 0) && (x-1+j < largeur) && (y-1+j >=0) && (y-1+j < hauteur))
	    {
	      if (((map[x-1][y-1] == 'X') || (map[x-1][y-1] == 'O')) && ((map[x-1+j][y-1+j] == 'X') || (map[x-1+j][y-1+j] == 'O')))
		{
		  compteur1++;
		}
	    }

	  
	  // alignement possible sur une antidiagonale 
	  if ((x-1-j >= 0) && (x-1-j < largeur ) && (y-1+j >= 0) && (y-1+j < hauteur))
	    {
	      if (((map[x-1][y-1] == 'X') || (map[x-1][y-1] == 'O')) && ((map[x-1-j][y-1+j] == 'X') || (map[x-1-j][y-1+j] == 'O')))
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

// #############################################################################################################
// is_fin_du_jeu 
// retourne 0 si aucun alignement de 5 est possible
// retourne 1 sinon

int is_fin_de_partie(partie *p){
  int largeur, hauteur;
  int i, j;
  largeur = p->l;
  hauteur = p->h;
  int test;
  test = 0;
  for (i=1 ; i <= hauteur ; i++){
    for (j=1 ; j<= largeur  ; j++){
      if (alignement5possible(p, j, i) == 0){
	test = 1;
      }
      
    }
  }
  return test;
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
  if ((x<1) || (x>largeur) || (y<1) || (y>hauteur) )
    {
      printf("joueur %d : coup (%d, %d) non valide\n", joueur, x, y);     
      return (-1);
    }

  // case déjà marquée
  if (p->map[x-1][y-1] != 'O')
    {
      printf("joueur %d : case (%d, %d) marquee\n", joueur, x, y);
      return (-1);
    }
  // case vide
  else 
    {
      // on marque la case 
      p->map[x-1][y-1] = 'X';
      printf("le joueur %d a joué le coup (%d, %d).\n", joueur, x, y);
      if (alignement5(p, x, y, joueur) == 0) // mise a jour de la map
	{
	  printf("joueur %d : alignement de 5 : (%d, %d)\n", joueur, x, y);
	  // mise a jour du score du joueur
	  p->score[joueur-1]++;	    
	}

      if (is_fin_de_partie(p) == 0)
	{
	  printf("fin de la partie !\n");
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
  free(p); 
}

// ####################################################################################
char ** getmap(partie *p){
  return p->map;
}

// ###################################################################################
int getscore(partie *p, int joueur){
  return p->score[joueur-1];
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


// ########################### test des fonctions ##################################
int main(){

  partie *p;
  p = init(6, 7, 3); 
  printf("largeur : %d\n", p->l);
  printf("hauteur : %d\n", p->h);
  printf("nombre des joueurs : %d\n", p->nbjoueurs); 

 
  affiche_map(p->map, p->l, p->h);
  affiche_score(p->score, p->nbjoueurs);

  printf(" ######################## \n");
  play(p, 1, 1, 1);
  play(p, 2, 2, 2);
  play(p, 3, 3, 3);
  play(p, 4, 4, 1);
  play(p, 5, 5, 2);

  printf(" ####### test ####### \n");
  play(p, 5, 5, 3);
  play(p, 7, -1, 3);

  affiche_map(getmap(p), p->l, p->h);
  affiche_score(p->score, p->nbjoueurs);
  

  printf(" ######################## \n");
  play(p, 1, 6, 3);
  play(p, 2, 6, 1);
  play(p, 3, 6, 2);
  play(p, 4, 6, 3); 
  play(p, 5, 6, 1);

  affiche_map(getmap(p), p->l, p->h);
  affiche_score(getscore(p), p->nbjoueurs);

  printf(" ######################## \n");
  play(p, 3, 7, 2);
  play(p, 4, 7, 3); 
  play(p, 5, 7, 1);
  play(p, 6, 7, 2);
  play(p, 2, 7, 3);

  affiche_map(getmap(p), p->l, p->h);
  affiche_score(getscore(p), p->nbjoueurs);
 
   
  printf(" ######################## \n");
  play(p, 6, 1, 1);
  play(p, 5, 2, 2);
  play(p, 3, 4, 3);
  play(p, 2, 5, 1);
  play(p, 4, 3, 2); 
  play(p, 4, 3, 2); 
  
  affiche_map(getmap(p), p->l, p->h);
  affiche_score(getscore(p), p->nbjoueurs);

  printf(" ######################## \n");
  play(p, 6, 2, 3);
  play(p, 6, 3, 1);
  play(p, 6, 4, 2);
  play(p, 6, 5, 3);
  play(p, 6, 4, 1); 
  play(p, 6, 6, 1);  

  affiche_map(getmap(p), p->l, p->h);
  affiche_score(getscore(p), p->nbjoueurs);

  return 0;
}

