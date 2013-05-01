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


//TODO Liste des fichiers images nécessaires
#define BORDURE_CASE_G "Images/bordure_case_g.bmp"
#define BORDURE_CASE_I "Images/bordure_case_i.bmp"
#define BORDURE_CASE_D "Images/bordure_case_d.bmp"
#define BORDURE_CASE_S "Images/bordure_case_s.bmp"
#define BORDURE_INTERCASE_G "Images/bordure_intercase_g.bmp"
#define BORDURE_INTERCASE_I "Images/bordure_intercase_i.bmp"
#define BORDURE_INTERCASE_D "Images/bordure_intercase_d.bmp"
#define BORDURE_INTERCASE_S "Images/bordure_intercase_s.bmp"
#define BORDURE_COIN_SG "Images/bordure_coin_sg.bmp"
#define BORDURE_COIN_IG "Images/bordure_coin_ig.bmp"
#define BORDURE_COIN_ID "Images/bordure_coin_id.bmp"
#define BORDURE_COIN_SD "Images/bordure_coin_sd.bmp"
#define COLONNE "Images/colonne.bmp"
#define LIGNE "Images/ligne.bmp"
#define CROISEMENT "Images/croisement.bmp"
#define CASE_VIDE "Images/case_vide.bmp"
#define CASE_COCHEE "Images/case_cochee.bmp"
#define CASE_JOUEUR_1 "Images/case_joueur_1.bmp"
#define CASE_JOUEUR_2 "Images/case_joueur_2.bmp"
#define CASE_JOUEUR_3 "Images/case_joueur_3.bmp"
#define CASE_JOUEUR_4 "Images/case_joueur_4.bmp"
#define CASE_JOUEUR_5 "Images/case_joueur_5.bmp"
#define CASE_JOUEUR_6 "Images/case_joueur_6.bmp"
#define CASE_JOUEUR_7 "Images/case_joueur_7.bmp"
#define CASE_JOUEUR_8 "Images/case_joueur_8.bmp"
#define CASE_JOUEUR_9 "Images/case_joueur_9.bmp"



//Variables globales : surfaces et tailles de référence
SDL_Surface* bordure_case_g = NULL;
SDL_Surface* bordure_case_i = NULL;
SDL_Surface* bordure_case_d = NULL;
SDL_Surface* bordure_case_s = NULL;
SDL_Surface* bordure_intercase_g = NULL;
SDL_Surface* bordure_intercase_i = NULL;
SDL_Surface* bordure_intercase_d = NULL;
SDL_Surface* bordure_intercase_s = NULL;
SDL_Surface* bordure_coin_sg = NULL;
SDL_Surface* bordure_coin_ig = NULL;
SDL_Surface* bordure_coin_id = NULL;
SDL_Surface* bordure_coin_sd = NULL;
SDL_Surface* colonne = NULL;
SDL_Surface* ligne = NULL;
SDL_Surface* croisement = NULL;
SDL_Surface* case_vide = NULL;
SDL_Surface* case_cochee = NULL;
SDL_Surface* case_joueur[9] = {NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL};

int taille_bordure;
int taille_case;
int taille_intercase;



