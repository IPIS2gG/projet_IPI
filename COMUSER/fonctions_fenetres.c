/****************************************************/
/*        Projet IPI S2 - 2013 : puissance 5        */
/*                                                  */
/*                    Groupe 4.2                    */
/*                     Groupe G                     */
/*          Nathalie Au,  Lewin Alexandre,          */
/*        Beaume Jérémy et Métraud Alexandre        */
/*                                                  */
/*                      COMUSER                     */
/*               fonctions_fenetres.c               */
/*         Fonctions de gestion des fenêtres        */
/*           (créé par Métraud Alexandre)           */
/****************************************************/


//Inclusion du header global, à regarder en premier
#include "comuser.h"



//Fonction de création des quatre fenêtres
//  (et remplissage de la structure globale)
void creer_fenetres (struct main* m)
{
    //LOCK GTK OK
  printf("Création des fenêtres.\n");
  creer_fenetre_principale(m);
  creer_fenetre_connexion(m);
  //Ces deux dernières fonctions se trouvent dans admin_windows.c
  creer_fenetre_pre_game(m);
  creer_fenetre_accept_joueur(m);
}





struct entry_label add_new_champ_end (char*, GtkWidget*, int);
void menu_item_new (GtkMenu*, const gchar*, GCallback, gpointer);

//Fonction de création de la fenêtre principale
void creer_fenetre_principale (struct main* m)
{
  //LOCK GTK OK

  GtkWidget *p_window = NULL;
    GtkWidget *p_main_box = NULL;
      GtkWidget *p_menu_bar = NULL;
        GtkWidget *p_menu_item = NULL;
        GtkWidget *p_menu = NULL;
      GtkWidget *p_instruction = NULL;
      GtkWidget *p_box_boutons = NULL;
        GtkWidget *p_bouton_valider = NULL;
        GtkWidget *p_bouton_quitter = NULL;

  /* Creation de la fenêtre principale */
  p_window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  m->fenetre_principale.sig_destroy = g_signal_connect (G_OBJECT (p_window), "destroy", G_CALLBACK (arret), m);
  gtk_window_set_title (GTK_WINDOW (p_window), "Puissance 5 en réseau");

  /*Création du conteneur principal*/
  p_main_box = gtk_vbox_new (FALSE, 10);
  gtk_container_add (GTK_CONTAINER (p_window), p_main_box);

  /*Création de la barre menu*/
  p_menu_bar = gtk_menu_bar_new ();
  gtk_box_pack_start (GTK_BOX (p_main_box), p_menu_bar, FALSE, FALSE, 0);

  /*Création du menu Quitter*/
  p_menu = gtk_menu_new ();
  p_menu_item = gtk_menu_item_new_with_mnemonic ("_Fichier");
  menu_item_new (GTK_MENU(p_menu), "Quitter", G_CALLBACK(arret), m);
  gtk_menu_item_set_submenu (GTK_MENU_ITEM (p_menu_item), p_menu);
  gtk_menu_shell_append (GTK_MENU_SHELL (p_menu_bar), p_menu_item);

  /*Création de la ligne de boutons*/
  p_box_boutons = gtk_hbox_new (FALSE, 20); //gtk_hbutton_box_new ();
  gtk_box_pack_end (GTK_BOX (p_main_box), p_box_boutons, FALSE, FALSE, 0);

  /*Création du bouton quitter*/
  p_bouton_quitter = gtk_button_new_from_stock (GTK_STOCK_QUIT);
  g_signal_connect (G_OBJECT (p_bouton_quitter), "clicked", G_CALLBACK (arret), m);
  gtk_box_pack_end (GTK_BOX (p_box_boutons), p_bouton_quitter, FALSE, FALSE, 0);

  /*Création du bouton Valider*/
  p_bouton_valider = gtk_button_new_from_stock (GTK_STOCK_OK); //gtk_button_new_with_label("Valider");
  g_signal_connect (G_OBJECT (p_bouton_valider), "clicked", G_CALLBACK (signal_traitement_champs), (gpointer) m);
  gtk_box_pack_end (GTK_BOX (p_box_boutons), p_bouton_valider, FALSE, FALSE, 0);

  //Création du champ de mot de passe
  m->fenetre_principale.mdp = add_new_champ_end("Mot de passe :", p_main_box, 0);
  gtk_entry_set_visibility(m->fenetre_principale.mdp.entry, FALSE);
  g_signal_connect(GTK_OBJECT(m->fenetre_principale.mdp.entry), "activate", G_CALLBACK(signal_traitement_champs), (gpointer) m);

  //Création du champ de login
  m->fenetre_principale.login = add_new_champ_end("Login :", p_main_box, 0);
  g_signal_connect(GTK_OBJECT(m->fenetre_principale.login.entry), "activate", G_CALLBACK(signal_traitement_champs), (gpointer) m);

  //Création du champ de port
  m->fenetre_principale.port = add_new_champ_end("Port :", p_main_box, 0);
  g_signal_connect(GTK_OBJECT(m->fenetre_principale.port.entry), "activate", G_CALLBACK(signal_traitement_champs), (gpointer) m);

  //Création du champ de IP
  m->fenetre_principale.ip = add_new_champ_end("IP :", p_main_box, 16);
  g_signal_connect(GTK_OBJECT(m->fenetre_principale.ip.entry), "activate", G_CALLBACK(signal_traitement_champs), (gpointer) m);

  //Création de l'instruction
  p_instruction = gtk_label_new("Veuillez rentrer les informations pour vous connecter :");
  gtk_box_pack_end (GTK_BOX (p_main_box), p_instruction, FALSE, FALSE, 0);
  m->fenetre_principale.instruction = GTK_LABEL(p_instruction);

  m->fenetre_principale.open = 1;
  m->fenetre_principale.adresse = p_window;
}

