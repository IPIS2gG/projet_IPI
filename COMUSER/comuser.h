/****************************************************/
/*        Projet IPI S2 - 2013 : puissance 5        */
/*                                                  */
/*                    Groupe 4.2                    */
/*                     Groupe G                     */
/*          Nathalie Au,  Lewin Alexandre,          */
/*        Beaume Jérémy et Métraud Alexandre        */
/*                                                  */
/*                      COMUSER                     */
/*                     comuser.h                    */
/*                  Header  global                  */
/*           (créé par Métraud Alexandre)           */
/****************************************************/



//Ce fichier est le point d'entrée du programme.
//Après sa lecture, vous pouvez lire comuser.c, qui contient la fonction main.



//___INCLUSION DES BIBLIOTHÈQUES___//
//Bibliothèques standard
#include <stdlib.h>
#include <stdio.h>

//GTK+ pour la gestion des fenêtres
#include <gtk/gtk.h>
//L'interface graphique pour la gestion de la partie
#include "interface_graphique.h"

//Pour la gestion du réseau
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>
int XInitThreads();

#define true 1
#define false 0
typedef short bool;



//___STRUCTURE GLOBALE ET SOUS-STRUCTURES___//

//Plutôt que d'utiliser des variables globales, on utilise une structure globale instanciée dans la fonction main qu'on passe en paramètre de toutes les fonctions qui en ont besoin et qui contient, entre autre, tout ce qui est nécessaire pour faire fonctionner les fenêtres

struct entry_label
{ //Association d'un champ de texte et d'une zone d'affichage d'erreur.
  GtkEntry* entry;
  GtkLabel* label;
};

struct fenetre_principale
{ //Fenêtre pour demaner à l'utilisateur les infos de connexion
  bool open; //Booléen qui détermine si la fenêtre est cachée ou non
  GtkWidget* adresse; //Pointeur vers la fenêtre
  gulong sig_destroy; //Contient le signal "destroy" pour pouvoir le désactiver avant de détruire la fenêtre
  GtkLabel* instruction; //Zone d'affichage d'instructions
  struct entry_label ip; //Champ pour l'ip
  struct entry_label port; //Champ pour le port
  struct entry_label login; //Champ pour le login
  struct entry_label mdp; //Champ pour le mot de passe
};

struct fenetre_connexion
{ //Fenêtre d'attente de connexion, validation, et début de partie
  bool open; //Booléen qui détermine si la fenêtre est cachée ou non
  GtkWidget* adresse; //Pointeur vers la fenêtre
  gulong sig_destroy; //Contient le signal "destroy" pour pouvoir le désactiver avant de détruire la fenêtre
  GtkLabel* instruction; //Zone d'affichage d'instructions
};

struct param_signal_create_user //click sur créer user
{ //Première partie de la fenêtre pre_game : création d'utilisateur
	GtkEntry* entry_login; //Champ pour le login
	GtkEntry* entry_mdp; //Champ pour le mot de passe
	GtkLabel* label_err; //Zone d'affichage d'erreurs
  GtkWidget* bouton; //Bouton de validation de création de joueur (pour pouvoir le sensibiliser/désensibiliser)
  bool thread_ouvert; //Booléen qui indique si on est en train d'attendre la confimation qu'un joueur a été créé (auquel cas le bouton ci-dessus est désensibilisé)
  GThread* thread; //Thread d'attente de confirmation
};

struct param_signal_create_game //click sur créer partie
{ //Deuxième partie de la fenêtre pre_game: création de partie
	GtkWidget* entry_w; //Champ pour la largeur de la map
	GtkWidget* entry_h; //Champ pour la hauteur de la map
	GtkWidget* entry_nbj; //Champ pour le nombre de joueurs max
	GtkWidget* entry_nbc; //Champ pour le nombre de coups max
	GtkWidget* label_err; //Zone d'affichage d'erreurs
};

struct fenetre_pre_game
{ //Fenêtre administrateur de création d'utilisateurs et de partie
  bool open; //Booléen qui détermine si la fenêtre est cachée ou non
  GtkWidget* adresse; //Pointeur vers la fenêtre
  gulong sig_destroy; //Contient le signal "destroy" pour pouvoir le désactiver avant de détruire la fenêtre
	struct param_signal_create_user param_create_user; //Première partie de la fenêtre : création d'utilisateur
  struct param_signal_create_game param_create_game; //Deuxième partie de la fenêtre : création de partie
};

