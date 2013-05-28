#include <stdlib.h>
#include <stdio.h>

#include <gtk/gtk.h>
#include "interface_graphique.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>
int XInitThreads();

#define true 1
#define false 0
typedef short bool;

//gcc -Wall `pkg-config --cflags --libs gtk+-2.0` comuser.c -o comuser





struct entry_label
{
  GtkEntry* entry;
  GtkLabel* label;
};

struct fenetre_principale
{
  bool open;
  GtkWidget* adresse;
  GtkLabel* instruction;
  struct entry_label ip;
  struct entry_label port;
  struct entry_label login;
  struct entry_label mdp;
};

struct fenetre_connexion
{
  bool open;
  GtkWidget* adresse;
  GtkLabel* instruction;
};

struct param_signal_create_game //click sur créer partie
{
	GtkWidget* entry_w;
	GtkWidget* entry_h;
	GtkWidget* entry_nbj;
	GtkWidget* label_err;
};

struct param_signal_create_user //click sur créer user
{
	GtkEntry* entry_login;
	GtkEntry* entry_mdp;
  GtkWidget* bouton;
	GtkLabel* label_err;
  bool thread_ouvert;
  GThread* thread;
};

struct fenetre_pre_game
{
  bool open;
  GtkWidget* adresse;
	struct param_signal_create_user param_create_user;
  struct param_signal_create_game param_create_game;
};

struct fenetre_accept_joueur
{
  bool open;
  GtkWidget* adresse;
  GtkWidget* Vbox_layout;
  GtkWidget* label_aff_accepted;
  GtkWidget* bouton_launch;
  int nb_joueurs_acceptes;
};

struct message_recu
{
  char commande;
  char* description_commande;
  char identification;
  int accepte;
  bool id;
  char* pseudo;
  int nb_col_map;
  int nb_lig_map;
  int nb_joueurs;
  char* pseudo_joueur[9];
  int joueur_dont_cest_le_tour;
  char* score;
  char* map;
  int winner;
};

struct toutes_les_fenetres
{
  struct fenetre_principale fenetre_principale;
  struct fenetre_connexion fenetre_connexion;
  struct fenetre_pre_game fenetre_pre_game;
  struct fenetre_accept_joueur fenetre_accept_joueur;
  struct message_recu mess;
  int sock;
  int open_sdl;
  int numero_user;
  char* pseudo_user;
};


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
void connexion(struct toutes_les_fenetres*, char*, char*);
void attendre_et_dechiffrer_message (struct toutes_les_fenetres*, char);
  char* lire_mot (int sock, int* nb_lettres);
  char stoc (struct toutes_les_fenetres*, int, char*, char*);
  int verifier_socket (struct toutes_les_fenetres*, int, char*);

//Fonctions pour la gestion des fenêtres
//  Se trouvent dans fonctions_fenetres.c (fonctions générales et fenêtres de connexion) et admin_windows.c (fenêtres administrateur)
void creer_fenetres (struct toutes_les_fenetres*);
  void creer_fenetre_principale (struct toutes_les_fenetres*);
    struct entry_label add_new_champ_end (char*, GtkWidget*, int);
    void menu_item_new (GtkMenu*, const gchar*, GCallback, gpointer);
  void creer_fenetre_connexion (struct toutes_les_fenetres*);
  void creer_fenetre_pre_game(struct toutes_les_fenetres*);
  void creer_fenetre_accept_joueur(struct toutes_les_fenetres*);
    void add_demande(struct toutes_les_fenetres*, GtkWidget*, GtkWidget*, const char*, int);
void detruire_fenetres (struct toutes_les_fenetres*); //TODO




void traitement_champs (GtkWidget*, gpointer);
  void* authentification (void* arg);

    //Fonctions pour la gestion côté joueur
    //  Se trouvent dans fonctions_joueur.c
    void attente_validation_joueur (struct toutes_les_fenetres*);
      void attente_debut_partie_joueur (struct toutes_les_fenetres*);
        void traitement_jeu (struct toutes_les_fenetres*);
          void trouver_numero_joueur (struct toutes_les_fenetres*);
          void* attente_fermeture (void*);
          void fin_de_partie (struct toutes_les_fenetres*);

    //Fonctions pour la gestion côté admin
    //  Se trouvent dans fonctions_admin.c
    void traitement_admin (struct toutes_les_fenetres*);
      void creer_utilisateur (struct toutes_les_fenetres*, const char*, const char*);
        void* attendre_validation_creation_utilisateur (void*);
      void creer_jeu (struct toutes_les_fenetres*, int, int, int);
        void* attendre_reception_demandes_joueurs (void*);

void retour_fenetre_connexion(struct toutes_les_fenetres*, char*, bool);

void arret (GtkWidget*, gpointer);