//Prototypes de fonctions
void initialisation();
void arret();
void pause();
void draw_bordures_map(SDL_Surface*, int, int);
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
  int largeur_map;
  int hauteur_map;
  int i; //TODO Delete
  char* desc_map = malloc ((nb_case_tot_map + 1) * sizeof(char));

  SDL_Surface* liste_joueurs = NULL;
  int hauteur_liste_joueurs;
  int largeur_liste_joueurs;




  //Initialisation (chargement de la SDL, chargement des surfaces de référence, calcul des tailles de référence)
  initialisation();


  


  //Création de la zone de l'écran qui contiendra la liste des joueurs
  hauteur_liste_joueurs = 1; //TODO
  largeur_liste_joueurs = 50;
  liste_joueurs = SDL_CreateRGBSurface(SDL_HWSURFACE, largeur_liste_joueurs, hauteur_liste_joueurs, 32, 0, 0, 0, 0);
  //TODO initialisation de l'affichage

  

  //Création de la zone de l'écran qui contiendra la map
  largeur_map = 2 * taille_bordure + nb_case_col_map * (taille_case + taille_intercase) - taille_intercase;
  hauteur_map = 2 * taille_bordure + nb_case_lig_map * (taille_case + taille_intercase) - taille_intercase;
  map = SDL_CreateRGBSurface(SDL_HWSURFACE, largeur_map, hauteur_map, 32, 0, 0, 0, 0);
  draw_bordures_map(map, nb_case_col_map, nb_case_lig_map);
  draw_intercases_map(map, nb_case_col_map, nb_case_lig_map);
  //TODO Delete
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
  ecran = SDL_SetVideoMode(largeur_liste_joueurs + largeur_map, hauteur_map, 32, SDL_HWSURFACE | SDL_DOUBLEBUF); // On tente d'ouvrir une fenêtre en 32 bit sur la mémoire vidéo en double buffering
  if (ecran == NULL) // Si l'ouverture a échoué, on le note et on arrête
  {
      fprintf(stderr, "Impossible de charger le mode vidéo : %s\n", SDL_GetError());
      exit(EXIT_FAILURE);
  }



  //Options de la fenêtre
  SDL_WM_SetCaption("Puissance 5 en réseau", "Puissance 5 en réseau"); //Titre de la fenêtre, et de l'icône
  SDL_WM_SetIcon(case_joueur[0], NULL); //Icône de la fenêtre
	SDL_FillRect(ecran, NULL, SDL_MapRGB(ecran->format, 255, 255, 255)); //Couleur de fond, blanc



  //Collage de la liste des joueurs et de la map sur la fenêtre
  position.x = 0;
  position.y = 0;
  SDL_BlitSurface(liste_joueurs, NULL, ecran, &position);
  position.x = largeur_liste_joueurs;
  position.y = 0;
  SDL_BlitSurface(map, NULL, ecran, &position);

	SDL_Flip(ecran);


  pause();


  //Arrêt de la SDL et libération de la mémoire des surfaces de référence
  SDL_FreeSurface(liste_joueurs);
  SDL_FreeSurface(map);
  arret();
	return EXIT_SUCCESS;
}










//Fontion d'initialisation :
//  - chargement de la SDL
//  - chargement des surfaces de référence
//  - calcul des tailles de référence et tests de compatibilité
void initialisation()
{
  //Démarrage de la SDL.
	if (SDL_Init(SDL_INIT_VIDEO) == -1) // Si erreur :
	{
  	fprintf(stderr, "Erreur d'initialisation de la SDL : %s\n", SDL_GetError()); // Écriture de l'erreur
 	  exit(EXIT_FAILURE); // On quitte le programme
	}
  atexit(SDL_Quit); // Si on rencontre un exit on arrête la SDL avant.



  //Création des surfaces de référence
  //TODO tests  pour vérifier que les fichiers sont bien ouverts
  bordure_case_g = SDL_LoadBMP(BORDURE_CASE_G);
  bordure_case_i = SDL_LoadBMP(BORDURE_CASE_I);
  bordure_case_d = SDL_LoadBMP(BORDURE_CASE_D);
  bordure_case_s = SDL_LoadBMP(BORDURE_CASE_S);
  bordure_intercase_g = SDL_LoadBMP(BORDURE_INTERCASE_G);
  bordure_intercase_i = SDL_LoadBMP(BORDURE_INTERCASE_I);
  bordure_intercase_d = SDL_LoadBMP(BORDURE_INTERCASE_D);
  bordure_intercase_s = SDL_LoadBMP(BORDURE_INTERCASE_S);
  bordure_coin_sg = SDL_LoadBMP(BORDURE_COIN_SG);
  bordure_coin_ig = SDL_LoadBMP(BORDURE_COIN_IG);
  bordure_coin_id = SDL_LoadBMP(BORDURE_COIN_ID);
  bordure_coin_sd = SDL_LoadBMP(BORDURE_COIN_SD);
  colonne = SDL_LoadBMP(COLONNE);
  ligne = SDL_LoadBMP(LIGNE);
  croisement = SDL_LoadBMP(CROISEMENT);
  case_vide = SDL_LoadBMP(CASE_VIDE);
  case_cochee = SDL_LoadBMP(CASE_COCHEE);
  case_joueur[0] = SDL_LoadBMP(CASE_JOUEUR_1);
  case_joueur[1] = SDL_LoadBMP(CASE_JOUEUR_2);
  case_joueur[2] = SDL_LoadBMP(CASE_JOUEUR_3);
  case_joueur[3] = SDL_LoadBMP(CASE_JOUEUR_4);
  case_joueur[4] = SDL_LoadBMP(CASE_JOUEUR_5);
  case_joueur[5] = SDL_LoadBMP(CASE_JOUEUR_6);
  case_joueur[6] = SDL_LoadBMP(CASE_JOUEUR_7);
  case_joueur[7] = SDL_LoadBMP(CASE_JOUEUR_8);
  case_joueur[8] = SDL_LoadBMP(CASE_JOUEUR_9);



  //TODO Calcul des différentes tailles et vérifications de compatibilités
  taille_bordure = bordure_case_g->w;
  if (taille_bordure != bordure_case_i->h)
  {
    fprintf(stderr,"bordure_case_g et bordure_case_i n'ont pas la même largeur de bordure.\n");
    exit (-1);
  }
  if (taille_bordure != bordure_case_d->w)
  {
    fprintf(stderr,"bordure_case_d n'a pas la bonne largeur de bordure.\n");
    exit (-1);
  }
  if (taille_bordure != bordure_case_s->h)
  {
    fprintf(stderr,"bordure_case_s n'a pas la bonne largeur de bordure.\n");
    exit (-1);
  }
  if (taille_bordure != bordure_intercase_g->w)
  {
    fprintf(stderr,"bordure_intercase_g n'a pas la bonne largeur de bordure.\n");
    exit (-1);
  }
  if (taille_bordure != bordure_intercase_i->h)
  {
    fprintf(stderr,"bordure_intercase_i n'a pas la bonne largeur de bordure.\n");
    exit (-1);
  }
  if (taille_bordure != bordure_intercase_d->w)
  {
    fprintf(stderr,"bordure_intercase_d n'a pas la bonne largeur de bordure.\n");
    exit (-1);
  }
  if (taille_bordure != bordure_intercase_s->h)
  {
    fprintf(stderr,"bordure_intercase_s n'a pas la bonne largeur de bordure.\n");
    exit (-1);
  }
  if (taille_bordure != bordure_coin_sg->w || taille_bordure != bordure_coin_sg->h)
  {
    fprintf(stderr,"bordure_coin_sg n'a pas la bonne largeur de bordure.\n");
    exit (-1);
  }
  if (taille_bordure != bordure_coin_ig->w || taille_bordure != bordure_coin_ig->h)
  {
    fprintf(stderr,"bordure_coin_ig n'a pas la bonne largeur de bordure.\n");
    exit (-1);
  }
  if (taille_bordure != bordure_coin_id->w || taille_bordure != bordure_coin_id->h)
  {
    fprintf(stderr,"bordure_coin_id n'a pas la bonne largeur de bordure.\n");
    exit (-1);
  }
  if (taille_bordure != bordure_coin_sd->w || taille_bordure != bordure_coin_sd->h)
  {
    fprintf(stderr,"bordure_coin_sd n'a pas la bonne largeur de bordure.\n");
    exit (-1);
  }

  taille_case = case_joueur[0]->w;

  taille_intercase = bordure_intercase_g->h;
}











