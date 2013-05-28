/***************************************************/
/*      Projet  IPI S2 - 2013 : puissance 5        */
/*                    Groupe G                     */
/*          Nathalie Au, Lewin Alexandre,          */
/*       Beaume Jérémy et Métraud Alexandre        */
/*                   Groupe 4.2                    */
/*           COMUSR, interface graphique           */
/***************************************************/

/*
  Ce fichier comporte des tests sur l'interface graphique du client.
  La fenêtre graphique en jeu est séparée en deux parties : la map et la liste des joueurs
*/

/*Commande pour compiler avec la SDL
gcc -pthread -Wall -o interface_graphique interface_graphique.c -lSDLmain -lSDL -lSDL_ttf
gcc -Wall -o interface_graphique interface_graphique.c `sdl-config --cflags --libs`
*/

#include <stdlib.h>
#include <stdio.h>
#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
#include <pthread.h>
int XInitThreads();

//Liste des fichiers images nécessaires et des paramètres
#include "interface_graphique_parametres.h"

#define GUILLEMETS(x) #x
#define FICHIER(chemin,nom_fichier) GUILLEMETS(chemin/nom_fichier)
#define IMAGE(x) FICHIER(DOSSIER_IMAGES,x)

#define true 1
#define false 0
typedef short bool;













//Variables globales : surfaces et tailles de référence
SDL_Surface* ecran = NULL;
SDL_Surface* map = NULL;
SDL_Surface* liste_joueurs = NULL;
SDL_Surface* menu = NULL;
SDL_Surface* fond_ecran = NULL;

SDL_Surface* fond_liste_joueurs = NULL;
SDL_Surface* cadre_g = NULL;
SDL_Surface* cadre_i = NULL;
SDL_Surface* cadre_d = NULL;
SDL_Surface* cadre_s = NULL;
SDL_Surface* cadre_coin_sg = NULL;
SDL_Surface* cadre_coin_ig = NULL;
SDL_Surface* cadre_coin_id = NULL;
SDL_Surface* cadre_coin_sd = NULL;
TTF_Font* police = NULL;

SDL_Surface* fond_map = NULL;
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

SDL_Surface* bouton_valider_grise = NULL;
SDL_Surface* bouton_valider_dispo = NULL;

SDL_Surface* case_vide = NULL;
SDL_Surface* case_cochee = NULL;
SDL_Surface* case_derniere_jouee = NULL;
SDL_Surface* case_tontour = NULL;
SDL_Surface* case_selectionnee = NULL;
SDL_Surface* case_joueur[9] = {NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL};

int largeur_cadre_g;
int hauteur_cadre_i;
int largeur_cadre_d;
int hauteur_cadre_s;
int hauteur_max;
int hauteur_max_pseudo;
int largeur_max_pseudo;

int largeur_bordure_g;
int hauteur_bordure_i;
int largeur_bordure_d;
int hauteur_bordure_s;
int largeur_intercase;
int hauteur_intercase;
int largeur_case;
int hauteur_case;

int largeur_bouton_valider;
int hauteur_bouton_valider;

bool initialise = false;
char* scores[9];
SDL_Surface* scores_affiches[9] = {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};
int nb_joueurs;
int nb_case_col_map;
int nb_case_lig_map;







//Prototypes et hiérarchie des fonctions (seules les fonctions non-indentées sont utilisables de l'extérieur.
int initialisation_sdl(int, char**, int, int);
  void demarrage();
    void chargement_images();
    void calcul_tailles();
  int init_zones(char**);
    void init_liste_joueurs(char**);
      void init_scores(char**);
      void draw_cadre_liste_joueurs();
      void draw_scores_liste_joueurs(int);
    int init_map();
      void draw_bordures_map();
      void draw_intercases_map();
      int draw_cases_map(char*);
  void init_menu(int);
  void affiche();

int update_sdl(int, char*, char*);
  int update_score(int, int, char*);
  //int draw_cases_map(char*);
  //void affiche()

void a_toi_de_jouer(int*, int*, char*);
  void* switch_message(void*);
  int check_mouse(bool, int*, int*, int, int, char*);

void attendre_fermeture_sdl();

void arret_sdl();

void pause_sdl(); //TODO rm













//Fonction d'initialisation :
//  - chargement de la SDL
//  - chargement des surfaces et de la police de référence
//  - calcul des tailles de référence
//  - création des zones et de la fenêtre
//  - affichage de la fenêtre
int initialisation_sdl(int nombre_joueurs, char** pseudos, int nombre_case_col_map, int nombre_case_lig_map)
{
  int i; //FIXME rm
  if (initialise)
  {
    fprintf(stderr, "Tentative de réinitialisation\n");
    return 0;
  }
  printf("Démarrage de la sdl.\n");
  int res;
  printf("nb joueurs : %d, nb_col_map : %d, nb_lig_map : %d\n", nombre_joueurs, nombre_case_col_map, nombre_case_lig_map);
  for (i=0; i < nombre_joueurs; i++) printf("Pseudo joueur %d : %s\n", i, pseudos[i]);

  nb_joueurs = nombre_joueurs;
  nb_case_col_map = nombre_case_col_map;
  nb_case_lig_map = nombre_case_lig_map;
  initialise = true;

  demarrage();
  res = init_zones(pseudos);
  affiche();
  return res;
}




//Sous-fontion d'initialisation :
//  - chargement de la SDL
//  - appel à la fonction de chargement des surfaces et de la police de référence
//  - appel à la fonction de calcul des tailles de référence et tests de compatibilité
void demarrage()
{
  if(!XInitThreads())
  {
    fprintf(stderr, "Erreur d'exécution de XInitThreads\n");
    exit(EXIT_FAILURE);
  }

  //Démarrage de la SDL.
  if (SDL_Init(SDL_INIT_VIDEO) == -1) // Si erreur :
  {
    fprintf(stderr, "Erreur d'initialisation de la SDL : %s\n", SDL_GetError());
    exit(EXIT_FAILURE);
  }
  atexit(SDL_Quit); // Si on rencontre un exit on arrête la SDL avant.

  //Démarrage de SDL_ttf
  if(TTF_Init() == -1)
  {
    fprintf(stderr, "Erreur d'initialisation de TTF : %s\n", TTF_GetError());
    exit(EXIT_FAILURE);
  }
  atexit(TTF_Quit); // Si on rencontre un exit on arrête la SDL avant.

  atexit(arret_sdl); // Si on rencontre un exit effacer toutes les allocations.

  chargement_images();
  calcul_tailles();
}



