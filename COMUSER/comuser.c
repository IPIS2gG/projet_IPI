/****************************************************/
/*        Projet IPI S2 - 2013 : puissance 5        */
/*                                                  */
/*                    Groupe 4.G                    */
/*                    Groupe 4.2                    */
/*                     Groupe G                     */
/*          Nathalie Au,  Lewin Alexandre,          */
/*        Beaume Jérémy et Métraud Alexandre        */
/*                                                  */
/*                      COMUSER                     */
/*                     comuser.c                    */
/*       Fonction main et fonctions générales       */
/*           (créé par Métraud Alexandre)           */
/****************************************************/


//Inclusion du header global, à regarder en premier
#include "comuser.h"


//___FONCTION MAIN___

int main(int argc, char** argv)
{
  //Initialisation de la structure globale
  struct main m;

  struct message_recu mess;
  char* description_commande = malloc(sizeof(char));
  char* pseudo = malloc(sizeof(char));
  char* score = malloc(sizeof(char));
  char* map = malloc(sizeof(char));

  description_commande[0] = '\0';
  pseudo[0] = '\0';
  score[0] = '\0';
  map[0] = '\0';

  mess.description_commande = description_commande;
  mess.score = score;
  mess.map = map;
  mess.pseudo = pseudo;
  m.mess = mess;
  m.open_sdl = false;
  m.test_retour_fenetre_connexion = true;


  // Initialisation des threads et de GTK+
  if(!XInitThreads())
  {
    fprintf(stderr, "Erreur d'exécution de XInitThreads\n");
    exit(EXIT_FAILURE);
  }
  gdk_threads_enter();
  gdk_threads_init();
  gtk_init (&argc, &argv);

  // Création des fenêtres
  creer_fenetres(&m);

  // Affichage de la fenetre principale
  //  (appel à la fonction traitement_champs de ce même fichier en cas de clic sur le bouton valider)
  gtk_widget_show_all (m.fenetre_principale.adresse);
  gdk_threads_leave();

  //Boucle de retour à la fenêtre de conexion
  while (m.test_retour_fenetre_connexion)
  {
    m.test_retour_fenetre_connexion = false;

    // Lancement de la boucle de GTK+ pour la procédure de connexion.
    gdk_threads_enter();
    gtk_main ();
    gdk_threads_leave();

    //Si on sort de la boucle GTK+ et que open_sdl = true, cela veut dire qu'on est un joueur et qu'on veut lancer le jeu
    if (m.open_sdl)
    {
      detruire_fenetres(&m, NULL);
      printf("Lancement du jeu.\n");
      traitement_jeu(&m);
    }
  }

    printf("Fin de la fonction main. Fermeture...\n");
    //On libère les chaînes de caractères allouées.
    free(mess.description_commande);
    free(mess.score);
    free(mess.map);
    free(mess.pseudo);

    return EXIT_SUCCESS;
}





//Fonction déclenchée lors de l'appui sur le bouton valider de la fenêtre principale
//  Elle crée un nouveau thread qui éxécute la fonction authentification ci-après.
//  Cette deuxième fonction va récupérer ce que l'utilisateur a rentré dans les champs de la fenêtre.
//  Elle va ensuite appeler la fonction connexion de fonctions_connexion.c qui va vérifier que tout est valide, et va tenter de créer une connexion.
//  Elle va ensuite envoyer au serveur central une demande d'authentification
//  On cache ensuite la fenêtre principale et on ouvre la fenêtre de connexion qui va afficher ce qu'il se passe.
//  Elle va enfin attendre une réponse pour savoir si l'authentification s'est bien passée.
//    Si l'utilisateur est correctement authentifié en tant que joueur, la fonction attente_validation_joueur, du fichier fonctions_joueur.c, est exécutée.
//    Si l'utilisateur est correctement authentifié en tant qu'administrateur, la fonction traitement_admin, du fichier fonctions_admin.c, est exécutée.
//  En cas de problème quelque part, on revient à la fenêtre principale en affichant ce qu'il s'est passé, grâce à la foncion retour_fenetre_connexion plus loin dans ce fichier.
//    Remarque : même si ce ne sera pas toujours indiqué par la suite, la plupart des cas d'erreur sont traité ainsi, en revenant à la fenêtre principale.

void traitement_champs (GtkWidget *p_widget, gpointer user_data)
{
  //Création d'un thread
  g_thread_unref(g_thread_new("Authentification", authentification, user_data));

  /* Paramêtre inutilisé */
  (void)p_widget;
}


void* authentification (void* arg)
{
  struct main* m = (struct main*) arg;

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


  //Vérification de l'intégrité des champs et tentative de connexion
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