void arret()
{
  int i;
  SDL_FreeSurface(bordure_case_g);
  SDL_FreeSurface(bordure_case_i);
  SDL_FreeSurface(bordure_case_d);
  SDL_FreeSurface(bordure_case_s);
  SDL_FreeSurface(bordure_intercase_g);
  SDL_FreeSurface(bordure_intercase_i);
  SDL_FreeSurface(bordure_intercase_d);
  SDL_FreeSurface(bordure_intercase_s);
  SDL_FreeSurface(bordure_coin_sg);
  SDL_FreeSurface(bordure_coin_ig);
  SDL_FreeSurface(bordure_coin_id);
  SDL_FreeSurface(bordure_coin_sd);
  SDL_FreeSurface(colonne);
  SDL_FreeSurface(ligne);
  SDL_FreeSurface(croisement);
  SDL_FreeSurface(case_vide);
  SDL_FreeSurface(case_cochee);
  for(i=0; i<9; i++) SDL_FreeSurface(case_joueur[i]);
	SDL_Quit();
}












//Cette fonction dessine les bordures de la map en utilisant les images fournies
void draw_bordures_map(SDL_Surface* map, int nb_case_col_map, int nb_case_lig_map)
{
  SDL_Rect position;
  int i;

  //Coin supérieur gauche
  position.x = 0;
  position.y = 0;
  SDL_BlitSurface(bordure_coin_sg, NULL, map, &position);
  position.y += taille_bordure;

  //Bordure gauche
  for(i=1; i< nb_case_lig_map; i++)
  {
    SDL_BlitSurface(bordure_case_g, NULL, map, &position);
    position.y += taille_case;
    SDL_BlitSurface(bordure_intercase_g, NULL, map, &position);
    position.y += taille_intercase;
  }
  SDL_BlitSurface(bordure_case_g, NULL, map, &position);
    position.y += taille_case;

  //Coin inférieur gauche
  SDL_BlitSurface(bordure_coin_ig, NULL, map, &position);
  position.x += taille_bordure;

  //Bordure inférieure
  for(i=1; i< nb_case_col_map; i++)
  {
    SDL_BlitSurface(bordure_case_i, NULL, map, &position);
    position.x += taille_case;
    SDL_BlitSurface(bordure_intercase_i, NULL, map, &position);
    position.x += taille_intercase;
  }
  SDL_BlitSurface(bordure_case_i, NULL, map, &position);
  position.x += taille_case;

  //Coin inférieur droit
  SDL_BlitSurface(bordure_coin_id, NULL, map, &position);

  //Bordure droite
  for(i=1; i< nb_case_lig_map; i++)
  {
    position.y -= taille_case;
    SDL_BlitSurface(bordure_case_d, NULL, map, &position);
    position.y -= taille_intercase;
    SDL_BlitSurface(bordure_intercase_d, NULL, map, &position);
  }
  position.y -= taille_case;
  SDL_BlitSurface(bordure_case_d, NULL, map, &position);

  //Coin supérieur droit
  position.y -= taille_bordure;
  SDL_BlitSurface(bordure_coin_sd, NULL, map, &position);

  //Bordure supérieure
  for(i=1; i< nb_case_col_map; i++)
  {
    position.x -= taille_case;
    SDL_BlitSurface(bordure_case_s, NULL, map, &position);
    position.x -= taille_intercase;
    SDL_BlitSurface(bordure_intercase_s, NULL, map, &position);
  }
  position.x -= taille_case;
  SDL_BlitSurface(bordure_case_s, NULL, map, &position);
}