//Fonction de chargement des images et de création des surfaces et de la police de référence
void chargement_images()
{
  //On vérifie que les images n'ont pas déjà été initialisées
  if (
       (fond_ecran)
    && (fond_liste_joueurs)
    && (cadre_g)
    && (cadre_i)
    && (cadre_d)
    && (cadre_s)
    && (cadre_coin_sg)
    && (cadre_coin_ig)
    && (cadre_coin_id)
    && (cadre_coin_sd)
    && (fond_map)
    && (bordure_case_g)
    && (bordure_case_i)
    && (bordure_case_d)
    && (bordure_case_s)
    && (bordure_intercase_g)
    && (bordure_intercase_i)
    && (bordure_intercase_d)
    && (bordure_intercase_s)
    && (bordure_coin_sg)
    && (bordure_coin_ig)
    && (bordure_coin_id)
    && (bordure_coin_sd)
    && (colonne)
    && (ligne)
    && (croisement)
    && (case_vide)
    && (case_cochee)
    && (case_derniere_jouee)
    && (case_tontour)
    && (case_selectionnee)
    && (case_joueur[0])
    && (case_joueur[1])
    && (case_joueur[2])
    && (case_joueur[3])
    && (case_joueur[4])
    && (case_joueur[5])
    && (case_joueur[6])
    && (case_joueur[7])
    && (case_joueur[8])
    && (bouton_valider_grise)
    && (bouton_valider_dispo) 
    && (police)
  )
  //Si les images ont déjà été chargées, on annule et on affiche un message d'erreur
  {
    fprintf(stderr, "Tentative de réinitialisation des images et de la police.\n");
    return;
  }

  //On charge les images, et on vérifie qu'elles ont été bien chargées.
  if (!(

       (fond_ecran = SDL_LoadBMP(IMAGE(FOND_ECRAN)))
    && (fond_liste_joueurs = SDL_LoadBMP(IMAGE(FOND_LISTE_JOUEURS)))
    && (cadre_g = SDL_LoadBMP(IMAGE(CADRE_G)))
    && (cadre_i = SDL_LoadBMP(IMAGE(CADRE_I)))
    && (cadre_d = SDL_LoadBMP(IMAGE(CADRE_D)))
    && (cadre_s = SDL_LoadBMP(IMAGE(CADRE_S)))
    && (cadre_coin_sg = SDL_LoadBMP(IMAGE(CADRE_COIN_SG)))
    && (cadre_coin_ig = SDL_LoadBMP(IMAGE(CADRE_COIN_IG)))
    && (cadre_coin_id = SDL_LoadBMP(IMAGE(CADRE_COIN_ID)))
    && (cadre_coin_sd = SDL_LoadBMP(IMAGE(CADRE_COIN_SD)))
    && (fond_map = SDL_LoadBMP(IMAGE(FOND_MAP)))
    && (bordure_case_g = SDL_LoadBMP(IMAGE(BORDURE_CASE_G)))
    && (bordure_case_i = SDL_LoadBMP(IMAGE(BORDURE_CASE_I)))
    && (bordure_case_d = SDL_LoadBMP(IMAGE(BORDURE_CASE_D)))
    && (bordure_case_s = SDL_LoadBMP(IMAGE(BORDURE_CASE_S)))
    && (bordure_intercase_g = SDL_LoadBMP(IMAGE(BORDURE_INTERCASE_G)))
    && (bordure_intercase_i = SDL_LoadBMP(IMAGE(BORDURE_INTERCASE_I)))
    && (bordure_intercase_d = SDL_LoadBMP(IMAGE(BORDURE_INTERCASE_D)))
    && (bordure_intercase_s = SDL_LoadBMP(IMAGE(BORDURE_INTERCASE_S)))
    && (bordure_coin_sg = SDL_LoadBMP(IMAGE(BORDURE_COIN_SG)))
    && (bordure_coin_ig = SDL_LoadBMP(IMAGE(BORDURE_COIN_IG)))
    && (bordure_coin_id = SDL_LoadBMP(IMAGE(BORDURE_COIN_ID)))
    && (bordure_coin_sd = SDL_LoadBMP(IMAGE(BORDURE_COIN_SD)))
    && (colonne = SDL_LoadBMP(IMAGE(COLONNE)))
    && (ligne = SDL_LoadBMP(IMAGE(LIGNE)))
    && (croisement = SDL_LoadBMP(IMAGE(CROISEMENT)))
    && (case_vide = SDL_LoadBMP(IMAGE(CASE_VIDE)))
    && (case_cochee = SDL_LoadBMP(IMAGE(CASE_COCHEE)))
    && (case_derniere_jouee = SDL_LoadBMP(IMAGE(CASE_DERNIERE_JOUEE)))
    && (case_tontour = SDL_LoadBMP(IMAGE(CASE_TONTOUR)))
    && (case_selectionnee = SDL_LoadBMP(IMAGE(CASE_SELECTIONNEE)))
    && (case_joueur[0] = SDL_LoadBMP(IMAGE(CASE_JOUEUR_1)))
    && (case_joueur[1] = SDL_LoadBMP(IMAGE(CASE_JOUEUR_2)))
    && (case_joueur[2] = SDL_LoadBMP(IMAGE(CASE_JOUEUR_3)))
    && (case_joueur[3] = SDL_LoadBMP(IMAGE(CASE_JOUEUR_4)))
    && (case_joueur[4] = SDL_LoadBMP(IMAGE(CASE_JOUEUR_5)))
    && (case_joueur[5] = SDL_LoadBMP(IMAGE(CASE_JOUEUR_6)))
    && (case_joueur[6] = SDL_LoadBMP(IMAGE(CASE_JOUEUR_7)))
    && (case_joueur[7] = SDL_LoadBMP(IMAGE(CASE_JOUEUR_8)))
    && (case_joueur[8] = SDL_LoadBMP(IMAGE(CASE_JOUEUR_9)))
    && (bouton_valider_grise = SDL_LoadBMP(IMAGE(BOUTON_VALIDER_GRISE)))
    && (bouton_valider_dispo = SDL_LoadBMP(IMAGE(BOUTON_VALIDER_DISPO)))
  ))
  {
    fprintf(stderr, "Erreur de chargement d'une image : %s\n", SDL_GetError());
    arret_sdl();
    exit(EXIT_FAILURE);
  }

  if (!(police = TTF_OpenFont(POLICE, TAILLE_POLICE)))
  {
    fprintf(stderr, "Erreur de chargement de la police : %s\n", TTF_GetError());
    arret_sdl();
    exit(EXIT_FAILURE);    
  }
}


