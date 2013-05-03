/***************************************************/
/*      Projet  IPI S2 - 2013 : puissance 5        */
/*          Nathalie Au, Lewin Alexandre,          */
/*       Beaume Jérémy et Métraud Alexandre        */
/*                  Groupe 4.2                     */
/*           COMUSR, interface graphique           */
/***************************************************/

//Ce fichier comporte des tests sur l'interface graphique du client.
//La fenêtre graphique en jeu est séparée en deux parties : la map et la liste des joueurs

//Commande pour compiler avec la SDL:
//gcc -o test test.c -lSDLmain -lSDL

#include <stdlib.h>
#include <stdio.h>
#include "SDL/SDL.h"
#define true 1
#define false 0
typedef int bool;


#define TAILLE_CASE 20
#define TAILLE_INTERCASE 5
#define TAILLE_BORDURE 7


void pause();
void draw_bordures_map(SDL_Surface*);
void draw_intercases_map(SDL_Surface*, int, int);
int draw_cases_map(SDL_Surface*, int, int, char*);
//TODO init_map
//TODO draw_liste_joueurs


int main(int argc, char** argv)
{
  //Initialisation des variables.
	SDL_Surface* ecran = NULL;
  SDL_Rect position;

  SDL_Surface* map = NULL;
  int nb_case_col_map = 20;
  int nb_case_lig_map = 15;
  int nb_case_tot_map = nb_case_col_map * nb_case_lig_map;
  int largeur_map = 2 * TAILLE_BORDURE + nb_case_col_map * (TAILLE_CASE + TAILLE_INTERCASE) - TAILLE_INTERCASE;
  int hauteur_map = 2 * TAILLE_BORDURE + nb_case_lig_map * (TAILLE_CASE + TAILLE_INTERCASE) - TAILLE_INTERCASE;
  int i;
  char* desc_map = malloc ((nb_case_tot_map + 1) * sizeof(char));

  SDL_Surface* liste_joueurs = NULL;
  int hauteur_liste_joueurs = hauteur_map;
  int largeur_liste_joueurs = 50;



  //Démarrage de la SDL.
	if (SDL_Init(SDL_INIT_VIDEO) == -1) // Si erreur :
	{
  	fprintf(stderr, "Erreur d'initialisation de la SDL : %s\n", SDL_GetError()); // Écriture de l'erreur
 	  exit(EXIT_FAILURE); // On quitte le programme
	}
  atexit(SDL_Quit); // Si on rencontre un exit on arrête la SDL avant.



  //Création de la zone de l'écran qui contiendra la liste des joueurs
  liste_joueurs = SDL_CreateRGBSurface(SDL_HWSURFACE, largeur_liste_joueurs, hauteur_liste_joueurs, 32, 0, 0, 0, 0);
  //TODO initialisation de l'affichage

  

  //Création de la zone de l'écran qui contiendra la map
  map = SDL_CreateRGBSurface(SDL_HWSURFACE, largeur_map, hauteur_map, 32, 0, 0, 0, 0);
  draw_bordures_map(map);
  draw_intercases_map(map, nb_case_col_map, nb_case_lig_map);
  for (i=0; i < nb_case_tot_map; i++) desc_map[i] = 'O';
  desc_map[nb_case_tot_map] = '\0';
  desc_map[10] = '1';
  desc_map[13] = '2';
  desc_map[23] = '3';
  desc_map[45] = '4';
  desc_map[52] = '5';
  desc_map[53] = '6';
  desc_map[63] = '7';
  desc_map[68] = '8';
  desc_map[70] = '9';
  desc_map[72] = 'X';
  draw_cases_map(map, nb_case_col_map, nb_case_lig_map, desc_map);



  //Ouverture d'une fenêtre.
  ecran = SDL_SetVideoMode(largeur_liste_joueurs + largeur_map, hauteur_map, 32, SDL_HWSURFACE | SDL_DOUBLEBUF); // On tente d'ouvrir une fenêtre en 32 bit sur la mémoire vidéo en double buffuring
  if (ecran == NULL) // Si l'ouverture a échoué, on le note et on arrête
  {
      fprintf(stderr, "Impossible de charger le mode vidéo : %s\n", SDL_GetError());
      exit(EXIT_FAILURE);
  }



  //Options de la fenêtre
  SDL_WM_SetCaption("Puissance 5 en réseau", "Puissance 5 en réseau"); //Titre de la fenêtre, et de l'icône
  //TODO SDL_WM_SetIcon(SDL_LoadBMP("pack_images_sdz/sdl_icone.bmp"), NULL); //Icône de la fenêtre
	SDL_FillRect(ecran, NULL, SDL_MapRGB(ecran->format, 255, 255, 255)); //Couleur de fond, blanc



  //Collage de la liste des joueurs et de la map sur la fenêtre
  position.x = 0;
  position.y = 0;
  SDL_BlitSurface(liste_joueurs, NULL, ecran, &position);
  position.x = largeur_liste_joueurs;
  position.y = 0;
  SDL_BlitSurface(map, NULL, ecran, &position);

	SDL_Flip(ecran);

  SDL_FreeSurface(liste_joueurs);
  SDL_FreeSurface(map);
  pause();



  //Arrêt de la SDL.
	SDL_Quit();
	return EXIT_SUCCESS;
}






