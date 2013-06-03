/****************************************************/
/*        Projet IPI S2 - 2013 : puissance 5        */
/*                                                  */
/*                    Groupe 4.2                    */
/*                     Groupe G                     */
/*          Nathalie Au,  Lewin Alexandre,          */
/*        Beaume Jérémy et Métraud Alexandre        */
/*                                                  */
/*                      COMUSER                     */
/*                fonctions_connexion.c             */
/*  Fonctions de gestion des communications réseau  */
/*           (créé par Métraud Alexandre)           */
/****************************************************/


#include "comuser.h"



//Fonction de connexion au réseau
//  Vérifie que l'intégrité paramètres rentrés dans les champs, auquel cas tente de se connecter.
void connexion (struct main* m, char* login, char* mdp)
{
  G_CONST_RETURN gchar* ip_cible = gtk_entry_get_text(m->fenetre_principale.ip.entry);
	short cont;
  G_CONST_RETURN gchar* port_cible = gtk_entry_get_text(m->fenetre_principale.port.entry);

	struct sockaddr_in cible;
	struct sockaddr_in mes_infos;
	unsigned int sizeof_mes_infos = sizeof(mes_infos);

  char* message_erreur = concat_string("Erreur :\n","");
  int erreur = 0;


  gdk_threads_enter();
  gtk_label_set_text(m->fenetre_principale.instruction, "Tentative de connexion. Veuillez patienter s'il vous plaît...");

  cible.sin_family=AF_INET;


  printf("Clic sur le bouton \"Valider\".\nInformations rentrées :\n");


  //Vérifications d'intégrité

  printf("   IP : %s\n", ip_cible);
  cont=inet_aton(ip_cible, (struct in_addr*) &cible.sin_addr);
  if (cont)
  {
    printf("      IP correcte.\n");
    gtk_label_set_text(m->fenetre_principale.ip.label, "");
  }
  else
  {
    printf("      IP incorrecte.\n");
    gtk_label_set_text(m->fenetre_principale.ip.label, "L'adresse IP est incorrecte.");
    erreur = 1;
    message_erreur = concat_string_gfree(message_erreur, "   - L'adresse IP est incorrecte\n");
  }

  printf("   Port : %s\n", port_cible);
  if (*port_cible)
  {
    printf("      Port correct.\n");
    gtk_label_set_text(m->fenetre_principale.port.label, "");
	  cible.sin_port=htons(atoi(port_cible));
  }
  else
  {
    printf("      port incorrect (vide).\n");
    gtk_label_set_text(m->fenetre_principale.port.label, "Le port est incorrect.");
    erreur = 1;
    message_erreur = concat_string_gfree(message_erreur, "   - Le port est incorrect\n");
  }

  printf("   Login : %s\n", login);
  if (*login)
  {
    printf("      login correct.\n");
    gtk_label_set_text(m->fenetre_principale.login.label, "");
  }
  else
  {
    printf("      login incorrect (vide).\n");
    gtk_label_set_text(m->fenetre_principale.login.label, "Le login est incorrect.");
    erreur = 1;
    message_erreur = concat_string_gfree(message_erreur, "   - Le login est incorrect\n");
  }

  printf("   MDP : %s\n", mdp);
  if (*mdp)
  {
    printf("      mdp correct.\n");
    gtk_label_set_text(m->fenetre_principale.mdp.label, "");
  }
  else
  {
    printf("      mdp incorrect (vide).\n");
    gtk_label_set_text(m->fenetre_principale.mdp.label, "Le mot de passe est incorrect.");
    erreur = 1;
    message_erreur = concat_string_gfree(message_erreur, "   - Le mot de passe est incorrect\n");
  }

  if (erreur)
  {
    message_erreur = concat_string_gfree(message_erreur, "Veuillez retenter s'il vous plaît.");
    gtk_label_set_text(m->fenetre_principale.instruction, message_erreur);
    free(message_erreur);
    gdk_threads_leave();
    g_thread_exit(NULL);
  }

  gtk_widget_hide (m->fenetre_principale.adresse);
  m->fenetre_principale.open = 0;
  gtk_widget_show_all (m->fenetre_connexion.adresse);
  m->fenetre_connexion.open = 1;

  gtk_label_set_text(m->fenetre_connexion.instruction, "Tentative de connexion.\nVeuillez patienter s'il vous plaît...");
  printf("\nTentative de connexion...\n");

	//Création du socket
  m->sock=socket(AF_INET, SOCK_STREAM, 0);
  if(m->sock==-1)
  {
    fprintf(stderr, "Erreur lors de la création du socket : %s\n", strerror(errno));
    message_erreur = concat_string_gfree(message_erreur, "Lors de la création du socket : ");
    message_erreur = concat_string_gfree(message_erreur, strerror(errno));
    message_erreur = concat_string_gfree(message_erreur, "\nVous pouvez néanmoins retenter :");
    gtk_label_set_text(m->fenetre_principale.instruction, message_erreur);
    free(message_erreur);
    gtk_widget_hide (m->fenetre_connexion.adresse);
    m->fenetre_connexion.open = 0;
    gtk_widget_show_all (m->fenetre_principale.adresse);
    m->fenetre_principale.open = 1;
    gdk_threads_leave();
    g_thread_exit(NULL);
  }
  else printf("Socket créé.\n");

	//connexion du socket
	if(connect(m->sock, (struct sockaddr*) &cible, sizeof(cible)) < 0)
	{
		fprintf(stderr, "Erreur lors de la connexion du socket : %s\n", strerror(errno));
    message_erreur = concat_string_gfree(message_erreur, "Lors de la connexion du socket : ");
    message_erreur = concat_string_gfree(message_erreur, strerror(errno));
    message_erreur = concat_string_gfree(message_erreur, "\nVous pouvez néanmoins retenter :");
    gtk_label_set_text(m->fenetre_principale.instruction, message_erreur);
    free(message_erreur);
    gtk_widget_hide (m->fenetre_connexion.adresse);
    m->fenetre_connexion.open = 0;
    gtk_widget_show_all (m->fenetre_principale.adresse);
    m->fenetre_principale.open = 1;
    gdk_threads_leave();
    g_thread_exit(NULL);
	}
  else printf("Socket connecté.\n");

  //récupération et affichage de mes infos de connexion
  getsockname(m->sock, (struct sockaddr*) &mes_infos, &sizeof_mes_infos);
  printf("\nConnexion réussie !\n   Mes infos de connexion : ");
  printf(" [%s::%d]\n", inet_ntoa((struct in_addr) cible.sin_addr), ntohs(cible.sin_port));

  gtk_label_set_text(m->fenetre_connexion.instruction, "Connexion réussie !\nTentative d'authentification.\nVeuillez patienter s'il vous plaît...");

  gdk_threads_leave();
  free(message_erreur);
}