//Fonction d'initialisation des tailles
void calcul_tailles()
{
  int i;

  //Initialisation des tableaux de valeurs qui doivent être de même taille
  //  (cette taille sera mise dans la variable globale du même nom en remplaçant h par hauteur et l par largeur)
  int un[4] = {cadre_g->h, cadre_i->w, cadre_d->h, cadre_s->w};
  int l_cadre_g[3] = {cadre_coin_sg->w, cadre_g->w, cadre_coin_ig->w};
  int h_cadre_i[3] = {cadre_coin_ig->h, cadre_i->h, cadre_coin_id->h};
  int l_cadre_d[3] = {cadre_coin_id->w, cadre_d->w, cadre_coin_sd->w};
  int h_cadre_s[3] = {cadre_coin_sd->h, cadre_s->h, cadre_coin_sg->h};
  int l_bordure_g[4] = {bordure_coin_sg->w, bordure_case_g->w, bordure_intercase_g->w, bordure_coin_ig->w};
  int h_bordure_i[4] = {bordure_coin_ig->h, bordure_case_i->h, bordure_intercase_i->h, bordure_coin_id->h};
  int l_bordure_d[4] = {bordure_coin_id->w, bordure_case_d->w, bordure_intercase_d->w, bordure_coin_sd->w};
  int h_bordure_s[4] = {bordure_coin_sd->h, bordure_case_s->h, bordure_intercase_s->h, bordure_coin_sg->h};
  int h_intercase[4] = {bordure_intercase_g->h, bordure_intercase_d->h, ligne->h, croisement->h};
  int l_intercase[4] = {bordure_intercase_i->w, bordure_intercase_s->w, colonne->w, croisement->w};
  int h_case[17] = {case_vide->h, case_cochee->h, case_derniere_jouee->h, case_tontour->h, case_selectionnee->h, case_joueur[0]->h, case_joueur[1]->h, case_joueur[2]->h, case_joueur[3]->h, case_joueur[4]->h, case_joueur[5]->h, case_joueur[6]->h, case_joueur[7]->h, case_joueur[8]->h, bordure_case_g->h, bordure_case_d->h, colonne->h};
  int l_case[17] = {case_vide->w, case_cochee->w, case_derniere_jouee->w, case_tontour->w, case_selectionnee->h, case_joueur[0]->w, case_joueur[1]->w, case_joueur[2]->w, case_joueur[3]->w, case_joueur[4]->w, case_joueur[5]->w, case_joueur[6]->w, case_joueur[7]->w, case_joueur[8]->w, bordure_case_i->w, bordure_case_s->w, ligne->w};

  //Initialisation des variables et vérification que les tailles sont compatibles
  //  En cas d'erreur, l'indice est celui d'un des tableau ci-dessus (le tableau correspondant à l'erreur.
  //  Par exemple, si le message d'erreur est :
  //   Problème de compatibilité des largeurs de bordure gauche. Indice : 2
  //  Cela veut dire que bordure_coin_sg->w et bordure_case_g->w sont égales, mais que bordure_intercase_g->w a une valeur différente, ce qui n'est pas normal.
  for(i=0; i<4; i++)
  {
    if (1 != un[i])
    {
      fprintf(stderr,"Problème de compatibilité des épaisseurs de cadre.\nTaille attendue : 1px, taille effective : %dpx\nIndice : %d. (cf code source pour le traitement de l'erreur)\n", un[i], i);
      arret_sdl();
      exit(EXIT_FAILURE);
    }
  }

  largeur_cadre_g = l_cadre_g[0];
  for(i=1; i<3; i++)
  {
    if (largeur_cadre_g != l_cadre_g[i])
    {
      fprintf(stderr,"Problème de compatibilité des largeurs de cadre gauche.\nTaille attendue : %dpx, taille effective : %dpx\nIndice : %d. (cf code source pour le traitement de l'erreur)\n", largeur_cadre_g, l_cadre_g[i], i);
      arret_sdl();
      exit(EXIT_FAILURE);
    }
  }

  hauteur_cadre_i = h_cadre_i[0];
  for(i=1; i<3; i++)
  {
    if (hauteur_cadre_i != h_cadre_i[i])
    {
      fprintf(stderr,"Problème de compatibilité des hauteurs de cadre inférieure.\nTaille attendue : %dpx, taille effective : %dpx\nIndice : %d. (cf code source pour le traitement de l'erreur)\n", hauteur_cadre_i, h_cadre_i[i], i);
      arret_sdl();
      exit(EXIT_FAILURE);
    }
  }

  largeur_cadre_d = l_cadre_d[0];
  for(i=1; i<3; i++)
  {
    if (largeur_cadre_d != l_cadre_d[i])
    {
      fprintf(stderr,"Problème de compatibilité des largeurs de cadre droite.\nTaille attendue : %dpx, taille effective : %dpx\nIndice : %d. (cf code source pour le traitement de l'erreur)\n", largeur_cadre_d, l_cadre_d[i], i);
      arret_sdl();
      exit(EXIT_FAILURE);
    }
  }

  hauteur_cadre_s = h_cadre_s[0];
  for(i=1; i<3; i++)
  {
    if (hauteur_cadre_s != h_cadre_s[i])
    {
      fprintf(stderr,"Problème de compatibilité des hauteurs de cadre supérieure.\nTaille attendue : %dpx, taille effective : %dpx\nIndice : %d. (cf code source pour le traitement de l'erreur)\n", hauteur_cadre_s, h_cadre_s[i], i);
      arret_sdl();
      exit(EXIT_FAILURE);
    }
  }

  largeur_bordure_g = l_bordure_g[0];
  for(i=1; i<4; i++)
  {
    if (largeur_bordure_g != l_bordure_g[i])
    {
      fprintf(stderr,"Problème de compatibilité des largeurs de bordure gauche.\nTaille attendue : %dpx, taille effective : %dpx\nIndice : %d. (cf code source pour le traitement de l'erreur)\n", largeur_bordure_g, l_bordure_g[i], i);
      arret_sdl();
      exit(EXIT_FAILURE);
    }
  }

  hauteur_bordure_i = h_bordure_i[0];
  for(i=1; i<4; i++)
  {
    if (hauteur_bordure_i != h_bordure_i[i])
    {
      fprintf(stderr,"Problème de compatibilité des hauteurs de bordure inférieure.\nTaille attendue : %dpx, taille effective : %dpx\nIndice : %d. (cf code source pour le traitement de l'erreur)\n", hauteur_bordure_i, h_bordure_i[i], i);
      arret_sdl();
      exit(EXIT_FAILURE);
    }
  }

  largeur_bordure_d = l_bordure_d[0];
  for(i=1; i<4; i++)
  {
    if (largeur_bordure_d != l_bordure_d[i])
    {
      fprintf(stderr,"Problème de compatibilité des largeurs de bordure droite.\nTaille attendue : %dpx, taille effective : %dpx\nIndice : %d. (cf code source pour le traitement de l'erreur)\n", largeur_bordure_d, l_bordure_d[i], i);
      arret_sdl();
      exit(EXIT_FAILURE);
    }
  }

  hauteur_bordure_s = h_bordure_s[0];
  for(i=1; i<4; i++)
  {
    if (hauteur_bordure_s != h_bordure_s[i])
    {
      fprintf(stderr,"Problème de compatibilité des hauteurs de bordure supérieure.\nTaille attendue : %dpx, taille effective : %dpx\nIndice : %d. (cf code source pour le traitement de l'erreur)\n", hauteur_bordure_s, h_bordure_s[i], i);
      arret_sdl();
      exit(EXIT_FAILURE);
    }
  }

  hauteur_intercase = h_intercase[0];
  for(i=1; i<4; i++)
  {
    if (hauteur_intercase != h_intercase[i])
    {
      fprintf(stderr,"Problème de compatibilité des hauteurs d'intercase.\nTaille attendue : %dpx, taille effective : %dpx\nIndice : %d. (cf code source pour le traitement de l'erreur)\n", hauteur_intercase, h_intercase[i], i);
      arret_sdl();
      exit(EXIT_FAILURE);
    }
  }

  largeur_intercase = l_intercase[0];
  for(i=1; i<4; i++)
  {
    if (largeur_intercase != l_intercase[i])
    {
      fprintf(stderr,"Problème de compatibilité des largeurs d'intercase.\nTaille attendue : %dpx, taille effective : %dpx\nIndice : %d. (cf code source pour le traitement de l'erreur)\n", largeur_intercase, l_intercase[i], i);
      arret_sdl();
      exit(EXIT_FAILURE);
    }
  }

  hauteur_case = h_case[0];
  for(i=1; i<17; i++)
  {
    if (hauteur_case != h_case[i])
    {
      fprintf(stderr,"Problème de compatibilité des hauteurs de case.\nTaille attendue : %dpx, taille effective : %dpx\nIndice : %d. (cf code source pour le traitement de l'erreur)\n", hauteur_case, h_case[i], i);
      arret_sdl();
      exit(EXIT_FAILURE);
    }
  }

  largeur_case = l_case[0];
  for(i=1; i<17; i++)
  {
    if (largeur_case != l_case[i])
    {
      fprintf(stderr,"Problème de compatibilité des largeurs de case.\nTaille attendue : %dpx, taille effective : %dpx\nIndice : %d. (cf code source pour le traitement de l'erreur)\n", largeur_case, l_case[i], i);
      arret_sdl();
      exit(EXIT_FAILURE);
    }
  }

  largeur_bouton_valider = bouton_valider_grise->w;
  if (largeur_bouton_valider != bouton_valider_dispo->w)
  {
    fprintf(stderr,"Problème de compatibilité des largeurs du bouton valider.\nTaille du bouton grisé : %dpx, taille du bouton dispo : %dpx\n", largeur_bouton_valider, bouton_valider_dispo->w);
    arret_sdl();
    exit(EXIT_FAILURE);
  }

  hauteur_bouton_valider = bouton_valider_grise->h;
  if (hauteur_bouton_valider != bouton_valider_dispo->h)
  {
    fprintf(stderr,"Problème de compatibilité des hauteurs du bouton valider.\nTaille du bouton grisé : %dpx, taille du bouton dispo : %dpx\n", hauteur_bouton_valider, bouton_valider_dispo->h);
    arret_sdl();
    exit(EXIT_FAILURE);
  }
}



















