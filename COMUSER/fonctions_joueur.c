/****************************************************/
/*        Projet IPI S2 - 2013 : puissance 5        */
/*                                                  */
/*                    Groupe 4.2                    */
/*                     Groupe G                     */
/*          Nathalie Au,  Lewin Alexandre,          */
/*        Beaume Jérémy et Métraud Alexandre        */
/*                                                  */
/*                      COMUSER                     */
/*                fonctions_joueurs.c               */
/*           Fonctions de gestion joueurs           */
/*           (créé par Métraud Alexandre)           */
/****************************************************/


#include "comuser.h"

//Point d'entrée du traitement administrateur.
//Fonction d'attente de validation par l'administrateur.
//  Si le joueur est validé, appelle attente_debut_partie_joueur ci-après.
void attente_validation_joueur (struct main* m)
{
  char* message_erreur = concat_string("Erreur :","\n   ");
  
  gdk_threads_enter();
  gtk_label_set_text(m->fenetre_connexion.instruction, "Authentification réussie !\nAttente de validation par l'Administrateur.\nVeuillez patienter s'il vous plaît...");
  gdk_threads_leave();

  //Réception du message de validation :
  attendre_et_dechiffrer_message (m, 'J');

  switch (m->mess.commande)
  {
    case 'J' :
      printf("\nCommande d'acceptation ");

      switch (m->mess.accepte)
      {
        case 0 :
        printf("échouée.\n");
        message_erreur = concat_string_gfree(message_erreur, "Echec de la connexion : l'administrateur vous a refusé.\nVous pouvez néanmoins retenter :");
        retour_fenetre_connexion(m, message_erreur, true);
        break;

        case 1 :
        printf("réussie.\n");
        free(message_erreur);
        attente_debut_partie_joueur(m);
        break;

        default :
        printf("non comprise. ->%d<-\n", m->mess.accepte - 1);
        message_erreur = concat_string_bfree(message_erreur, m->mess.description_commande);
        retour_fenetre_connexion(m, message_erreur, true);
        return;
        break;
      }
      break;

    default :
      printf("Commande reçue non comprise ou reçue au mauvais moment : %s (attendu : commande J)\n", m->mess.description_commande);
      message_erreur = concat_string_gfree(message_erreur, m->mess.description_commande);
      retour_fenetre_connexion(m, message_erreur, true);
      return;
  }
}



//Fonction d'attente de début de partie.
//  Lorsque la partie commence : on demande l'ouverture de la SDL, on recherche son numéro de joueur grâce à trouver_numero_joueur ci-après, on ferme la boucle GTK+, et ce thread.
void attente_debut_partie_joueur (struct main* m)
{
  gdk_threads_enter();
  gtk_label_set_markup(m->fenetre_connexion.instruction, "Validation par l'administrateur réussie !\nAttente de la création de la partie.\nVeuillez patienter s'il vous plaît...");
  gdk_threads_leave();


  //Réception du message de création de la partie :
  attendre_et_dechiffrer_message (m, 's');

  switch (m->mess.commande)
  {
    case 's' :
      printf("\nCommande de création de partie.\n");
      m->open_sdl = true;
      trouver_numero_joueur(m);
      gdk_threads_enter();
      gtk_main_quit();
      gtk_widget_hide (m->fenetre_connexion.adresse);
      m->fenetre_connexion.open = 0;
      gdk_threads_leave();
      g_thread_exit(NULL);
      break;

    default :
      printf("Commande reçue non comprise ou reçue au mauvais moment : %s (attendu : commande s)", m->mess.description_commande);
      retour_fenetre_connexion(m, concat_string("Erreur :\n   ", m->mess.description_commande), true);
      return;
  }
}



//Fonction qui, à partir de la liste des joueurs et du pseudo de l'utilisateur courant enregistrés dans la structure globale, va enregistrer dans la structure globale le numéro du joueur courant.
//  Si echec de la recherche : le numéro 0 est donné, et le joueur courant sera spectateur uniquement.
void trouver_numero_joueur(struct main* m)
{
  int i;
  char* p;
  char* c;

  printf("Recherche du numéro de joueur :\n");

  for (i=0; i<m->mess.nb_joueurs; i++)
  {
    p = m->pseudo_user;
    printf("     Pseudo user : %s == ", p);
    c = m->mess.pseudo_joueur[i];
    printf("%s : Pseudo joueur ?\n", c);
    while (*p == *c)
    {
      if (*p == '\0')
      {
        printf ("Utilisateur courant (numéro) : %d\n", i + 1);
        m->numero_user = i + 1;
        return;
      }
      p++; c++;
    }
  }

  fprintf(stderr, "Pseudo de l'utilisateur non-trouvé. Numéro 0 par défaut (ne jouera pas).\n");
  m->numero_user = 0;
}