char* lire_mot (int sock, int* nb_lettres);
char stoc (struct main*, int, char*, char*);
int verifier_socket (struct main*, int, char*);

//Fonction d'attente de message sur le socket et de décryptage du message.
//  Sauf erreur de lecture, le message sera rentré dans m->mess
//  La commande par exemple est rentrée dans m->mess.commande
void attendre_et_dechiffrer_message (struct main* m, char commande_attendue)
{
  int taille_mot;
  char* mot;
  char* temp_s;
  char caractere;
  int i;
  int score;
  int sock = m->sock;

  free(m->mess.description_commande);
  m->mess.description_commande = concat_string("", "");

  //Réception de la commande
  mot = lire_mot(sock, &taille_mot);
  caractere = stoc(m, taille_mot, mot, "'c', 'a', 'P', 'J', 's', 'T', 'M' ou 'W'");


  //Réception des paramètres
  switch (caractere)
  {




    //Commande d'échec/réussite de la connexion.
    case 'c':
      m->mess.commande = 'c';

      m->mess.description_commande = concat_string_gfree(m->mess.description_commande, "Récupération de l'information de connexion :\n   ");
      printf("Commande reçue : %c\n   ", caractere);

      //Récupération de l'information de connexion.
      free(mot);
      mot = lire_mot(sock, &taille_mot);
      caractere = stoc(m, taille_mot, mot, "'0', 'J', 'A' ou 'W'");

      switch (caractere)
      {
        case '0':
          m->mess.identification = '0';
          m->mess.description_commande = concat_string_gfree(m->mess.description_commande, "Échec de la connexion.");
          printf("Recu message 'echec de la connexion'\n");
          break;
        case 'J':
          m->mess.identification = 'J';
          m->mess.description_commande = concat_string_gfree(m->mess.description_commande, "Connexion réussie en tant que joueur.");
          printf("Recu message 'connexion réussie en tant que joueur'\n");
          break;
        case 'A':
          m->mess.identification = 'A';
          m->mess.description_commande = concat_string_gfree(m->mess.description_commande, "Connexion réussie en tant qu'Admin.");
          printf("Recu message 'connexion réussie en tant qu'admin'\n");
          break;
        case 'W':
          m->mess.identification = 'W';
          m->mess.description_commande = concat_string_gfree(m->mess.description_commande, "Connexion impossible car aucun admin n'est connecté.");
          printf("Recu message 'connexion impossible car aucun admin n'est connecté'\n");
          break;
        default:
          m->mess.commande = '\0';
          m->mess.description_commande = concat_string_bfree(m->mess.description_commande, concat_string_gfree(concat_string("Erreur d'argument : ", mot), " (attendu : '0', 'A', 'J' ou 'W')"));
          fprintf(stderr, "Recu message 'connexion' avec argument invalide : %s (attendu : '0', 'A', 'J' ou 'W')\n", mot);
          free(mot);
          retour_fenetre_connexion(m, m->mess.description_commande, false);
          break;
      }
      break;



    //Commande de validation de la création d'un utilisateur
    case 'a':
      m->mess.commande = 'a';

      m->mess.description_commande = concat_string_gfree(m->mess.description_commande, "Récupération de l'information de succès de création d'utilisateur :\n   ");
      printf("Commande reçue : %c\n   ", caractere);

      //Récupération de la valeur d'acceptation.
      free(mot);
      mot = lire_mot(sock, &taille_mot);
      verifier_socket (m, taille_mot, mot);
      free(m->mess.pseudo);
      m->mess.pseudo = mot;

      mot = lire_mot(sock, &taille_mot);
      caractere = stoc(m, taille_mot, mot, "'0' ou '1'");

      switch (caractere)
      {
        case '0':
          m->mess.accepte = 0;
          m->mess.description_commande = concat_string_gfree(m->mess.description_commande, "Échec de la création.");
          printf("Recu message 'echec de la création d'utilisateur.'\n");
          break;
        case '1':
          m->mess.accepte = 1;
          m->mess.description_commande = concat_string_gfree(m->mess.description_commande, "Réussite de la création.");
          printf("Recu message 'réussite de la création d'utilisateur.'\n");
          break;
        default:
          m->mess.commande = '\0';
          m->mess.description_commande = concat_string_bfree(m->mess.description_commande, concat_string_dfree("Erreur d'argument : ", concat_string(mot, " (attendu : '0' ou '1')")));
          fprintf(stderr, "Recu message 'succès de création d'utilisateur' avec argument invalide : %s (attendu : '0' ou '1')\n", mot);
          free(mot);
          retour_fenetre_connexion(m, m->mess.description_commande, false);
      }
      break;



    //Commande de demande d'acceptation d'un joueur
    case 'P':
      m->mess.commande = 'P';

      m->mess.description_commande = concat_string_gfree(m->mess.description_commande, "Récupération de l'information de demande d'acceptation de la part d'un joueur auprès de l'administrateur :\n   ");
      printf("Commande reçue : %c\n   ", caractere);

      //Récupération de l'id
      free(mot);
      mot = lire_mot(sock, &taille_mot);
      verifier_socket (m, taille_mot, mot);
      m->mess.id = atoi(mot);

      //Récupération du pseudo
      free(mot);
      mot = lire_mot(sock, &taille_mot);
      verifier_socket (m, taille_mot, mot);
      free(m->mess.pseudo);
      m->mess.pseudo = mot;

      mot = NULL;
      break;




    //Commande d'acceptation/refus de l'administrateur
    case 'J':
      m->mess.commande = 'J';

      m->mess.description_commande = concat_string_gfree(m->mess.description_commande, "Récupération de l'information de validation par l'administrateur :\n   ");
      printf("Commande reçue : %c\n   ", caractere);

      //Récupération de la valeur d'acceptation.
      free(mot);
      mot = lire_mot(sock, &taille_mot);
      caractere = stoc(m, taille_mot, mot, "'0' ou '1'");

      switch (caractere)
      {
        case '0':
          m->mess.accepte = 0;
          m->mess.description_commande = concat_string_gfree(m->mess.description_commande, "Échec de la validation.");
          printf("Recu message 'echec de la validation par l'admin.'\n");
          break;
        case '1':
          m->mess.accepte = 1;
          m->mess.description_commande = concat_string_gfree(m->mess.description_commande, "Réussite de la validation.");
          printf("Recu message 'réussite de la validation par l'admin.'\n");
          break;
        default:
        m->mess.commande = '\0';
        m->mess.description_commande = concat_string_bfree(m->mess.description_commande, concat_string_dfree("Erreur d'argument : ", concat_string(mot, " (attendu : '0' ou '1')")));
          fprintf(stderr, "Recu message 'acceptation administrateur' avec argument invalide : %s (attendu : '0' ou '1')\n", mot);
          free(mot);
          retour_fenetre_connexion(m, m->mess.description_commande, false);
      }
      break;




    //Commande de début de partie
    case 's':
      m->mess.commande = 's';

      m->mess.description_commande = concat_string_gfree(m->mess.description_commande, "Début de la partie :\n   ");
      printf("Commande reçue : %c\n", caractere);

      free(mot);
      mot = lire_mot(sock, &taille_mot);
      verifier_socket (m, taille_mot, mot);

      m->mess.nb_col_map = atoi(mot);
      if (m->mess.nb_col_map < 5)
      {
        m->mess.commande = '\0';
        m->mess.description_commande = concat_string_gfree(concat_string_gfree(concat_string_gfree(m->mess.description_commande, "nombre de colonnes sur la map ("), mot), ") non-valide.");
        fprintf(stderr, "Recu message 'début partie' avec nombre de colonnnes invalide : %s (< 5)\n", mot);
        free(mot);
        retour_fenetre_connexion(m, m->mess.description_commande, false);
        return;
      }

      free(mot);
      mot = lire_mot(sock, &taille_mot);
      verifier_socket (m, taille_mot, mot);

      m->mess.nb_lig_map = atoi(mot);
      if (m->mess.nb_lig_map < 5)
      {
        m->mess.commande = '\0';
        m->mess.description_commande = concat_string_gfree(concat_string_gfree(concat_string_gfree(m->mess.description_commande, "nombre de lignes sur la map ("), mot), ") non-valide.");
        fprintf(stderr, "Recu message 'début partie' avec nombre de lignes invalide : %s (< 5)\n", mot);
        free(mot);
        retour_fenetre_connexion(m, m->mess.description_commande, false);
        return;
      }

      free(mot);
      mot = lire_mot(sock, &taille_mot);
      verifier_socket (m, taille_mot, mot);

      m->mess.nb_joueurs = atoi(mot);
      if (m->mess.nb_joueurs < 2 || m->mess.nb_joueurs > 9)
      {
        m->mess.commande = '\0';
        m->mess.description_commande = concat_string_gfree(concat_string_gfree(concat_string_gfree(m->mess.description_commande, "nombre de joueurs ("), mot), ") non-valide.");
        fprintf(stderr, "Recu message 'début partie' avec nombre de joueurs invalide : %s\n", mot);
        free(mot);
        retour_fenetre_connexion(m, m->mess.description_commande, false);
        return;
      }

      for(i=0; i<m->mess.nb_joueurs; i++)
      {
        mot = lire_mot(sock, &taille_mot);
        verifier_socket (m, taille_mot, mot);
        m->mess.pseudo_joueur[i] = mot;
        printf("pseudo joueur %d : %s\n", i, m->mess.pseudo_joueur[i]);
      }
      for (i=0; i < m->mess.nb_joueurs; i++) printf("Pseudo joueur %d : %s\n", i, m->mess.pseudo_joueur[i]); 

      mot = NULL;

      printf("Recu message 'début partie' avec %d joueurs, et une map de %d x %d\n", m->mess.nb_joueurs, m->mess.nb_lig_map, m->mess.nb_col_map);
      break;




    //Commande de tour suivant
    case 'T':
      m->mess.commande = 'T';

      m->mess.description_commande = concat_string_gfree(m->mess.description_commande, "Tour suivant :\n   ");
      printf("Commande reçue : %c\n", caractere);

      free(mot);
      mot = lire_mot(sock, &taille_mot);
      verifier_socket (m, taille_mot, mot);

      m->mess.joueur_dont_cest_le_tour = atoi(mot);
      if (m->mess.joueur_dont_cest_le_tour < 1 || m->mess.joueur_dont_cest_le_tour > 10)
      {
        m->mess.commande = '\0';
        m->mess.description_commande = concat_string_gfree(concat_string_gfree(concat_string_gfree(m->mess.description_commande, "numero de joueur qui joue ("), mot), ") non-valide.");
        fprintf(stderr, "Recu message 'tour suivant' avec numero de joueur qui joue invalide : %s\n", mot);
        free(mot);
        retour_fenetre_connexion(m, m->mess.description_commande, false);
        return;
      }

      free(mot);
      mot = lire_mot(sock, &taille_mot);
      verifier_socket (m, taille_mot, mot);

      m->mess.joueur_score_update = atoi(mot);
      if (m->mess.joueur_score_update < 1 || m->mess.joueur_score_update > 9)
      {
        m->mess.commande = '\0';
        m->mess.description_commande = concat_string_gfree(concat_string_gfree(concat_string_gfree(m->mess.description_commande, "numero de joueur à updater ("), mot), ") non-valide.");
        fprintf(stderr, "Recu message 'tour suivant' avec numero de joueur à updater invalide : %s\n", mot);
        free(mot);
        retour_fenetre_connexion(m, m->mess.description_commande, false);
        return;
      }

      free(mot);
      mot = lire_mot(sock, &taille_mot);
      verifier_socket (m, taille_mot, mot);

      score = atoi(mot);
      if (score >= 0 && score < 10)
      {
        temp_s = malloc(2 * sizeof(char));
        temp_s[1]='\0';
        temp_s[0] = score + 48;
      }
      else if (score < 100)
      {
        temp_s = malloc(3 * sizeof(char));
        temp_s[2]='\0';
        temp_s[1] = score % 10 + 48;
        temp_s[0] = score / 10 + 48;
      }
      else
      {
        m->mess.commande = '\0';
        m->mess.description_commande = concat_string_gfree(concat_string_gfree(concat_string_gfree(m->mess.description_commande, "score ("), mot), ") non-valide.");
        fprintf(stderr, "Recu message 'tour suivant' avec score invalide : %s\n", mot);
        free(mot);
        retour_fenetre_connexion(m, m->mess.description_commande, false);
        return;
      }

      printf ("Conversion du score en string : %s->%d->%s", mot, score, temp_s);
      free(m->mess.score);
      m->mess.score = temp_s;

      free(mot);
      mot = lire_mot(sock, &taille_mot);
      m->mess.map = mot;

      mot = NULL;
      printf("Recu message 'tour suivant' avec un nouveau score de %s, pour le tour du joueur %d, et avec la map suivante :\n%s\n", m->mess.score, m->mess.joueur_dont_cest_le_tour, m->mess.map);
      break;



    //Commande de coup mal placé
    case 'M':
      m->mess.commande = 'M';
      break;



    //Commande de fin de partie
    case 'W':
      m->mess.commande = 'W';

      m->mess.description_commande = concat_string_gfree(m->mess.description_commande, "Fin de partie :\n   ");
      printf("Commande reçue : %c\n", caractere);

      mot = lire_mot(sock, &taille_mot);
      verifier_socket (m, taille_mot, mot);

      m->mess.winner = atoi(mot);
      if (m->mess.winner < 1 || m->mess.joueur_dont_cest_le_tour > 9)
      {
        m->mess.commande = '\0';
        m->mess.description_commande = concat_string_gfree(concat_string_gfree(concat_string_gfree(m->mess.description_commande, "numero de joueur à updater ("), mot), ") non-valide.");
        fprintf(stderr, "Recu message 'tour suivant' avec numero de joueur à updater invalide : %s\n", mot);
        free(mot);
        retour_fenetre_connexion(m, m->mess.description_commande, false);
        return;
      }

      printf("Recu message 'fin de partie' avec le numéro du gagnant = %d\n", m->mess.winner);
      break;




    default:
      m->mess.commande = '\0';
      m->mess.description_commande = concat_string_bfree(m->mess.description_commande, concat_string_dfree("Commande invalide : ", concat_string(mot, " (attendu : 'c', 'a', 'P', 'J', 's', 'T', 'M' ou 'W')")));
      fprintf(stderr, "Commande invalide : %s (attendu : 'c', 'a', 'P', 'J', 's', 'T', 'M' ou 'W')\n", mot);
      free(mot);
      retour_fenetre_connexion(m, m->mess.description_commande, false);
  }
  free(mot);
}