int init_zones(char** pseudos)
{
  int res;
  int hauteur_map;
  int largeur_map;
  int hauteur_liste_joueurs;
  int largeur_liste_joueurs;
  int hauteur_menu;
  int largeur_menu;
  int hauteur_ecran;
  int largeur_ecran;
  SDL_Rect position;



  if (liste_joueurs) fprintf(stderr, "Tentative de réinitialisation de la liste des joueurs.\n");
  else
  {
    //Création de la zone de l'écran qui contiendra la liste des joueurs
    init_liste_joueurs(pseudos);
    hauteur_liste_joueurs = liste_joueurs->h;
    largeur_liste_joueurs = liste_joueurs->w;
  }


  if (map) fprintf(stderr, "Tentative de réinitialisation de la map.\n");
  else
  {
    //Création de la zone de l'écran qui contiendra la map
    largeur_map = largeur_bordure_g + nb_case_col_map * (largeur_case + largeur_intercase) - largeur_intercase + largeur_bordure_d;
    hauteur_map = hauteur_bordure_s + nb_case_lig_map * (hauteur_case + hauteur_intercase) - hauteur_intercase + hauteur_bordure_i;
    map = SDL_CreateRGBSurface(SDL_HWSURFACE, largeur_map, hauteur_map, 32, 0, 0, 0, 0);
    position.x = 0;
    position.y = 0;
    SDL_BlitSurface(fond_map, NULL, map, &position); //Image de fond
    res = init_map();
  }

  if (menu) fprintf(stderr, "Tentative de réinitialisation du menu.\n");
  else
  {
    //Création de la zone de l'écran qui contiendra le menu
    if (HAUTEUR_AVANT_MAP + hauteur_map > HAUTEUR_AVANT_LISTE + hauteur_liste_joueurs) hauteur_menu = HAUTEUR_AVANT_MAP + hauteur_map;
    else hauteur_menu = HAUTEUR_AVANT_LISTE + hauteur_liste_joueurs;
    hauteur_menu += HAUTEUR_APRES_MAP_LISTE;

    init_menu(hauteur_menu);
    largeur_menu = menu->w;
  }


  if (ecran) fprintf(stderr, "Tentative de réouverture de la fenêtre.\n");
  else
  {
    //Ouverture d'une fenêtre.
    largeur_ecran = LARGEUR_AVANT_LISTE + largeur_liste_joueurs + LARGEUR_ENTRE_LISTE_MAP + largeur_map + LARGEUR_APRES_MAP + largeur_menu;
    if (HAUTEUR_AVANT_MAP + hauteur_map > HAUTEUR_AVANT_LISTE + hauteur_liste_joueurs) hauteur_ecran = HAUTEUR_AVANT_MAP + hauteur_map;
    else hauteur_ecran = HAUTEUR_AVANT_LISTE + hauteur_liste_joueurs;
    hauteur_ecran += HAUTEUR_APRES_MAP_LISTE;

    ecran = SDL_SetVideoMode(largeur_ecran, hauteur_ecran, 32, SDL_HWSURFACE | SDL_DOUBLEBUF); // On tente d'ouvrir une fenêtre en 32 bit sur la mémoire vidéo en double buffering
    if (!ecran) // Si l'ouverture a échoué, on le note et on arrête
    {
        fprintf(stderr, "Impossible de charger le mode vidéo : %s\n", SDL_GetError());
        arret_sdl();
        exit(EXIT_FAILURE);
    }


    //Options de la fenêtre
    SDL_WM_SetCaption(TITRE_DE_LA_FENETRE, TITRE_DE_LA_FENETRE); //Titre de la fenêtre, et de l'icône
    SDL_WM_SetIcon(case_joueur[4], NULL); //Icône de la fenêtre
    SDL_FillRect(ecran, NULL, SDL_MapRGB(ecran->format, 255, 255, 255)); //Couleur de fond, blanc
    position.x = 0;
    position.y = 0;
    SDL_BlitSurface(fond_ecran, NULL, ecran, &position); //Image de fond
  }

  return res;
}