//Fonction de création d'un élément Label : Champ Label_derreur.
struct entry_label add_new_champ_end (char* nom_du_champ, GtkWidget* conteneur, int nb_car_max)
{
  //LOCK GTK OK

  GtkWidget* p_cont;
  GtkWidget* p_label;
  GtkWidget* p_entry;
  GtkWidget* p_erreur;
  struct entry_label p_champ;

  p_cont = gtk_hbox_new (FALSE, 10);
  gtk_box_pack_end (GTK_BOX (conteneur), p_cont, FALSE, FALSE, 0);

  p_label = gtk_label_new(nom_du_champ);
  gtk_box_pack_start (GTK_BOX (p_cont), p_label, FALSE, FALSE, 0);

  p_erreur = gtk_label_new("");
  gtk_box_pack_end (GTK_BOX (p_cont), p_erreur, TRUE, TRUE, 0);
  p_champ.label = GTK_LABEL(p_erreur);

  if (nb_car_max < 1) p_entry = gtk_entry_new();
  else p_entry = gtk_entry_new_with_max_length(nb_car_max);
  gtk_box_pack_end (GTK_BOX (p_cont), p_entry, TRUE, TRUE, 0);
  p_champ.entry = GTK_ENTRY(p_entry);

  return p_champ;
}

//Fonction de création d'un élément de menu
void menu_item_new (GtkMenu *p_menu, const gchar *title, GCallback callback, gpointer user_data)
{
  GtkWidget *p_menu_item = NULL;
  p_menu_item = gtk_menu_item_new_with_mnemonic (title);
  gtk_menu_shell_append (GTK_MENU_SHELL (p_menu), p_menu_item);
  g_signal_connect (G_OBJECT (p_menu_item), "activate", callback, user_data);
}


//Fonction de création de la fenêtre de connexion
void creer_fenetre_connexion (struct main* m)
{
  GtkWidget* p_window;
    GtkWidget* p_main_box;
      GtkWidget* p_instruction;
      //GtkWidget* p_bouton_annuler;

  //Création de la fenêtre
  p_window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  m->fenetre_connexion.sig_destroy = g_signal_connect (G_OBJECT (p_window), "destroy", G_CALLBACK (arret), (gpointer) m);
  gtk_window_set_title (GTK_WINDOW (p_window), "Tentative de connexion");

  //Création du conteneur principal
  p_main_box = gtk_vbox_new (FALSE, 10);
  gtk_container_add (GTK_CONTAINER (p_window), p_main_box);

  //Création du bouton Annuler
  //  Bouton annulé *Badum tsss*
  /*p_bouton_annuler = gtk_button_new_from_stock (GTK_STOCK_OK); //gtk_button_new_with_label("Valider");
  g_signal_connect (G_OBJECT (p_bouton_annuler), "clicked", G_CALLBACK (arret_fenetre_connexion), (gpointer) p_window);
  gtk_box_pack_end (GTK_BOX (p_main_box), p_bouton_annuler, FALSE, FALSE, 0);*/

  //Création de l'instruction
  p_instruction = gtk_label_new("Tentative de connexion, veuillez patienter...");
  gtk_box_pack_end (GTK_BOX (p_main_box), p_instruction, FALSE, FALSE, 0);
  m->fenetre_connexion.instruction = GTK_LABEL(p_instruction);

  m->fenetre_connexion.open = 0;
  m->fenetre_connexion.adresse = p_window;
}