//Cette fonction dessine les bordures de la map avec une couleur unie
void draw_bordures_map(SDL_Surface* map)
{
	SDL_Surface* bordure = NULL;
  SDL_Rect position;

  //Bordures basse et haute
  bordure = SDL_CreateRGBSurface(SDL_HWSURFACE, map->w, TAILLE_BORDURE, 32, 0, 0, 0, 0);
  SDL_FillRect(bordure, NULL, SDL_MapRGB(map->format, 200, 0, 0));
  position.x = 0;
  position.y = 0;
  SDL_BlitSurface(bordure, NULL, map, &position);
  position.y = map->h - TAILLE_BORDURE;
  SDL_BlitSurface(bordure, NULL, map, &position);
  SDL_FreeSurface(bordure);

  //Bordures gauche et droite
  bordure = SDL_CreateRGBSurface(SDL_HWSURFACE, TAILLE_BORDURE, map->h, 32, 0, 0, 0, 0);
  SDL_FillRect(bordure, NULL, SDL_MapRGB(map->format, 200, 0, 0));
  position.x = 0;
  position.y = 0;
  SDL_BlitSurface(bordure, NULL, map, &position);
  position.x = map->w - TAILLE_BORDURE;
  SDL_BlitSurface(bordure, NULL, map, &position);
  SDL_FreeSurface(bordure);
}






//Cette fonction dessine les lignes sur la map avec une couleur unie
void draw_lignes_map(SDL_Surface* map, int nb_case_col_map, int nb_case_lig_map, Uint32 color_c, Uint32 color_ic)
{
	SDL_Surface* ligne = SDL_CreateRGBSurface(SDL_HWSURFACE, map->w - 2 * TAILLE_BORDURE, TAILLE_INTERCASE, 32, 0, 0, 0, 0);
  SDL_Surface* ligne_case = SDL_CreateRGBSurface(SDL_HWSURFACE, TAILLE_CASE, TAILLE_INTERCASE, 32, 0, 0, 0, 0);
  SDL_Surface* ligne_intercase = SDL_CreateRGBSurface(SDL_HWSURFACE, TAILLE_INTERCASE, TAILLE_INTERCASE, 32, 0, 0, 0, 0);
  SDL_Rect position;
  int i;


  //On initialise nos "morceaux de ligne"
  SDL_FillRect(ligne_case, NULL, SDL_MapRGB(map->format, 0, 0, 200));
  SDL_FillRect(ligne_intercase, NULL, SDL_MapRGB(map->format, 0, 200, 0));


  //On crée une ligne de séparation à partir des "morceaux de ligne"
  position.x = 0;
  position.y = 0;
  for(i=1; i< nb_case_col_map; i++)
  {
    SDL_BlitSurface(ligne_case, NULL, ligne, &position);
    position.x += TAILLE_CASE;
    SDL_BlitSurface(ligne_intercase, NULL, ligne, &position);
    position.x += TAILLE_INTERCASE;
  }
  SDL_BlitSurface(ligne_case, NULL, ligne, &position);
  SDL_FreeSurface(ligne_case);
  SDL_FreeSurface(ligne_intercase);


  //On imprime cette ligne plusieurs fois sur la map pour séparer les cases horizontalement
  position.x = TAILLE_BORDURE;
  position.y = TAILLE_BORDURE;
  for(i=1; i< nb_case_lig_map; i++)
  {
    position.y += TAILLE_CASE;
    SDL_BlitSurface(ligne, NULL, map, &position);
    position.y += TAILLE_INTERCASE;
  }
  SDL_FreeSurface(ligne);
}