//Initialisation de la liste des joueurs
void init_liste_joueurs(char** pseudos)
{
  int i;

  SDL_Color couleur_joueur[9] = {COULEUR_JOUEUR_1, COULEUR_JOUEUR_2, COULEUR_JOUEUR_3, COULEUR_JOUEUR_4, COULEUR_JOUEUR_5, COULEUR_JOUEUR_6, COULEUR_JOUEUR_7, COULEUR_JOUEUR_8, COULEUR_JOUEUR_9};


  init_scores(pseudos);


  hauteur_max_pseudo = 0;
  largeur_max_pseudo = 0;

  for(i=0; i<nb_joueurs; i++)
  {
    scores_affiches[i] = TTF_RenderText_Blended(police, scores[i], couleur_joueur[i]);
    if ((scores_affiches[i])->w > largeur_max_pseudo) largeur_max_pseudo = (scores_affiches[i])->w;
    if ((scores_affiches[i])->h > hauteur_max_pseudo) hauteur_max_pseudo = (scores_affiches[i])->h;
  }

  if (hauteur_max_pseudo > hauteur_case) hauteur_max = hauteur_max_pseudo;
  else hauteur_max = hauteur_case;

  
  liste_joueurs = SDL_CreateRGBSurface(SDL_HWSURFACE,
    largeur_cadre_g + LARGEUR_AVANT + largeur_case + LARGEUR_MILIEU + largeur_max_pseudo + HAUTEUR_APRES + largeur_cadre_d,
    hauteur_cadre_s + HAUTEUR_AVANT + (hauteur_max + HAUTEUR_MILIEU) * nb_joueurs - HAUTEUR_MILIEU + HAUTEUR_APRES + largeur_cadre_d,
    32, 0, 0, 0, 0);

  draw_scores_liste_joueurs(0);
}






//Fonction qui rajoute un score nul à la fin de chaque pseudo
void init_scores(char** pseudos)
{
  int i,j;
  int taille_score;
  char* pseudo;
  char* score;

  for(i=0; i<nb_joueurs; i++)
  {
    pseudo = *(pseudos + i);
    j=0;
    while(pseudo[j] != '\0') j++;
    taille_score = j + 6;
    score = malloc(taille_score * sizeof(char));

    for(j=0; j<taille_score - 6; j++) score[j] = pseudo[j];

    score[j++] = ' ';
    score[j++] = ':';
    score[j++] = ' ';
    score[j++] = ' ';
    score[j++] = '0';
    score[j] = '\0';

    scores[i] = score;
  }
}





void draw_cadre_liste_joueurs()
{
  SDL_Rect position;
  int i;
  int hauteur = liste_joueurs->h - hauteur_cadre_i - hauteur_cadre_s;
  int largeur = liste_joueurs->w - largeur_cadre_g - largeur_cadre_d;

  //Coin supérieur gauche
  position.x = 0;
  position.y = 0;
  SDL_BlitSurface(cadre_coin_sg, NULL, liste_joueurs, &position);
  position.y += hauteur_cadre_s;

  //Cadre gauche
  for(i=0; i< hauteur; i++)
  {
    SDL_BlitSurface(cadre_g, NULL, liste_joueurs, &position);
    position.y += 1;
  }

  //Coin inférieur gauche
  SDL_BlitSurface(cadre_coin_ig, NULL, liste_joueurs, &position);
  position.x += largeur_cadre_g;

  //Cadre inférieur
  for(i=0; i< largeur; i++)
  {
    SDL_BlitSurface(cadre_i, NULL, liste_joueurs, &position);
    position.x += 1;
  }

  //Coin inférieur droit
  SDL_BlitSurface(cadre_coin_id, NULL, liste_joueurs, &position);

  //Cadre droite
  for(i=0; i< hauteur; i++)
  {
    position.y -= 1;
    SDL_BlitSurface(cadre_d, NULL, liste_joueurs, &position);
  }

  //Coin supérieur droit
  position.y -= hauteur_cadre_s;
  SDL_BlitSurface(cadre_coin_sd, NULL, liste_joueurs, &position);

  //Cadre supérieur
  for(i=0; i< largeur; i++)
  {
    position.x -= 1;
    SDL_BlitSurface(cadre_s, NULL, liste_joueurs, &position);
  }
}










void draw_scores_liste_joueurs(int indice_joueur_dont_cest_le_tour)
{
  int i, y;

  SDL_Rect position;

  SDL_FillRect(liste_joueurs, NULL, SDL_MapRGB(liste_joueurs->format, 255,255,255)); //Couleur de fond, blanc
  position.x = 0;
  position.y = 0;
  SDL_BlitSurface(fond_liste_joueurs, NULL, liste_joueurs, &position); //Image de fond

  draw_cadre_liste_joueurs();

  y = hauteur_cadre_s + HAUTEUR_AVANT;
  for(i=0; i< nb_joueurs; i++)
  {
    position.x = largeur_cadre_g + HAUTEUR_AVANT;
    position.y = y + ((hauteur_max - hauteur_case) / 2);
    if (i == indice_joueur_dont_cest_le_tour) SDL_BlitSurface(case_tontour, NULL, liste_joueurs, &position);
    else SDL_BlitSurface(case_joueur[i], NULL, liste_joueurs, &position);

    position.x += liste_joueurs->w - largeur_bordure_d - LARGEUR_APRES - (scores_affiches[i])->w;
    position.y = y + ((hauteur_max - (scores_affiches[i])->h) / 2);
    SDL_BlitSurface(scores_affiches[i], NULL, liste_joueurs, &position);
    y += hauteur_max + HAUTEUR_MILIEU;
  }
}








//Fonction d'initialisation de la map.
int init_map()
{
  //On remplit un descripteur de map vide
  int i, res;
  int nb_case_tot_map = nb_case_col_map * nb_case_lig_map;
  char* desc_map = malloc ((nb_case_tot_map + 1) * sizeof(char));
  for (i=0; i < nb_case_tot_map; i++) desc_map[i] = 'O';
  desc_map[nb_case_tot_map] = '\0';

  //On dessine les bordures, les intercases, puis les cases vides.
  draw_bordures_map();
  draw_intercases_map();
  res = draw_cases_map(desc_map);

  free(desc_map);
  return res;
}