//Fonction de retour à la fenêtre principale
//  Si la SDL n'est pas ouverte, on se contente de cacher toutes les fenêtres et d'afficher la fenêtre principale
//  Si la SDL est ouverte, on la ferme, on recrée les fenêtres, et on affiche la fenêtre principale
//  On affiche message_erreur dans la fenêtre principale
//  On libère cette chaîne de charactères si a_liberer = true
void retour_fenetre_connexion(struct main* m, char* message_erreur, bool a_liberer)
{
  //PAS DE LOCK GTK
  printf("Fermeture de la connexion.\n");
  close(m->sock);

  printf("Retour à la fenêtre de connexion, avec message affiché :\n Cause :\n%s\n", message_erreur);

  if(!m->open_sdl)
  { //Si on est encore dans la phase pre-game :
    gdk_threads_enter();

    //LOCK GTK OK
    
    //On cache toutes les fenêtres
    gtk_widget_hide (m->fenetre_connexion.adresse);
    m->fenetre_connexion.open = 0;
    gtk_widget_hide (m->fenetre_pre_game.adresse);
    m->fenetre_pre_game.open = 0;
    gtk_widget_hide (m->fenetre_accept_joueur.adresse);
    m->fenetre_accept_joueur.open = 0;
    //On affiche la fenetre principale
    gtk_widget_show_all (m->fenetre_principale.adresse);
    m->fenetre_principale.open = 1;
    gdk_threads_leave();
    m->test_retour_fenetre_connexion = true;
  }

  else
  { //Si on est en jeu
    //On arrête la SDL
    arret_sdl();
    //On recrée les fenêtres GTK qui avaient été supprimées.
    gdk_threads_enter();
    creer_fenetres(m);
    gdk_threads_leave();
  }

  //On affiche le message d'erreur
  gdk_threads_enter();
  gtk_label_set_text(m->fenetre_principale.instruction, message_erreur);
  gdk_threads_leave();
  //On le libère éventuellement
  if (a_liberer) free(message_erreur);

  //On ferme le thread si on arrive à partir d'une fenêtre GTK+
  if(!m->open_sdl) g_thread_exit(NULL);
}



//Même fonction que ci-dessus, lancée dans un thread, puis à partir du clic croix d'une fenêtre.

void* retour_fenetre_connexion_thread (void* arg)
{
  struct main* m = (struct main*) arg;
  retour_fenetre_connexion(m, "Vous avez redémarré le programme en cliquant sur une croix.", false);
  return NULL;
}

void retour_fenetre_connexion_clic_croix(GtkWidget* widget, gpointer data)
{
  struct main* m = (struct main*) data;

  if (widget == m->fenetre_pre_game.adresse)
  {
    printf("  Reconstruction de la fenêtre pre_game...\n");
    creer_fenetre_pre_game(m);
  }
  if (widget == m->fenetre_accept_joueur.adresse)
  {
    printf("  Reconstruction de la fenêtre accept_joueur...\n");
    creer_fenetre_accept_joueur(m);
  }
  g_thread_unref(g_thread_new("Retour à la fenêtre principale", retour_fenetre_connexion_thread, data));
}



//Fonction de destruction des quatre fenêtres.
//  La variable p sera mise à NULL dans la plupart des cas et les quatre fenêtres seront détruites.
//  Si cette fonction a été appelée par la fonction arret, la variable p contient l'adresse de l'élément qui a appelé arret.
//    Si cette adresse est la même qu'une des quatre fenêtre, cela veut dire que cette arret a été appelée lors d'un clic sur une croix d'une fenêtre, et cette fenêtre a donc déjà été détruite. On fait donc le test afin de ne pas la détruire à nouveau.
//  Autre remarque : avant de détruire une fenêtre, on prend soin de déconnecter le signal associé à la croix, qui s'éxécutera sinon.
void detruire_fenetres (struct main* m, GtkWidget* p)
{
  printf("Destruction des fenêtres GTK...\n");
  if (p != m->fenetre_accept_joueur.adresse)
  {
    printf("  Destruction de la fenêtre d'acceptation de joueurs...\n");
    g_signal_handler_disconnect(m->fenetre_accept_joueur.adresse, m->fenetre_accept_joueur.sig_destroy); //Déconnexion du signal
    gtk_widget_destroy(m->fenetre_accept_joueur.adresse); //on peut détruire la fenêtre en toute sérinité
  }

  if (p != m->fenetre_pre_game.adresse)
  {
    printf("  Destruction de la fenêtre administrateur...\n");
    g_signal_handler_disconnect(m->fenetre_pre_game.adresse, m->fenetre_pre_game.sig_destroy); //Idem
    gtk_widget_destroy(m->fenetre_pre_game.adresse);
  }

  if (p != m->fenetre_connexion.adresse)
  {
    printf("  Destruction de la fenêtre de connexion...\n");
    g_signal_handler_disconnect(m->fenetre_connexion.adresse, m->fenetre_connexion.sig_destroy);
    gtk_widget_destroy(m->fenetre_connexion.adresse);
  }

  if (p != m->fenetre_principale.adresse)
  {
    printf("  Destruction de la fenêtre principale...\n");
    g_signal_handler_disconnect(m->fenetre_principale.adresse, m->fenetre_principale.sig_destroy);
    gtk_widget_destroy(m->fenetre_principale.adresse);
  }
}



//Fonction d'arret appelée par signal à partir d'une fenêtre GTK+.
//  Elle n'arrête pas le programme elle-même, mais détruit les fenêtres et arrête la boucle GTK+.
//  Le déroulement de la fonction main après cette boucle fera ce qu'il faut pour arrêter proprement le programme.
void arret (GtkWidget *p_widget, gpointer user_data)
{
  struct main* m = (struct main*) user_data;
  detruire_fenetres (m, p_widget);
  gtk_main_quit();

  /* Parametres inutilises */
  (void)p_widget;
  (void)user_data;
}
