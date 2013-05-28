#include "comuser.h"










int main(int argc, char** argv)
{
  //Initialisation de la structure globale
  struct toutes_les_fenetres m;

  struct message_recu mess;
  char* description_commande = malloc(sizeof(char));
  char* score = malloc(sizeof(char));
  char* map = malloc(sizeof(char));
  char* pseudo = malloc(sizeof(char));

  description_commande[0] = '\0';
  score[0] = '\0';
  map[0] = '\0';
  pseudo[0] = '\0';

  mess.description_commande = description_commande;
  mess.score = score;
  mess.map = map;
  mess.pseudo = pseudo;
  m.mess = mess;

  // Initialisation des threads
  if(!XInitThreads())
  {
    fprintf(stderr, "Erreur d'exécution de XInitThreads\n");
    exit(EXIT_FAILURE);
  }

  gdk_threads_enter();
  //Initialisation de GTK+
  //g_thread_init(NULL); //deprecated
  gdk_threads_init();
  gtk_init (&argc, &argv);

  // Création des fenêtres
  creer_fenetres(&m);

  // Affichage de la fenetre principale
  gtk_widget_show_all (m.fenetre_principale.adresse);

  // Lancement de la boucle de gtk pour la procédure de connexion.
  gtk_main ();
  gdk_threads_leave();

  //Si on sort de la boucle, cela veut dire qu'on est un joueur et qu'on veut lancer le jeu
  printf("Lancement du jeu.\n");
  traitement_jeu(&m);

  return EXIT_SUCCESS;
}










//FIXME
void retour_fenetre_connexion(struct toutes_les_fenetres* m, char* message_erreur, bool a_liberer)
{
  printf("Fermeture de la connexion.");
  close(m->sock);

  printf("Retour à la fenêtre de connexion, avec message affiché :\n Cause :\n%s\n", message_erreur);
  
  arret_sdl();
  gdk_threads_enter();
  gtk_widget_hide (m->fenetre_connexion.adresse);
  m->fenetre_connexion.open = 0;
  gtk_widget_hide (m->fenetre_pre_game.adresse);
  m->fenetre_pre_game.open = 0;
  gtk_widget_show_all (m->fenetre_principale.adresse);
  m->fenetre_principale.open = 1;
  gtk_label_set_text(m->fenetre_principale.instruction, message_erreur);
  gdk_threads_leave();
  if (a_liberer) free(message_erreur);
  g_thread_exit(NULL);
}













void traitement_champs (GtkWidget *p_widget, gpointer user_data)
{
  g_thread_unref(g_thread_new("Authentification", authentification, user_data));

  /* Paramêtre inutilisé */
  (void)p_widget;
}




void* authentification (void* arg)
{
  struct toutes_les_fenetres* m = (struct toutes_les_fenetres*) arg;

  if(!m->fenetre_principale.open)
  {
    fprintf(stderr, "Fonction de callback traitement_champs lancée avec fenêtre cachée.\n");
    g_thread_exit(NULL);
  }

  char* message_erreur = concat_string("Erreur :\n","");

  char* message_envoye;
  int i;

  G_CONST_RETURN gchar* login = gtk_entry_get_text(m->fenetre_principale.login.entry);
  G_CONST_RETURN gchar* mdp = gtk_entry_get_text(m->fenetre_principale.mdp.entry);

  connexion(m, (char*) login, (char*) mdp);

  //Envoi de la demande d'authentification
  message_envoye = concat_string_gfree(concat_string_gfree(concat_string("C ", (char*) login), " "), (char*) mdp);
  i = 0;
  while (*(message_envoye + i)) i++;
  write(m->sock, message_envoye, i+1);
  printf("Commande 'demande de connexion' envoyée : %s", message_envoye); //FIXME Le mdp apparait en clair dans les logs (et dans le protocole)
  free(message_envoye);
  m->pseudo_user = (char*) login;


  //Réception du message de connexion :
  attendre_et_dechiffrer_message (m, 'c');

  switch (m->mess.commande)
  {
    case 'c' :
      printf("\nCommande de connexion ");
      switch (m->mess.identification)
      {
        case '0' :
        printf("échouée.\n");
        message_erreur = concat_string_gfree(message_erreur, "   Echec de la connexion.\nAvez-vous correctement rentré votre login et votre mot de passe ?\nVous pouvez néanmoins retenter :");
        retour_fenetre_connexion(m, message_erreur, true);
        break;

        case 'J' :
        printf("réussie pour un joueur.\n");
        free(message_erreur);
        attente_validation_joueur(m);
        break;

        case 'A' :
        printf("réussie pour un administrateur.\n");
        free(message_erreur);
        traitement_admin(m);
        break;

        case 'W' :
        printf("réussie pour un joueur, mais en attente d'admin.\n");
        message_erreur = concat_string_gfree(message_erreur, "   Connexion impossible car aucun admin n'est connecté.\nVous pouvez néanmoins retenter :");
        retour_fenetre_connexion(m, message_erreur, true);
        break;

        default :
        printf("non comprise.\n");
        message_erreur = concat_string_gfree(message_erreur, m->mess.description_commande);
        retour_fenetre_connexion(m, message_erreur, true);
        break;
      }
      break;

    default :
      printf("Commande reçue non comprise ou reçue au mauvais moment : %s (attendu : commande c)\n", m->mess.description_commande);
      message_erreur = concat_string_gfree(message_erreur, m->mess.description_commande);
      retour_fenetre_connexion(m, message_erreur, true);
      break;
  }

  g_thread_exit(NULL);
  return NULL;
}




void arret (GtkWidget *p_widget, gpointer user_data)
{
  gdk_threads_enter();
  gtk_main_quit();
  gdk_threads_leave();

  /* Parametres inutilises */
  (void)p_widget;
  (void)user_data;
}