//Cette fonction dessine les bordures de la map en utilisant les images fournies
void draw_bordures_map()
{
  SDL_Rect position;
  int i;

  //Coin supérieur gauche
  position.x = 0;
  position.y = 0;
  SDL_BlitSurface(bordure_coin_sg, NULL, map, &position);
  position.y += hauteur_bordure_s;

  //Bordure gauche
  for(i=1; i< nb_case_lig_map; i++)
  {
    SDL_BlitSurface(bordure_case_g, NULL, map, &position);
    position.y += hauteur_case;
    SDL_BlitSurface(bordure_intercase_g, NULL, map, &position);
    position.y += hauteur_intercase;
  }
  SDL_BlitSurface(bordure_case_g, NULL, map, &position);
    position.y += hauteur_case;

  //Coin inférieur gauche
  SDL_BlitSurface(bordure_coin_ig, NULL, map, &position);
  position.x += largeur_bordure_g;

  //Bordure inférieure
  for(i=1; i< nb_case_col_map; i++)
  {
    SDL_BlitSurface(bordure_case_i, NULL, map, &position);
    position.x += largeur_case;
    SDL_BlitSurface(bordure_intercase_i, NULL, map, &position);
    position.x += largeur_intercase;
  }
  SDL_BlitSurface(bordure_case_i, NULL, map, &position);
  position.x += largeur_case;

  //Coin inférieur droit
  SDL_BlitSurface(bordure_coin_id, NULL, map, &position);

  //Bordure droite
  for(i=1; i< nb_case_lig_map; i++)
  {
    position.y -= hauteur_case;
    SDL_BlitSurface(bordure_case_d, NULL, map, &position);
    position.y -= hauteur_intercase;
    SDL_BlitSurface(bordure_intercase_d, NULL, map, &position);
  }
  position.y -= hauteur_case;
  SDL_BlitSurface(bordure_case_d, NULL, map, &position);

  //Coin supérieur droit
  position.y -= hauteur_bordure_s;
  SDL_BlitSurface(bordure_coin_sd, NULL, map, &position);

  //Bordure supérieure
  for(i=1; i< nb_case_col_map; i++)
  {
    position.x -= largeur_case;
    SDL_BlitSurface(bordure_case_s, NULL, map, &position);
    position.x -= largeur_intercase;
    SDL_BlitSurface(bordure_intercase_s, NULL, map, &position);
  }
  position.x -= largeur_case;
  SDL_BlitSurface(bordure_case_s, NULL, map, &position);
}










//Cette fonction dessine les intercases sur la map en utilisant les images fournies
void draw_intercases_map()
{
  SDL_Surface* intercase;
  SDL_Rect position;
  int i;

  
  //On crée une colonne de séparation à partir des "morceaux de colonne"
  intercase = SDL_CreateRGBSurface(SDL_HWSURFACE, hauteur_intercase, map->h - largeur_bordure_g - largeur_bordure_d, 32, 0, 0, 0, 0);
  position.x = 0;
  position.y = 0;
  for(i=1; i< nb_case_lig_map; i++)
  {
    SDL_BlitSurface(colonne, NULL, intercase, &position);
    position.y += hauteur_case;
    SDL_BlitSurface(croisement, NULL, intercase, &position);
    position.y += hauteur_intercase;
  }
  SDL_BlitSurface(colonne, NULL, intercase, &position);


  //On crée des colonnes pour séparer les cases verticalement
  position.x = largeur_bordure_g;
  position.y = hauteur_bordure_s;
  for(i=1; i< nb_case_col_map; i++)
  {
    position.x += largeur_case;
    SDL_BlitSurface(intercase, NULL, map, &position);
    position.x += largeur_intercase;
  }
  SDL_FreeSurface(intercase);

  
  //On crée une ligne de séparation à partir des "morceaux de ligne"
  intercase = SDL_CreateRGBSurface(SDL_HWSURFACE, map->w - hauteur_bordure_i - hauteur_bordure_s, hauteur_intercase, 32, 0, 0, 0, 0);
  position.x = 0;
  position.y = 0;
  for(i=1; i< nb_case_col_map; i++)
  {
    SDL_BlitSurface(ligne, NULL, intercase, &position);
    position.x += largeur_case;
    SDL_BlitSurface(croisement, NULL, intercase, &position);
    position.x += largeur_intercase;
  }
  SDL_BlitSurface(ligne, NULL, intercase, &position);


  //On crée des lignes pour séparer les cases horizontalement
  position.x = largeur_bordure_g;
  position.y = hauteur_bordure_s;
  for(i=1; i< nb_case_lig_map; i++)
  {
    position.y += hauteur_case;
    SDL_BlitSurface(intercase, NULL, map, &position);
    position.y += hauteur_intercase;
  }
  SDL_FreeSurface(intercase);
}