//Fonction qui va lire un mot sur le socket.
//  Ignore les espaces ou '\0' au début.
//  Dès la lecture d'un autre caractère, lit jusqu'à la prochaine espace ou au prochain '\0'.
//  Renvoie la chaîne lue après allocation.
//  Place le nombre de lettres de cette chaîne, '\0' exclu, dans nb_lettres.
//  Si la connexion avec le socket est perdue, place 0 dans nb_lettres. (cf verifier_socket ci-après)
char* lire_mot (int sock, int* nb_lettres)
{
  char buff[2];
  char caractere;
  char* mot;
  int i = 0;
  mot = concat_string("", "");
  int lu;
  buff[1] = '\0';


  printf("\nRéception du mot : ->");

  do
  {
    lu = read(sock, buff, 1);

    if(lu <= 0)
    {
      *nb_lettres = 0;
      return mot;
    }

    caractere = buff[0];
    if (caractere == ' ' || caractere == '\0')
    {
      if (i == 0) continue;
      *nb_lettres = i;
      printf("\nMot lu : %s\n", mot);
      return mot;
    }
    printf("%c", caractere);
    i++;
    mot = concat_string_gfree(mot, buff);
  } while (1);
}




//Cette fonction convertit une chaîne de caractère d'un caractère (+ éventuellement \0) en caractère.
//  Elle effectue préalablement les tests nécessaires, sur la taille du mot passée en paramètre
//  Elle prend en paramètre la structure générale, la taille du mot, le mot, et une chaîne de caractère affichant les caractères attendus en cas d'erreur.
//  Elle fait appel à verifier_socket (voir ci-après) pour vérifier que la connexion avec le socket n'a pas été perdue.
char stoc (struct main* m, int taille_mot, char* mot, char* desc_attendu)
{
  //On vérifie que la connexion avec le socket n'a pas été perdue
  if(verifier_socket (m, taille_mot, mot)) return '\0';

  //On vérifie que la chaîne ne contient qu'un caractère
  if (taille_mot != 1)
  {
    m->mess.commande = '\0';
    m->mess.description_commande = concat_string_bfree(m->mess.description_commande, concat_string_dfree("Erreur : reçu mot : ", concat_string_dfree(mot, concat_string(" ; attendu caractère simple : ", desc_attendu))));
    fprintf(stderr, "Erreur : reçu mot : %s ; attendu caractère simple : %s\n", mot, desc_attendu);
    free(mot);

    retour_fenetre_connexion(m, m->mess.description_commande, false);
    return '\0';
  }

  //On retourne ce caractère
  return mot[0];
}



//Fonction qui vérifie que la connexion avec le socket n'a pas été perdue.
//  A appeler après lire_mot, en lui fournissant ce qu'a modifié lire_mot.
int verifier_socket (struct main* m, int taille_mot, char* mot)
{
  if (taille_mot == 0)
  {
    m->mess.commande = '\0';
    m->mess.description_commande = concat_string_gfree(m->mess.description_commande, "Erreur de lecture sur le socket.");
    fprintf(stderr, "Erreur de lecture sur le socket.\n");
    free(mot);

    retour_fenetre_connexion(m, m->mess.description_commande, false);
    return -1;
  }
  return 0;
}
