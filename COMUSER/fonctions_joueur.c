#include "comuser.h"

void attente_validation_joueur (struct toutes_les_fenetres* m)
{
  char* message_erreur = concat_string("Erreur :","\n   ");

  gtk_label_set_markup(m->fenetre_connexion.instruction, "Authentification réussie !\nAttente de validation par l'Administrateur.\nVeuillez patienter s'il vous plaît...");

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



void attente_debut_partie_joueur (struct toutes_les_fenetres* m)
{
  gtk_label_set_markup(m->fenetre_connexion.instruction, "Validation par l'administrateur réussie !\nAttente de la création de la partie.\nVeuillez patienter s'il vous plaît...");


  //Réception du message de création de la partie :
  attendre_et_dechiffrer_message (m, 's');

  switch (m->mess.commande)
  {
    case 's' :
      printf("\nCommande de création de partie.\n");
      trouver_numero_joueur(m);
      gdk_threads_enter();
      gtk_main_quit();
      gdk_threads_leave();
      g_thread_exit(NULL);
      break;

    default :
      printf("Commande reçue non comprise ou reçue au mauvais moment : %s (attendu : commande s)", m->mess.description_commande);
      retour_fenetre_connexion(m, concat_string("Erreur :\n   ", m->mess.description_commande), true);
      return;
  }
}





void trouver_numero_joueur(struct toutes_les_fenetres* m)
{
  int i;
  char* p;
  char* c;

  printf("Recherche du numéro de joueur :");

  for (i=0; i<m->mess.nb_joueurs; i++)
  {
    p = m->pseudo_user;
    printf("     Pseudo user : %s == ", p);
    c = m->mess.pseudo_joueur[i];
    printf("%s : Pseudo joueur ?", c);
    while (*p == *c)
    {
      if (*p == '\0')
      {
        printf ("Utilisateur courant (numéro) : %d", i);
        m->numero_user = i + 1;
        return;
      }
      p++; c++;
    }
  }

  fprintf(stderr, "Pseudo de l'utilisateur non-trouvé. Numéro 1 par défaut.\n");
  m->numero_user = 1;
}






void traitement_jeu (struct toutes_les_fenetres* m)
{
  int x, y, i;
  char* message_envoye;
  int numero_joueur = m->numero_user;
  int sock = m->sock;

  printf("Lancement du jeu avec les paramètre suivant :\n");
  printf("nb joueurs : %d, nb_col_map : %d, nb_lig_map : %d\n", m->mess.nb_joueurs, m->mess.nb_col_map, m->mess.nb_lig_map);
  for (i=0; i < m->mess.nb_joueurs; i++) printf("Pseudo joueur %d : %s\n", i, m->mess.pseudo_joueur[i]);
  if (initialisation_sdl(m->mess.nb_joueurs, m->mess.pseudo_joueur, m->mess.nb_col_map, m->mess.nb_lig_map))
  {
    fprintf(stderr, "Problème lors de l'initialisation.\n");
    arret_sdl();
    exit (EXIT_FAILURE);
  }
  trouver_numero_joueur(m);

  g_thread_unref(g_thread_new("Attente_fermeture", attente_fermeture, NULL));

  while(1)
  {
    attendre_et_dechiffrer_message (m, 'T');
    switch (m->mess.commande)
    {
      case 'T' :

        printf("\nCommande de tour suivant.\n   Joueur qui joue (numéro) : %d\n   Score : %s\n   Map : %s\n", m->mess.joueur_dont_cest_le_tour, m->mess.score, m->mess.map);
        update_sdl(m->mess.joueur_dont_cest_le_tour - 1, m->mess.score, m->mess.map);

        if (m->mess.joueur_dont_cest_le_tour == numero_joueur)
        {
          a_toi_de_jouer(&x, &y, m->mess.map);

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
        fin_de_partie(m);
        break;
      default:
        break;
    }
  }
}


void fin_de_partie (struct toutes_les_fenetres* m)
{
  printf("Non traitée : fermeture du programme.");
  exit(EXIT_SUCCESS);
}


void* attente_fermeture (void* arg)
{
  attendre_clic_croix();
  arret_sdl();

  return arg;
}