//Cette fonction dessine les colonnes sur la map avec une couleur unie
void draw_colonnes_map(SDL_Surface* map, int nb_case_col_map, int nb_case_lig_map, Uint32 color_c, Uint32 color_ic)
{
	SDL_Surface* colonne = SDL_CreateRGBSurface(SDL_HWSURFACE, TAILLE_INTERCASE, map->h - 2 * TAILLE_BORDURE, 32, 0, 0, 0, 0);
  SDL_Surface* colonne_case = SDL_CreateRGBSurface(SDL_HWSURFACE, TAILLE_INTERCASE, TAILLE_CASE, 32, 0, 0, 0, 0);
  SDL_Surface* colonne_intercase = SDL_CreateRGBSurface(SDL_HWSURFACE, TAILLE_INTERCASE, TAILLE_INTERCASE, 32, 0, 0, 0, 0);
  SDL_Rect position;
  int i;


  //On initialise nos "morceaux de colonne"
  SDL_FillRect(colonne_case, NULL, color_c);
  SDL_FillRect(colonne_intercase, NULL, color_ic);


  //On crée une colonne de séparation à partir des "morceaux de colonne"
  position.x = 0;
  position.y = 0;
  for(i=1; i< nb_case_col_map; i++)
  {
    SDL_BlitSurface(colonne_case, NULL, colonne, &position);
    position.y += TAILLE_CASE;
    SDL_BlitSurface(colonne_intercase, NULL, colonne, &position);
    position.y += TAILLE_INTERCASE;
  }
  SDL_BlitSurface(colonne_case, NULL, colonne, &position);
  SDL_FreeSurface(colonne_case);
  SDL_FreeSurface(colonne_intercase);


  //On crée des colonnes pour séparer les cases verticalement
  position.x = TAILLE_BORDURE;
  position.y = TAILLE_BORDURE;
  for(i=1; i< nb_case_col_map; i++)
  {
    position.x += TAILLE_CASE;
    SDL_BlitSurface(colonne, NULL, map, &position);
    position.x += TAILLE_INTERCASE;
  }
  SDL_FreeSurface(colonne);
}



void draw_intercases_map(SDL_Surface* map, int nb_case_col_map, int nb_case_lig_map)
{
  draw_lignes_map(map, nb_case_col_map, nb_case_lig_map, SDL_MapRGB(map->format, 0, 0, 200), SDL_MapRGB(map->format, 0, 200, 0));
  draw_colonnes_map(map, nb_case_col_map, nb_case_lig_map, SDL_MapRGB(map->format, 0, 0, 200), SDL_MapRGB(map->format, 0, 200, 0));
}