//Fonction appelée lorsqu'on a fermé la boucle GTK+ et lancé la SDL, qui gère le déroulement du jeu.
//  Attend les signaux T et W.
//    En cas de signal T, rafraîchit l'affichage avec les nouvelles informations et vérifie que ce n'est pas au tout du joueur courant de joueur, auquel cas on demande de jouer.
//    En cas de signal W, fin de partie.
//  Remarque : on laisse tourner un thread qui vérifiera si l'utilisateur appuie sur la croi de la fenêtre. On ferme ce thread quand l'utilisateur courant joue, car la fonction a_toi_de_jouer vérifie déjà cela, et pour faire tourner le programme plus vite (la différence de performance est très visible)
void traitement_jeu (struct main* m)
{
  int x, y, i;
  char* message_envoye;
  int numero_joueur = m->numero_user;
  int sock = m->sock;
  bool fermer_thread = false;

  printf("Lancement du jeu avec les paramètre suivant :\n");
  printf("nb joueurs : %d, nb_col_map : %d, nb_lig_map : %d\n", m->mess.nb_joueurs, m->mess.nb_col_map, m->mess.nb_lig_map);
  for (i=0; i < m->mess.nb_joueurs; i++) printf("Pseudo joueur %d : %s\n", i, m->mess.pseudo_joueur[i]);

  //Initialisation de la SDL.
  if (initialisation_sdl(m->mess.nb_joueurs, m->mess.pseudo_joueur, m->mess.nb_col_map, m->mess.nb_lig_map))
  {
    fprintf(stderr, "Problème lors de l'initialisation.\n");
    arret_sdl();
    exit (EXIT_FAILURE);
  }

  //Création du thread de vérification de clic sur la croix.
  g_thread_unref(g_thread_new("Attente_fermeture", attente_fermeture, &fermer_thread));

  while(1)
  {
    attendre_et_dechiffrer_message (m, 'T');
    switch (m->mess.commande)
    {
      case 'T' :
        printf("\nCommande de tour suivant.\n   Joueur qui joue (numéro) : %d\n   Score : %s\n   Map : %s\n", m->mess.joueur_dont_cest_le_tour, m->mess.score, m->mess.map);
        //Mise à jour de l'affichage
        update_sdl(m->mess.joueur_dont_cest_le_tour - 1, m->mess.joueur_score_update - 1, m->mess.score, m->mess.map);

        if (m->mess.joueur_dont_cest_le_tour == numero_joueur)
        {
          //Demande de fermeture du thread de vérification du clic sur la croix.
          fermer_thread = true;
          //Au joueur courant de jouer.
          a_toi_de_jouer(&x, &y, m->mess.map);
          //On réouvre le thread.
          fermer_thread = false;
          g_thread_unref(g_thread_new("Attente_fermeture", attente_fermeture, &fermer_thread));

          message_envoye = concat_string_bfree(concat_string_gfree(concat_string_dfree("P ", itoa(x)), " "), itoa(y));
          i = 0;
          while (*(message_envoye + i)) i++;
          write(sock, message_envoye, i+1);
          printf("Commande envoyée : %s\n", message_envoye);
          free(message_envoye);
        }
        break;

      case 'M' :
        //TODO
        printf("Commande de mauvais coup (non traitée : fermeture du programme).\n");
        exit(EXIT_FAILURE);
        break;

      case 'W' :
        printf("Commande de fin de partie\n");
        printf("Gagnant : %d\n", m->mess.winner);
        //Fin de partie
        victoire(m->mess.winner - 1);
        fermer_thread = true;
        attendre_clic_croix_ou_bouton_valider();
        retour_fenetre_connexion(m, "Voulez-vous rejouer ?", false);
        return;

      default:
        retour_fenetre_connexion(m, concat_string("Erreur :\n   ", m->mess.description_commande), true);
        return;
    }
  }
}



//Thread de vérification de clic sur la croix.
void* attente_fermeture (void* arg)
{
  attendre_clic_croix_ou_changement_etat((bool*) arg, g_thread_exit);
  arret_sdl();

  return arg;
}