//Cette fonction dessine les cases sur la map en utilisant les images fournies
//Elle renvoie -1 s'il y a eu une erreur, 0 sinon.
int draw_cases_map(char* desc_map)
{
  int i = 0, j = 0, k = 0;
  bool continuer = true;
  SDL_Rect position;
  SDL_Surface* case_courante = NULL;


  position.x = largeur_bordure_g;
  position.y = hauteur_bordure_s;
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
        case 'D':
          case_courante = case_derniere_jouee;
          break;
        case '1':
          case_courante = case_joueur[0];
          break;
        case '2':
          case_courante = case_joueur[1];
          break;
        case '3':
          case_courante = case_joueur[2];
          if(nb_joueurs < 3)
          {
            fprintf(stderr, "draw_intercases_map : le descripteur de map passé en paramètre contient le caractère %c, supérieur au nombre de joueurs (%d), à la position numéro %d\nCase problématique : %d x %d\nDescripteur de map : %s\n", desc_map[k], nb_joueurs, k + 1, j, i, desc_map);
            continuer = false;
          }
          break;
        case '4':
          case_courante = case_joueur[3];
          if(nb_joueurs < 4)
          {
            fprintf(stderr, "draw_intercases_map : le descripteur de map passé en paramètre contient le caractère %c, supérieur au nombre de joueurs (%d), à la position numéro %d\nCase problématique : %d x %d\nDescripteur de map : %s\n", desc_map[k], nb_joueurs, k + 1, j, i, desc_map);
            continuer = false;
          }
          break;
        case '5':
          case_courante = case_joueur[4];
          if(nb_joueurs < 5)
          {
            fprintf(stderr, "draw_intercases_map : le descripteur de map passé en paramètre contient le caractère %c, supérieur au nombre de joueurs (%d), à la position numéro %d\nCase problématique : %d x %d\nDescripteur de map : %s\n", desc_map[k], nb_joueurs, k + 1, j, i, desc_map);
            continuer = false;
          }
          break;
        case '6':
          case_courante = case_joueur[5];
          if(nb_joueurs < 6)
          {
            fprintf(stderr, "draw_intercases_map : le descripteur de map passé en paramètre contient le caractère %c, supérieur au nombre de joueurs (%d), à la position numéro %d\nCase problématique : %d x %d\nDescripteur de map : %s\n", desc_map[k], nb_joueurs, k + 1, j, i, desc_map);
            continuer = false;
          }
          break;
        case '7':
          case_courante = case_joueur[6];
          if(nb_joueurs < 7)
          {
            fprintf(stderr, "draw_intercases_map : le descripteur de map passé en paramètre contient le caractère %c, supérieur au nombre de joueurs (%d), à la position numéro %d\nCase problématique : %d x %d\nDescripteur de map : %s\n", desc_map[k], nb_joueurs, k + 1, j, i, desc_map);
            continuer = false;
          }
          break;
        case '8':
          case_courante = case_joueur[7];
          if(nb_joueurs < 8)
          {
            fprintf(stderr, "draw_intercases_map : le descripteur de map passé en paramètre contient le caractère %c, supérieur au nombre de joueurs (%d), à la position numéro %d\nCase problématique : %d x %d\nDescripteur de map : %s\n", desc_map[k], nb_joueurs, k + 1, j, i, desc_map);
            continuer = false;
          }
          break;
        case '9':
          case_courante = case_joueur[8];
          if(nb_joueurs < 9)
          {
            fprintf(stderr, "draw_intercases_map : le descripteur de map passé en paramètre contient le caractère %c, supérieur au nombre de joueurs (%d), à la position numéro %d\nCase problématique : %d x %d\nDescripteur de map : %s\n", desc_map[k], nb_joueurs, k + 1, j, i, desc_map);
            continuer = false;
          }
          break;
        case '\0' :
          case_courante = case_vide;
          continuer = false;
          fprintf(stderr, "draw_intercases_map : le descripteur de map passé en paramètre fait %d cases, alors que le nombre de colonnes est %d et le nombre de lignes %d. Il est donc trop faible.\nDernière case atteinte : %d x %d\nDescripteur de map : %s\n", k, nb_case_col_map, nb_case_lig_map, j, i, desc_map);
          break;
        default :
          case_courante = case_vide;
          continuer = false;
          fprintf(stderr, "draw_intercases_map : le descripteur de map passé en paramètre contient le caractère non valide %c à la position numéro %d\nCase problématique : %d x %d\nDescripteur de map : %s\n", desc_map[k], k + 1, j, i, desc_map);
      }
      SDL_BlitSurface(case_courante, NULL, map, &position);
      position.x += largeur_case + largeur_intercase;
      j++; k++;
    }
    position.x = largeur_bordure_g;
    j = 0;
    position.y += hauteur_case + hauteur_intercase;
    i++;
  }

  if (continuer)
  {
    if (desc_map[k] == '\0') return 0;
    else
    {
      fprintf(stderr, "le descripteur de map passé en paramètre fait %d cases, alors que le nombre de colonnes est %d et le nombre de lignes %d. Il est donc trop élevé.\nDescripteur de map : %s\n", k, nb_case_col_map, nb_case_lig_map, desc_map);
      return (-1);
    }
  }
  else return (-1);
}








void init_menu(int hauteur_menu)
{
  SDL_Rect position;
  SDL_Surface* fond_menu = SDL_LoadBMP(IMAGE(FOND_MENU));

  menu = SDL_CreateRGBSurface(SDL_HWSURFACE, fond_menu->w, hauteur_menu, 32, 0, 0, 0, 0);

  position.x = 0;
  position.y = 0;
  SDL_BlitSurface(fond_menu, NULL, menu, &position); //Image de fond

  position.x = fond_menu->w - LARGEUR_APRES_VALIDER - largeur_bouton_valider;
  position.y = hauteur_menu - HAUTEUR_APRES_VALIDER - hauteur_bouton_valider;
  SDL_BlitSurface(bouton_valider_grise, NULL, menu, &position); //Bouton valider

  SDL_FreeSurface(fond_menu);
}







void affiche()
{
  SDL_Rect position;

  //Collage de la liste des joueurs et de la map sur la fenêtre
  position.x = LARGEUR_AVANT_LISTE;
  position.y = HAUTEUR_AVANT_LISTE;
  SDL_BlitSurface(liste_joueurs, NULL, ecran, &position);
  position.x += liste_joueurs->w + LARGEUR_ENTRE_LISTE_MAP;
  position.y = HAUTEUR_AVANT_MAP;
  SDL_BlitSurface(map, NULL, ecran, &position);

  position.x += map->w + LARGEUR_APRES_MAP;
  position.y = 0;
  SDL_BlitSurface(menu, NULL, ecran, &position);

  //Rafraîchissement de la fenêtre
  SDL_Flip(ecran);
}







int update_sdl(int joueur_dont_cest_le_tour, char* nouveau_score, char* desc_nouvelle_map)
{
  int res = 0;
  int joueur_a_updater = joueur_dont_cest_le_tour - 1;
  if (joueur_a_updater < 0) joueur_a_updater = nb_joueurs - 1;
  printf("Joueur dont c'est le tour (indice) : %d   Joueur a updater (indice) : %d\n", joueur_dont_cest_le_tour, joueur_a_updater);

  if (!initialise)
  {
    fprintf(stderr, "Tentative d'update sans initialisation.\n");
    return (-100);
  }

  res += update_score(joueur_a_updater, joueur_dont_cest_le_tour, nouveau_score);
  res += draw_cases_map(desc_nouvelle_map);
  affiche();

  return res;
}






int update_score(int joueur_a_updater, int joueur_dont_cest_le_tour, char* nouveau_score)
{
  int i, j;
  char* score_a_updater;

  SDL_Color couleur_joueur[9] = {COULEUR_JOUEUR_1, COULEUR_JOUEUR_2, COULEUR_JOUEUR_3, COULEUR_JOUEUR_4, COULEUR_JOUEUR_5, COULEUR_JOUEUR_6, COULEUR_JOUEUR_7, COULEUR_JOUEUR_8, COULEUR_JOUEUR_9};

  score_a_updater = scores[joueur_a_updater];
  i=0;  j=0;
  while(score_a_updater[i] != '\0') i++;
  while(nouveau_score[j] != '\0')
  {
    if ((nouveau_score[j] > '9' || nouveau_score[j] < '0') && nouveau_score[j] != ' ')
    {
      fprintf(stderr, "update_score : le score \"%s\" passé en paramètre contient un caractère non-valide '%c' à l'indice %d\n", nouveau_score, nouveau_score[j], j);
      return (-10);
    }
    j++;
  }


  if (j == 1)
  {
    score_a_updater[i - 2] = ' ';
    score_a_updater[i - 1] = nouveau_score[0];
  }
  else if (j == 2)
  {
    score_a_updater[i - 2] = nouveau_score[0];
    score_a_updater[i - 1] = nouveau_score[1];
  }
  else
  {
    fprintf(stderr, "update_score : le score \"%s\" passé en paramètre ne fait ni 1 ni 2 caractères, et n'est donc pas valide.\n", nouveau_score);
    return (-20);
  }

  SDL_FreeSurface(scores_affiches[joueur_a_updater]);
  scores_affiches[joueur_a_updater] = TTF_RenderText_Blended(police, scores[joueur_a_updater], couleur_joueur[joueur_a_updater]);

  draw_cadre_liste_joueurs();
  draw_scores_liste_joueurs(joueur_dont_cest_le_tour);
  return 0;
}