//Cette fonction dessine les cases sur la map avec des couleur unie.
//Elle renvoie -1 s'il y a eu une erreur, 0 sinon.
int draw_cases_map(SDL_Surface* map, int nb_case_col_map, int nb_case_lig_map, char* desc_map)
{
  int i = 0, j = 0, k = 0;
  bool continuer = true;
  SDL_Rect position;
  SDL_Surface* case_courante = NULL;
  SDL_Surface* case_vide = SDL_CreateRGBSurface(SDL_HWSURFACE, TAILLE_CASE, TAILLE_CASE, 32, 0, 0, 0, 0);
  SDL_Surface* case_cochee = SDL_CreateRGBSurface(SDL_HWSURFACE, TAILLE_CASE, TAILLE_CASE, 32, 0, 0, 0, 0);
  SDL_Surface* case_joueur[9] =
    {
      SDL_CreateRGBSurface(SDL_HWSURFACE, TAILLE_CASE, TAILLE_CASE, 32, 0, 0, 0, 0),
      SDL_CreateRGBSurface(SDL_HWSURFACE, TAILLE_CASE, TAILLE_CASE, 32, 0, 0, 0, 0),
      SDL_CreateRGBSurface(SDL_HWSURFACE, TAILLE_CASE, TAILLE_CASE, 32, 0, 0, 0, 0),
      SDL_CreateRGBSurface(SDL_HWSURFACE, TAILLE_CASE, TAILLE_CASE, 32, 0, 0, 0, 0),
      SDL_CreateRGBSurface(SDL_HWSURFACE, TAILLE_CASE, TAILLE_CASE, 32, 0, 0, 0, 0),
      SDL_CreateRGBSurface(SDL_HWSURFACE, TAILLE_CASE, TAILLE_CASE, 32, 0, 0, 0, 0),
      SDL_CreateRGBSurface(SDL_HWSURFACE, TAILLE_CASE, TAILLE_CASE, 32, 0, 0, 0, 0),
      SDL_CreateRGBSurface(SDL_HWSURFACE, TAILLE_CASE, TAILLE_CASE, 32, 0, 0, 0, 0),
      SDL_CreateRGBSurface(SDL_HWSURFACE, TAILLE_CASE, TAILLE_CASE, 32, 0, 0, 0, 0),
    };

  SDL_FillRect(case_vide, NULL, SDL_MapRGB(map->format, 255, 255, 255));
  SDL_FillRect(case_cochee, NULL, SDL_MapRGB(map->format, 0, 0, 0));
  SDL_FillRect(case_joueur[0], NULL, SDL_MapRGB(map->format, 0, 0, 255));
  SDL_FillRect(case_joueur[1], NULL, SDL_MapRGB(map->format, 0, 255, 0));
  SDL_FillRect(case_joueur[2], NULL, SDL_MapRGB(map->format, 255, 0, 0));
  SDL_FillRect(case_joueur[3], NULL, SDL_MapRGB(map->format, 0, 255, 255));
  SDL_FillRect(case_joueur[4], NULL, SDL_MapRGB(map->format, 255, 255, 0));
  SDL_FillRect(case_joueur[5], NULL, SDL_MapRGB(map->format, 255, 0, 255));
  SDL_FillRect(case_joueur[6], NULL, SDL_MapRGB(map->format, 127, 127, 127));
  SDL_FillRect(case_joueur[7], NULL, SDL_MapRGB(map->format, 0, 127, 127));
  SDL_FillRect(case_joueur[8], NULL, SDL_MapRGB(map->format, 127, 127, 0));


  position.x = TAILLE_BORDURE;
  position.y = TAILLE_BORDURE;
  while (i < nb_case_lig_map && continuer)
  {
    while (j < nb_case_col_map && continuer)
    {
      switch (desc_map[k])
      {
        case 'O':
          case_courante = case_vide;
          break;
        case 'X':
          case_courante = case_cochee;
          break;
        case '1':
          case_courante = case_joueur[0];
          break;
        case '2':
          case_courante = case_joueur[1];
          break;
        case '3':
          case_courante = case_joueur[2];
          break;
        case '4':
          case_courante = case_joueur[3];
          break;
        case '5':
          case_courante = case_joueur[4];
          break;
        case '6':
          case_courante = case_joueur[5];
          break;
        case '7':
          case_courante = case_joueur[6];
          break;
        case '8':
          case_courante = case_joueur[7];
          break;
        case '9':
          case_courante = case_joueur[8];
          break;
        case '\0' :
          case_courante = case_vide;
          continuer = false;
          fprintf(stderr, "draw_intercases_map : le descripteur de map passé en paramètre fait %d cases, alors que le nombre de colonnes est %d et le nombre de lignes %d\nCase problématique : %d x %d\n", k, nb_case_col_map, nb_case_lig_map, j, i);
          break;
        default :
          case_courante = case_vide;
          continuer = false;
          fprintf(stderr, "draw_intercases_map : le descripteur de map passé en paramètre contient un caractère non valide au caractère numéro %d\nCase problématique : %d x %d\n", k + 1, j, i);
      }
      SDL_BlitSurface(case_courante, NULL, map, &position);
      position.x += TAILLE_CASE + TAILLE_INTERCASE;
      j++; k++;
    }
    position.x = TAILLE_BORDURE;
    j = 0;
    position.y += TAILLE_CASE + TAILLE_INTERCASE;
    i++;
  }

  SDL_FreeSurface(case_vide);
  SDL_FreeSurface(case_cochee);
  SDL_FreeSurface(case_joueur[0]);
  SDL_FreeSurface(case_joueur[1]);
  SDL_FreeSurface(case_joueur[2]);
  SDL_FreeSurface(case_joueur[3]);
  SDL_FreeSurface(case_joueur[4]);
  SDL_FreeSurface(case_joueur[5]);
  SDL_FreeSurface(case_joueur[6]);
  SDL_FreeSurface(case_joueur[7]);
  SDL_FreeSurface(case_joueur[8]);

  if (continuer)
  {
    if (desc_map) return 0;
    else
    {
      fprintf(stderr, "le descripteur de map passé en paramètre fait %d cases, alors que le nombre de colonnes est %d et le nombre de lignes %d", k, nb_case_col_map, nb_case_lig_map);
    }
  }
  else return (-1);
}








//TODO Deleter cette fonction
void pause()
{
    int continuer = 1;
    SDL_Event event;
  
    while (continuer)
    {
        SDL_WaitEvent(&event);
        switch(event.type)
        {
            case SDL_QUIT:
                continuer = 0;
        }
    }
}
