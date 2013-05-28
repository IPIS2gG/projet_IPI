#include "comuser.h"


void creer_fenetres (struct toutes_les_fenetres* m)
{
  creer_fenetre_principale(m);
  creer_fenetre_connexion(m);
  creer_fenetre_pre_game(m);
  creer_fenetre_accept_joueur(m);
}

void detruire_fenetres (struct toutes_les_fenetres* m)
{
  //TODO
}

void creer_fenetre_principale (struct toutes_les_fenetres* m)
{
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
  g_signal_connect (G_OBJECT (p_window), "destroy", G_CALLBACK (arret), p_window);
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
  menu_item_new (GTK_MENU(p_menu), "Quitter", G_CALLBACK(arret), NULL);
  gtk_menu_item_set_submenu (GTK_MENU_ITEM (p_menu_item), p_menu);
  gtk_menu_shell_append (GTK_MENU_SHELL (p_menu_bar), p_menu_item);

  /*Création de la ligne de boutons*/
  p_box_boutons = gtk_hbox_new (FALSE, 20); //gtk_hbutton_box_new ();
  gtk_box_pack_end (GTK_BOX (p_main_box), p_box_boutons, FALSE, FALSE, 0);

  /*Création du bouton quitter*/
  p_bouton_quitter = gtk_button_new_from_stock (GTK_STOCK_QUIT);
  g_signal_connect (G_OBJECT (p_bouton_quitter), "clicked", G_CALLBACK (arret), NULL);
  gtk_box_pack_end (GTK_BOX (p_box_boutons), p_bouton_quitter, FALSE, FALSE, 0);

  /*Création du bouton Valider*/
  p_bouton_valider = gtk_button_new_from_stock (GTK_STOCK_OK); //gtk_button_new_with_label("Valider");
  g_signal_connect (G_OBJECT (p_bouton_valider), "clicked", G_CALLBACK (traitement_champs), (gpointer) m);
  gtk_box_pack_end (GTK_BOX (p_box_boutons), p_bouton_valider, FALSE, FALSE, 0);

  //Création du champ de mot de passe
  m->fenetre_principale.mdp = add_new_champ_end("Mot de passe :", p_main_box, 0);
  gtk_entry_set_visibility(m->fenetre_principale.mdp.entry, FALSE);

  //Création du champ de login
  m->fenetre_principale.login = add_new_champ_end("Login :", p_main_box, 0);

  //Création du champ de port
  m->fenetre_principale.port = add_new_champ_end("Port :", p_main_box, 0);

  //Création du champ de IP
  m->fenetre_principale.ip = add_new_champ_end("IP :", p_main_box, 16);

  //Création de l'instruction
  p_instruction = gtk_label_new("Veuillez rentrer les informations pour vous connecter :");
  gtk_box_pack_end (GTK_BOX (p_main_box), p_instruction, FALSE, FALSE, 0);
  m->fenetre_principale.instruction = GTK_LABEL(p_instruction);

  m->fenetre_principale.open = 1;
  m->fenetre_principale.adresse = p_window;
}


struct entry_label add_new_champ_end (char* nom_du_champ, GtkWidget* conteneur, int nb_car_max)
{
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



void menu_item_new (GtkMenu *p_menu, const gchar *title, GCallback callback, gpointer user_data)
{
  GtkWidget *p_menu_item = NULL;
  p_menu_item = gtk_menu_item_new_with_mnemonic (title);
  gtk_menu_shell_append (GTK_MENU_SHELL (p_menu), p_menu_item);
  g_signal_connect (G_OBJECT (p_menu_item), "activate", callback, user_data);
}








void creer_fenetre_connexion (struct toutes_les_fenetres* m)
{
  GtkWidget* p_window;
    GtkWidget* p_main_box;
      GtkWidget* p_instruction;
      //GtkWidget* p_bouton_annuler;

  p_window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  g_signal_connect (G_OBJECT (p_window), "destroy", G_CALLBACK (arret), (gpointer) NULL);
  gtk_window_set_title (GTK_WINDOW (p_window), "Tentative de connexion");

  //Création du conteneur principal
  p_main_box = gtk_vbox_new (FALSE, 10);
  gtk_container_add (GTK_CONTAINER (p_window), p_main_box);

  //Création du bouton Annuler
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