struct fenetre_accept_joueur
{ //Fenêtre administrateur d'acceptation de joueurs et de lancement de partie
  bool open; //Booléen qui détermine si la fenêtre est cachée ou non
  GtkWidget* adresse; //Pointeur vers la fenêtre
  gulong sig_destroy; //Contient le signal "destroy" pour pouvoir le désactiver avant de détruire la fenêtre
  GtkWidget* Vbox_layout;
  GtkWidget* label_aff_accepted;
  GtkWidget* bouton_launch; //Bouton de début de partie (pour pouvoir le sensibiliser/désensibiliser)
  int nb_joueurs_acceptes; //Pour retenir le nombre de joueurs qui ont été acceptés
};

struct message_recu
{ //Structure contenant les informations du dernier message reçu.
  char commande; //Commande reçue ('c' pour information de connexion par exemple, voir protocole). '\0' si erreur.
  char* description_commande; //Allouée (cf ci-après). Un message contenant soit une description de la commande, soit un message d'erreur.
  //Les paramètres ci-dessous sont remplis en fonction de la commande reçue
  //  (cf fonction attendre_et_dechiffrer_message dans fonctions_connexion.c)
  //  Les char* avec la mention "allouée" sont allouées au début de la fonction main, et libérées à la fin de la fonction main.
  //  On garde comme invariant qu'elles restent allouées tout au long du programme.
  char identification;
  int accepte;
  bool id;
  char* pseudo; //allouée
  int nb_col_map;
  int nb_lig_map;
  int nb_joueurs;
  char* pseudo_joueur[9];
  int joueur_dont_cest_le_tour;
  int joueur_score_update;
  char* score; //allouée
  char* map; //allouée
  int winner;
};

struct main
{ //Structure globale
  //Les quatres fenêtres GTK+ et la structure de message reçu précédentes
  struct fenetre_principale fenetre_principale;
  struct fenetre_connexion fenetre_connexion;
  struct fenetre_pre_game fenetre_pre_game;
  struct fenetre_accept_joueur fenetre_accept_joueur;
  struct message_recu mess;
  int sock; //Socket
  bool open_sdl; //est mis à true quand la sdl est ouverte, ou qu'on veut l'ouvrir., à false sinon.
  bool test_retour_fenetre_connexion; //est mis à true quand on veut revenir à la fenêtre de connexion après avoir fermé la SDL
  int numero_user; //numéro de l'utilisateur en cours (mis à 0 si admin)
  char* pseudo_user; //pseudo de l'utilisateur en cours
};



//___PROTOTYPES ET HIERARCHIE DES FONCTIONS___//
//Se reporter aux fichiers indiqués pour une description des fonctions

//Fonctions utiles.
//  Se trouvent dans fonctions_utiles.c
  char* concat_string (const char*, const char*);
  char* concat_string_gfree (char*, const char*);
  char* concat_string_dfree (const char*, char*);
  char* concat_string_bfree (char*, char*);
  char* itoa (int);

//Fonctions pour la réception et le décriptage de messages.
//  Se trouvent dans fonctions_connexion.c
//  Après appel de attendre_et_dechiffrer message, le message se trouve dans le champ mess de la structure principale
void connexion(struct main*, char*, char*);
void attendre_et_dechiffrer_message (struct main*, char);

//Fonctions pour la gestion des fenêtres
//  Se trouvent dans fonctions_fenetres.c (fonctions générales et fenêtres de connexion) et admin_windows.c (fenêtres administrateur)
void creer_fenetres (struct main*);
  void creer_fenetre_principale (struct main*);
  void creer_fenetre_connexion (struct main*);
  void creer_fenetre_pre_game(struct main*);
  void creer_fenetre_accept_joueur(struct main*);
    void add_demande(struct main*, GtkWidget*, GtkWidget*, const char*, int);
void retour_fenetre_connexion(struct main*, char*, bool);
void retour_fenetre_connexion_clic_croix(GtkWidget*, gpointer);
void detruire_fenetres (struct main*, GtkWidget*);
void arret (GtkWidget*, gpointer);


//Fonctions principales
//  Se trouvent dans comuser.c, avec la fonction main.
void traitement_champs (GtkWidget*, gpointer);
  void* authentification (void* arg);

    //Fonctions pour la gestion côté joueur
    //  Se trouvent dans fonctions_joueur.c
    void attente_validation_joueur (struct main*);
      void attente_debut_partie_joueur (struct main*);
        void traitement_jeu (struct main*);
          void trouver_numero_joueur (struct main*);
          void* attente_fermeture (void*);
          void fin_de_partie (struct main*);

    //Fonctions pour la gestion côté admin
    //  Se trouvent dans fonctions_admin.c
    void traitement_admin (struct main*);
      void creer_utilisateur (struct main*, const char*, const char*);
        void* attendre_validation_creation_utilisateur (void*);
      void creer_jeu (struct main*, int, int, int, int);
        void* attendre_reception_demandes_joueurs (void*);
