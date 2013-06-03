/****************************************************/
/*        Projet IPI S2 - 2013 : puissance 5        */
/*                                                  */
/*                    Groupe 4.2                    */
/*                     Groupe G                     */
/*          Nathalie Au,  Lewin Alexandre,          */
/*        Beaume Jérémy et Métraud Alexandre        */
/*                                                  */
/*                      COMUSER                     */
/*                 fonctions_admin.c                */
/*        Fonctions de gestion administrateur       */
/*           (créé par Métraud Alexandre)           */
/****************************************************/


//Inclusion du header global, à regarder en premier
#include "comuser.h"


//Point d'entrée du traitement administrateur
//  Cache la fenêtre connexion et affiche la fenêtre pre_game
void traitement_admin (struct main* m)
{
  printf("Début du traitement admin.\n");

  gdk_threads_enter();
  gtk_widget_hide (m->fenetre_connexion.adresse);
  m->fenetre_connexion.open = false;

  gtk_widget_show_all(m->fenetre_pre_game.adresse);
  m->fenetre_pre_game.open = true;
  gdk_threads_leave();
}



//Fonction de création d'utilisateur, appelée lors d'un clic sur le bouton valider de la partie "créer utilisateur" de la fenêtre pre_game
//  Envoie un message de demande de création d'utilisateur avec le login et mot de passe fournis (les vérifications d'intégrité ont déjà été faites).
//  Désensibilise le bouton d'acceptation, et crée un thread d'attente de réponse.
void creer_utilisateur (struct main* m, const char* login, const char* mdp)
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



//Fonction d'attente de réponse suite à une demande de création d'un utilisateur.
//  Dans les deux cas, sensibilise le bouton à nouveau.
void* attendre_validation_creation_utilisateur (void* arg)
{
  struct main* m = (struct main*) arg;
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



//Fonction de création de jeu, appelée lors d'un clic sur le bouton valider de la partie "créer jeu" de la fenêtre pre_game
//  Envoie un message de demande de création de jeu avec les paramètres fournis (les vérifications d'intégrité ont déjà été faites).
//  Cache la fenêtre pre_game et ouvre la fenêtre accept_joueur.
//  Crée un thread d'attente de demande d'acceptation de la part des joueurs.
void creer_jeu (struct main* m, int hauteur, int largeur, int nombre_de_joueurs, int nombre_de_coups)
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



//Thread d'attente de demande d'acceptation de la part des joueurs.
//  Réceptionne les demandes de joueurs et les envoie à la fonction add_demande du fichier admin_windows.c qui les traite.
//  Si le signal s est reçu, la partie commence : on demande l'ouverture de la SDL, on ferme la boucle GTK+, et ce thread.
void* attendre_reception_demandes_joueurs (void* arg)
{
  struct main* m = (struct main*) arg;

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
