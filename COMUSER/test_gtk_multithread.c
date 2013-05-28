#include <stdlib.h>
#include <stdio.h>
#include "interface_graphique.h"

#include <gtk/gtk.h>
#include <unistd.h>
int XInitThreads();

struct fenetre_principale
{
  GtkWidget* adresse;
};

struct toutes_les_fenetres
{
  struct fenetre_principale fenetre_principale;
};


void boucle_connexion(struct toutes_les_fenetres*, int, char**);
  GtkWidget* creer_fenetre_principale (struct toutes_les_fenetres*, const char*);
    void traitement_champs (GtkWidget*, gpointer);
      void* connexion (void* arg);



int main(int argc, char** argv)
{
  int i;
  char* pseudos[3] = {"tapata","tapatuche","tapatouche"};

  //Initialisation de la structure globale
  struct toutes_les_fenetres m;

  // Initialisation des threads
  if(!XInitThreads())
  {
    fprintf(stderr, "Erreur d'exécution de XInitThreads\n");
    exit(EXIT_FAILURE);
  }

  //Demande de connexion, gestion administrateur, etc... dans la boucle GTK+
  boucle_connexion(&m, argc, argv);

  //Si on sort de la boucle, cela veut dire qu'on est un joueur et qu'on veut lancer le jeu
  printf("Lancement du jeu.\n");
  for(i=0; i<3; i++)
  {
    printf("Boucle %d\n", i);
    sleep(1);
  }
  if (initialisation_sdl(3, pseudos, 5, 6))
  {
    fprintf(stderr, "Problème lors de l'initialisation.\n");
  }
  for(i=3; i<6; i++)
  {
    printf("Boucle %d\n", i);
    sleep(1);
  }

  return EXIT_SUCCESS;
}


void boucle_connexion(struct toutes_les_fenetres* m, int argc, char** argv)
{
  GtkWidget* p_window;

  //Initialisation de GTK+
  //g_thread_init(NULL); //deprecated
  gdk_threads_init();
  gtk_init (&argc, &argv);

  // Création des fenêtres
  p_window = creer_fenetre_principale(m, "Veuillez rentrer les informations pour vous connecter :");

  // Affichage de la fenetre principale
  gtk_widget_show_all (p_window);

  // Lancement de la boucle de gtk pour la procédure de connexion.
  gtk_main ();
}



GtkWidget* creer_fenetre_principale(struct toutes_les_fenetres* m, const char* instruction)
{
  GtkWidget *p_window = NULL;
    GtkWidget *p_main_box = NULL;
      GtkWidget *p_box_boutons = NULL;
        GtkWidget *p_bouton_valider = NULL;

  /* Creation de la fenêtre principale */
  p_window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW (p_window), "Puissance 5 en réseau");

  /*Création du conteneur principal*/
  p_main_box = gtk_vbox_new (FALSE, 10);
  gtk_container_add (GTK_CONTAINER (p_window), p_main_box);

  /*Création de la ligne de boutons*/
  p_box_boutons = gtk_hbox_new (FALSE, 20); //gtk_hbutton_box_new ();
  gtk_box_pack_end (GTK_BOX (p_main_box), p_box_boutons, FALSE, FALSE, 0);

  /*Création du bouton Valider*/
  p_bouton_valider = gtk_button_new_from_stock (GTK_STOCK_OK); //gtk_button_new_with_label("Valider");
  g_signal_connect (G_OBJECT (p_bouton_valider), "clicked", G_CALLBACK (traitement_champs), (gpointer) m);
  gtk_box_pack_end (GTK_BOX (p_box_boutons), p_bouton_valider, FALSE, FALSE, 0);

  m->fenetre_principale.adresse = p_window;
  return p_window;
}




void traitement_champs (GtkWidget *p_widget, gpointer user_data)
{
  g_thread_unref(g_thread_new("Connexion", connexion, user_data));

  /* Paramêtre inutilisé */
  (void)p_widget;
}



void* connexion (void* arg)
{
  printf("Début de la fonction connexion, attendre 2 secondes avant fermeture\n");
  sleep(2);
  gdk_threads_enter();
  gtk_main_quit();
  gdk_threads_leave();
  g_thread_exit(NULL);
  return NULL;
}