void a_toi_de_jouer(int* res_x, int* res_y, char* desc_map)
{
  int continuer = 1;
  bool selection = false;

  if (!initialise)
  {
    fprintf(stderr, "Lancement d'a_toi_de_jouer sans initialisation.\n");
    return;
  }

  pthread_t clignote;
  SDL_Rect position;
  SDL_Event event;

  if (pthread_create(&clignote, NULL, switch_message, NULL))
  {
    fprintf(stderr, "Echec de la création du thread clignote.\n");
  };

  while (continuer)
  {
    SDL_WaitEvent(&event);
    switch(event.type)
    {
      case SDL_QUIT:
        arret_sdl();
        exit(EXIT_SUCCESS);
        break;
      case SDL_MOUSEBUTTONUP:
        if (event.button.button == SDL_BUTTON_LEFT)
          {
            switch (check_mouse(selection, res_x, res_y, event.motion.x, event.motion.y, desc_map))
            {
              case 0:
                printf("clic_sur_case\n");
                position.x = menu->w - LARGEUR_APRES_VALIDER - largeur_bouton_valider;
                position.y = menu->h - HAUTEUR_APRES_VALIDER - hauteur_bouton_valider;
                SDL_BlitSurface(bouton_valider_dispo, NULL, menu, &position); //Bouton valider dispo
                affiche();
                selection = true;
                break;
              case 1:
                printf("clic_sur_valider\n");
                if (selection) continuer = false;
                break;
              case -2:
              case -1:
                printf("clic_n'importe_où\n");
                break;
              default:
              fprintf(stderr, "Code retour de check_mouse non valide (attendu : 1, 0, -1 ou -2).\n");
            }
          }
        break;
    }
  }

  SDL_BlitSurface(bouton_valider_grise, NULL, menu, &position); //Bouton valider grisé
  affiche();
  pthread_cancel(clignote);
  SDL_WM_SetCaption(TITRE_DE_LA_FENETRE, TITRE_DE_LA_FENETRE); //Titre de la fenêtre, et de l'icône
}







void* switch_message(void* pas_utilise)
{
  while(1)
  {
    SDL_WM_SetCaption("(!) A toi de jouer !", "(!) A toi de jouer !");
    SDL_Delay(1000);
    SDL_WM_SetCaption(TITRE_DE_LA_FENETRE, TITRE_DE_LA_FENETRE);
    SDL_Delay(1000);
  }
  return pas_utilise;
}








int check_mouse(bool selection, int* res_x, int* res_y, int souris_x, int souris_y, char* desc_map)
{
  int balayage_x = LARGEUR_AVANT_LISTE + liste_joueurs->w + LARGEUR_ENTRE_LISTE_MAP + largeur_bordure_g;
  int balayage_y = HAUTEUR_AVANT_MAP + hauteur_bordure_s;
  int i,j;
  bool trouve;
  SDL_Rect position;


  //On vérifie si le clic est dans la map
  if (balayage_x <= souris_x
   && souris_x <= LARGEUR_AVANT_LISTE + liste_joueurs->w + LARGEUR_ENTRE_LISTE_MAP + map->w - largeur_bordure_d
   && balayage_y <= souris_y
   && souris_y <= HAUTEUR_AVANT_MAP + map->h - hauteur_bordure_i)
  {
    trouve = false;
    i = 0;
    while (!trouve)
    {
      
      if (i >= nb_case_col_map)
      {
        fprintf(stderr, "check_mouse : cas non prévu.");
        arret_sdl();
        exit(EXIT_FAILURE);
      }
      balayage_x += largeur_case;
      if (souris_x <= balayage_x) trouve = true;
      else
      {
        balayage_x += largeur_intercase;
        if (souris_x < balayage_x) return (-1);
      }
      i++;
    }

    trouve = false;
    j = 0;
    while (!trouve)
    {
      
      if (j >= nb_case_lig_map)
      {
        fprintf(stderr, "check_mouse : cas non prévu.");
        arret_sdl();
        exit(EXIT_FAILURE);
      }
      balayage_y += hauteur_case;
      if (souris_y <= balayage_y) trouve = true;
      else
      {
        balayage_y += hauteur_intercase;
        if (souris_y < balayage_y) return (-1);
      }
      j++;
    }

    if(desc_map[(((j - 1) * nb_case_col_map) + i) - 1] != 'O') return (-2);

    if (selection)
    {
      position.x = largeur_bordure_g + (*res_x) * (largeur_case + largeur_intercase);
      position.y = hauteur_bordure_s + (*res_y) * (hauteur_case + hauteur_intercase);
      SDL_BlitSurface(case_vide, NULL, map, &position);
    }

    position.x = largeur_bordure_g + (i - 1) * (largeur_case + largeur_intercase);
    position.y = hauteur_bordure_s + (j - 1) * (hauteur_case + hauteur_intercase);
    SDL_BlitSurface(case_selectionnee, NULL, map, &position);

    affiche();

    *res_x = i - 1;
    *res_y = j - 1;
    return 0;
  }

  //On vérifie si le clic est sur le bouton valider.
  else if (ecran->w - LARGEUR_APRES_VALIDER - largeur_bouton_valider <= souris_x
   && souris_x <= ecran->w - LARGEUR_APRES_VALIDER
   && ecran->h - HAUTEUR_APRES_VALIDER - hauteur_bouton_valider <= souris_y
   && souris_y <= ecran->h - HAUTEUR_APRES_VALIDER) return 1;

  else return (-1);
}









void attendre_fermeture_sdl()
{
  if (!initialise)
  {
    fprintf(stderr, "Lancement de attendre_fermeture_sdl sans initialisation.\n");
    return;
  }

  SDL_Event event;
  
  while (1)
    {
        SDL_WaitEvent(&event);
        switch(event.type)
        {
            case SDL_QUIT:
              arret_sdl();
              exit(EXIT_SUCCESS);
        }
    }
}




void attendre_clic_croix()
{
  if (!initialise)
  {
    fprintf(stderr, "Lancement de attendre_clic_croix sans initialisation.\n");
    return;
  }

  bool continuer = true;
  SDL_Event event;

  while (continuer)
  {
    SDL_WaitEvent(&event);
    switch(event.type)
    {
      case SDL_QUIT:
        continuer = false;
    }
  }
}









void arret_sdl()
{
  if (!initialise) return;

  int i;

  TTF_CloseFont(police);
  SDL_FreeSurface(liste_joueurs);
  SDL_FreeSurface(map);
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
  for(i=0; i< nb_joueurs; i++) free(scores[i]);
  for(i=0; i< nb_joueurs; i++) SDL_FreeSurface(scores_affiches[i]);

  TTF_Quit();
  SDL_Quit();
  initialise = false;
}