//Cette fonction dessine les intercases sur la map en utilisant les images fournies
void draw_intercases_map(SDL_Surface* map, int nb_case_col_map, int nb_case_lig_map)
{
	SDL_Surface* intercase;
  SDL_Rect position;
  int i;

  
  //On crée une colonne de séparation à partir des "morceaux de colonne"
  intercase = SDL_CreateRGBSurface(SDL_HWSURFACE, taille_intercase, map->h - 2 * taille_bordure, 32, 0, 0, 0, 0);
  position.x = 0;
  position.y = 0;
  for(i=1; i< nb_case_lig_map; i++)
  {
    SDL_BlitSurface(colonne, NULL, intercase, &position);
    position.y += taille_case;
    SDL_BlitSurface(croisement, NULL, intercase, &position);
    position.y += taille_intercase;
  }
  SDL_BlitSurface(colonne, NULL, intercase, &position);


  //On crée des colonnes pour séparer les cases verticalement
  position.x = taille_bordure;
  position.y = taille_bordure;
  for(i=1; i< nb_case_col_map; i++)
  {
    position.x += taille_case;
    SDL_BlitSurface(intercase, NULL, map, &position);
    position.x += taille_intercase;
  }
  SDL_FreeSurface(intercase);

  
  //On crée une ligne de séparation à partir des "morceaux de ligne"
  intercase = SDL_CreateRGBSurface(SDL_HWSURFACE, map->w - 2 * taille_bordure, taille_intercase, 32, 0, 0, 0, 0);
  position.x = 0;
  position.y = 0;
  for(i=1; i< nb_case_col_map; i++)
  {
    SDL_BlitSurface(ligne, NULL, intercase, &position);
    position.x += taille_case;
    SDL_BlitSurface(croisement, NULL, intercase, &position);
    position.x += taille_intercase;
  }
  SDL_BlitSurface(ligne, NULL, intercase, &position);


  //On crée des lignes pour séparer les cases horizontalement
  position.x = taille_bordure;
  position.y = taille_bordure;
  for(i=1; i< nb_case_col_map; i++)
  {
    position.y += taille_case;
    SDL_BlitSurface(intercase, NULL, map, &position);
    position.y += taille_intercase;
  }
  SDL_FreeSurface(intercase);
}







//Cette fonction dessine les cases sur la map en utilisant les images fournies
//Elle renvoie -1 s'il y a eu une erreur, 0 sinon.
int draw_cases_map(SDL_Surface* map, int nb_case_col_map, int nb_case_lig_map, char* desc_map)
{
  int i = 0, j = 0, k = 0;
  bool continuer = true;
  SDL_Rect position;
  SDL_Surface* case_courante = NULL;


  position.x = taille_bordure;
  position.y = taille_bordure;
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
      position.x += taille_case + taille_intercase;
      j++; k++;
    }
    position.x = taille_bordure;
    j = 0;
    position.y += taille_case + taille_intercase;
    i++;
  }

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
