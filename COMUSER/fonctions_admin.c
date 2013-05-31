#include "comuser.h"

void traitement_admin (struct toutes_les_fenetres* m)
{
  printf("Début du traitement admin.\n");

  gtk_widget_hide (m->fenetre_connexion.adresse);
  m->fenetre_connexion.open = false;

  gdk_threads_enter();
  gtk_widget_show_all(m->fenetre_pre_game.adresse);
  gdk_threads_leave();
  m->fenetre_pre_game.open = true;
}



void creer_utilisateur (struct toutes_les_fenetres* m, const char* login, const char* mdp)
{
  char* message_envoye;
  int i;

  gtk_widget_set_sensitive (m->fenetre_pre_game.param_create_user.bouton, FALSE);

  printf("Envoi de demande de création de l'utilisateur %s avec le mot de passe %s\n", login, mdp);
  message_envoye = concat_string(concat_string(concat_string("A ", login), " "), mdp);
  i = 0;
  while (*(message_envoye + i)) i++;
  write(m->sock, message_envoye, i+1);
  printf("Commande 'demande d'ajout d'utilisateur' envoyée : %s\n", message_envoye); //FIXME Le mdp apparait en clair dans les logs (et dans le protocole)
  free(message_envoye);

  m->fenetre_pre_game.param_create_user.thread = g_thread_new("Attente validation création utilisateur", attendre_validation_creation_utilisateur, m);
}



void* attendre_validation_creation_utilisateur (void* arg)
{
  struct toutes_les_fenetres* m = (struct toutes_les_fenetres*) arg;
  char* message;

  m->fenetre_pre_game.param_create_user.thread_ouvert = true;

  //Réception du message de validation :
  attendre_et_dechiffrer_message (m, 'a');

  switch (m->mess.commande)
  {
    case 'a' :
      printf("\nCommande de création ");

      switch (m->mess.accepte)
      {
        case 0 :
        printf("échouée.\n");
        message = concat_string("Echec de la création de l'utilisateur (pseudo déjà pris ?).\n", "Vous pouvez néanmoins retenter !");
        break;

        case 1 :
        printf("réussie.\n");
        message = concat_string("","Réussite de la création de l'utilisateur.\n");
        break;

        default :
        printf("non comprise. ->%d<-\n", m->mess.accepte - 1);
        retour_fenetre_connexion(m, m->mess.description_commande, false);
      }
      break;

    default :
      printf("Commande reçue non comprise ou reçue au mauvais moment : %s (attendu : commande a)", m->mess.description_commande);
      retour_fenetre_connexion(m, m->mess.description_commande, false);
  }

  gdk_threads_enter();
  gtk_widget_set_sensitive (m->fenetre_pre_game.param_create_user.bouton, TRUE);

  gtk_label_set_text(m->fenetre_pre_game.param_create_user.label_err, message);
  gdk_threads_leave();
  free(message);

  m->fenetre_pre_game.param_create_user.thread_ouvert = false;
  g_thread_exit(NULL);
  return NULL;
}




void creer_jeu (struct toutes_les_fenetres* m, int hauteur, int largeur, int nombre_de_joueurs, int nombre_de_coups)
{
  char* message_envoye;
  int i;

  if(m->fenetre_pre_game.param_create_user.thread_ouvert) g_thread_join(m->fenetre_pre_game.param_create_user.thread);

  message_envoye = concat_string_gfree(concat_string_gfree(concat_string_gfree(concat_string_gfree(concat_string_gfree(concat_string_gfree(concat_string("N ", itoa(largeur)), " "), itoa(hauteur)), " "), itoa(nombre_de_coups)), " "), itoa(nombre_de_joueurs));
  i = 0;
  while (*(message_envoye + i)) i++;
  write(m->sock, message_envoye, i+1);
  printf("Commande 'création de partie' envoyée : %s\n", message_envoye); //FIXME Le mdp apparait en clair dans les logs (et dans le protocole)
  free(message_envoye);

  gtk_widget_hide (m->fenetre_pre_game.adresse);
  m->fenetre_pre_game.open = false;

  gtk_widget_show_all(m->fenetre_accept_joueur.adresse);
  m->fenetre_accept_joueur.open = true;

  g_thread_new("Attente de la réception de demandes de joueurs", attendre_reception_demandes_joueurs, m);
}



void* attendre_reception_demandes_joueurs (void* arg)
{
  struct toutes_les_fenetres* m = (struct toutes_les_fenetres*) arg;

  while (1)
  {
    //Réception du message de validation :
    attendre_et_dechiffrer_message (m, 'P');

    switch (m->mess.commande)
    {
      case 'P' :
        printf("\nCommande de demande d'acceptation du joueur %s (id : %d)\n", m->mess.pseudo, m->mess.id);

        add_demande(m, m->fenetre_accept_joueur.Vbox_layout, m->fenetre_accept_joueur.label_aff_accepted, m->mess.pseudo, m->mess.id);
        break;

      case 's' :
        printf("\nCommande de création de partie.\n");
        m->numero_user = 0;
        m->open_sdl = true;
        gdk_threads_enter();
        gtk_main_quit();
        gdk_threads_leave();
        g_thread_exit(NULL);
        break;

      default :
        printf("Commande reçue non comprise ou reçue au mauvais moment : %s (attendu : commande P)", m->mess.description_commande);
        retour_fenetre_connexion(m, m->mess.description_commande, false);
    }
  }
  g_thread_exit(NULL);
  return NULL;
}
