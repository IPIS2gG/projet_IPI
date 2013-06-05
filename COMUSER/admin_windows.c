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
/*         Fonction de gestion des fenêtres         */
/*                                                  */
/*             RÉPARTITION DES TÂCHES :             */
/*      - créé par Beaume Jérémy                    */
/*         (design et gestion primaire des fenêtres)*/
/*      - modifié par Métraud Alexandre             */
/*         (portage dans le reste du programme)     */
/****************************************************/


#include "comuser.h"
#include <string.h>



//____FENÊTRE PRE_GAME____


void signal_create_user(GtkWidget* widget, void* data);
void signal_create_game(GtkWidget* widget, void* data);


//Fonction de création de la fenêtre pre_game
void creer_fenetre_pre_game (struct main* m)
{
	//création de la fenêtre de pré-game

	//indentation = inclusions des widgets
	GtkWidget* window;
		GtkWidget* main_Vbox;
			GtkWidget* frame_new_user;
				GtkWidget* table_new_user;
					GtkWidget* label_login;
					GtkWidget* entry_login;
					GtkWidget* label_mdp;
					GtkWidget* entry_mdp;
					GtkWidget* bouton_create_user;
					GtkWidget* label_rep_authserv;
			GtkWidget* frame_create_part;
				GtkWidget* table_create_part;
					GtkWidget* label_w;
					GtkWidget* entry_w;
					GtkWidget* label_h;
					GtkWidget* entry_h;
					GtkWidget* label_nbj;
					GtkWidget* entry_nbj;
					GtkWidget* label_nbc;
					GtkWidget* entry_nbc;
					GtkWidget* label_err;
					GtkWidget* bouton_launch;
	
	gulong signal_window_destroy_handler;

  // Création de la fenêtre
	window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_MOUSE);
	gtk_window_set_title(GTK_WINDOW(window), "Admin : création partie");

		// Conteneur principal
		main_Vbox=gtk_vbox_new(FALSE, 10);
		gtk_container_add(GTK_CONTAINER(window), main_Vbox);

			// Cadre de création d'un utilisateur
			frame_new_user=gtk_frame_new("création d'un utilisateur");
			gtk_box_pack_start(GTK_BOX(main_Vbox),
												frame_new_user,
												TRUE, TRUE, 0);
				// Table de placement pour la création d'utilisateurs
				table_new_user=gtk_table_new(4, 2, FALSE);
				gtk_container_add(GTK_CONTAINER(frame_new_user), table_new_user);
				
					// Widgets pour création utilisateur
					label_login=gtk_label_new("login : ");
					gtk_table_attach(GTK_TABLE(table_new_user), label_login,
							0, 1, 0, 1, GTK_EXPAND, GTK_EXPAND, 5, 5);
					entry_login=gtk_entry_new();
					gtk_table_attach(GTK_TABLE(table_new_user), entry_login,
							1, 2, 0, 1, GTK_EXPAND, GTK_EXPAND, 5, 5);

					label_mdp=gtk_label_new("password : ");
					gtk_table_attach(GTK_TABLE(table_new_user), label_mdp,
							0, 1, 1, 2, GTK_EXPAND, GTK_EXPAND, 5, 5);
					entry_mdp=gtk_entry_new();
					gtk_table_attach(GTK_TABLE(table_new_user), entry_mdp,
							1, 2, 1, 2, GTK_EXPAND, GTK_EXPAND, 5, 5);

					bouton_create_user=gtk_button_new_with_label("Créer utilisateur");
					gtk_table_attach(GTK_TABLE(table_new_user), bouton_create_user,
							0, 2, 2, 3, GTK_EXPAND, GTK_EXPAND, 5, 5);

					label_rep_authserv=gtk_label_new("");
					gtk_table_attach(GTK_TABLE(table_new_user), label_rep_authserv,
							0, 2, 3,4, GTK_EXPAND, GTK_EXPAND, 5, 5);

			// Cadre de création d'un utilisateur
			frame_create_part=gtk_frame_new("création de la partie");
			gtk_box_pack_start(GTK_BOX(main_Vbox),
												frame_create_part,
												TRUE, TRUE, 0);
				// Table de placement pour la création d'utilisateurs
				table_create_part=gtk_table_new(4, 4, FALSE);
				gtk_container_add(GTK_CONTAINER(frame_create_part), table_create_part);

					// Widgets pour création partie
					label_w=gtk_label_new("largueur");
					gtk_table_attach(GTK_TABLE(table_create_part), label_w,
							0, 1, 0, 1, GTK_EXPAND, GTK_EXPAND, 5, 5);
					entry_w=gtk_entry_new();
					gtk_table_attach(GTK_TABLE(table_create_part), entry_w,
							0, 1, 1, 2, GTK_EXPAND, GTK_EXPAND, 5, 5);

					label_h=gtk_label_new("hauteur");
					gtk_table_attach(GTK_TABLE(table_create_part), label_h,
							1, 2, 0, 1, GTK_EXPAND, GTK_EXPAND, 5, 5);
					entry_h=gtk_entry_new();
					gtk_table_attach(GTK_TABLE(table_create_part), entry_h,
							1, 2, 1, 2, GTK_EXPAND, GTK_EXPAND, 5, 5);

					label_nbj=gtk_label_new("nb joueur max");
					gtk_table_attach(GTK_TABLE(table_create_part), label_nbj,
							2, 3, 0, 1, GTK_EXPAND, GTK_EXPAND, 5, 5);
					entry_nbj=gtk_entry_new();
					gtk_table_attach(GTK_TABLE(table_create_part), entry_nbj,
							2, 3, 1, 2, GTK_EXPAND, GTK_EXPAND, 5, 5);

					label_nbc=gtk_label_new("nb coups max");
					gtk_table_attach(GTK_TABLE(table_create_part), label_nbc,
							3, 4, 0, 1, GTK_EXPAND, GTK_EXPAND, 5, 5);
					entry_nbc=gtk_entry_new();
					gtk_table_attach(GTK_TABLE(table_create_part), entry_nbc,
							3, 4, 1, 2, GTK_EXPAND, GTK_EXPAND, 5, 5);

					label_err=gtk_label_new("");
					gtk_table_attach(GTK_TABLE(table_create_part), label_err,
							0, 4, 2, 3, GTK_EXPAND, GTK_EXPAND, 5, 5);

					bouton_launch=gtk_button_new_with_label("Créer partie");
					gtk_table_attach(GTK_TABLE(table_create_part), bouton_launch,
							0, 4, 3, 4, GTK_EXPAND, GTK_EXPAND, 5, 5);

	//Signaux et initialisation de la structure générale
    //  Clic sur la croix de la fenêtre
    //    On garde signal_window_destroy_handler afin de désactiver ce signal quand on détruit la fenêtre avec gtk_widget_destroy au lieu de cliquer sur la croix.
    //      (cf detruire_fenetres, dans fonctions_fenetres.c)
	  signal_window_destroy_handler=g_signal_connect(GTK_OBJECT(window),
							  "destroy", G_CALLBACK(retour_fenetre_connexion_clic_croix), (gpointer) m);
	
	  // Création utilisateur
		m->fenetre_pre_game.param_create_user.entry_login=GTK_ENTRY(entry_login);
		m->fenetre_pre_game.param_create_user.entry_mdp=GTK_ENTRY(entry_mdp);
		m->fenetre_pre_game.param_create_user.label_err=GTK_LABEL(label_rep_authserv);
    m->fenetre_pre_game.param_create_user.thread = NULL;
    m->fenetre_pre_game.param_create_user.thread_ouvert = false;
    m->fenetre_pre_game.param_create_user.bouton=bouton_create_user;
		g_signal_connect(GTK_OBJECT(bouton_create_user), "clicked",
								G_CALLBACK(signal_create_user),
								 m);
		g_signal_connect(GTK_OBJECT(entry_login), "activate",
								G_CALLBACK(signal_create_user),
								 m);
		g_signal_connect(GTK_OBJECT(entry_mdp), "activate",
								G_CALLBACK(signal_create_user),
								 m);

	  // Création partie
		m->fenetre_pre_game.param_create_game.entry_w=entry_w;
		m->fenetre_pre_game.param_create_game.entry_h=entry_h;
		m->fenetre_pre_game.param_create_game.entry_nbj=entry_nbj;
		m->fenetre_pre_game.param_create_game.entry_nbc=entry_nbc;
		m->fenetre_pre_game.param_create_game.label_err=label_err;

		g_signal_connect(GTK_OBJECT(bouton_launch), "clicked",
								G_CALLBACK(signal_create_game),
								 m);
		g_signal_connect(GTK_OBJECT(entry_w), "activate",
								G_CALLBACK(signal_create_game),
								 m);
		g_signal_connect(GTK_OBJECT(entry_h), "activate",
								G_CALLBACK(signal_create_game),
								 m);
		g_signal_connect(GTK_OBJECT(entry_nbj), "activate",
								G_CALLBACK(signal_create_game),
								 m);
		g_signal_connect(GTK_OBJECT(entry_nbc), "activate",
								G_CALLBACK(signal_create_game),
								 m);

  //Enregistrement de la fenêtre dans la structure globale.
  m->fenetre_pre_game.adresse = window;
  m->fenetre_pre_game.open = false;
  m->fenetre_pre_game.sig_destroy = signal_window_destroy_handler;
}



//Fonction déclenchée lors de l'appui sur le bouton valider de la partie "création de joueur".
//   Elle vérifie l'intégrité des logins et mot de passe rentrés dans les champs.
//   Elle fait ensuite appel à la fonction creer_utilisateur dans fonctions_admin.c qui enverra le message de création et attendra la réponse.
void signal_create_user(GtkWidget* widget, gpointer data)
{
	struct main* m = (struct main*) data;
	char* message;
  int i = 0;

	char* login = (char*) gtk_entry_get_text(m->fenetre_pre_game.param_create_user.entry_login);
	char* mdp = (char*) gtk_entry_get_text(m->fenetre_pre_game.param_create_user.entry_mdp);

  printf("Appuis sur 'créer utilisateur'. Login : %s. MDP : %s.\n", login, mdp);

  while (*(login + i)) i++;
	if(i<3)
	{
    printf("Erreur : Login de taille %d < 3\n", i);
    gtk_label_set_text(m->fenetre_pre_game.param_create_user.label_err, "Le login doit faire trois caractères minimum.");
    return ;
  }
	if(i>15)
	{
    printf("Erreur : Login de taille %d > 15\n", i);
    gtk_label_set_text(m->fenetre_pre_game.param_create_user.label_err, "Le login doit faire quinze caractères maximum.");
    return;
  }

  i = 0;
  while (*(mdp + i)) i++;
	if(i<3)
	{
    printf("Erreur : Mot de passe de taille %d < 3\n", i);
    gtk_label_set_text(m->fenetre_pre_game.param_create_user.label_err, "Le mot de passe doit faire trois caractères minimum.");
    return ;
  }
	if(i>15)
	{
    printf("Erreur : Mot de passe de taille %d > 15\n", i);
    gtk_label_set_text(m->fenetre_pre_game.param_create_user.label_err, "Le mot de passe doit faire quinze caractères maximum.");
    return ;
  }

  message = concat_string_gfree(concat_string_gfree(concat_string_gfree(concat_string("Création de l'utilisateur ", login), " (mdp : "), mdp), ").");
  gtk_label_set_text(m->fenetre_pre_game.param_create_user.label_err, message);
  free(message);

  creer_utilisateur(m, (char*) login, (char*) mdp);
}

//Fonction déclenchée lors de l'appui sur le bouton valider de la partie "création de partie".
//   Elle vérifie l'intégrité des chaînes de charactères rentrées dans les champs.
//   Elle fait ensuite appel à la fonction creer_jeu dans fonctions_admin.c qui enverra le message de création et attendra la réponse.
void signal_create_game(GtkWidget* widget, gpointer data)
{
	//LOCK GTK OK
	struct main* m = (struct main*) data;
  struct param_signal_create_game* param = &(m->fenetre_pre_game.param_create_game);
	int w;
	int h;
	int nbj;
  int nbc;
	GtkWidget* label_err=param->label_err;

	w=atoi(gtk_entry_get_text(GTK_ENTRY(param->entry_w)));
	h=atoi(gtk_entry_get_text(GTK_ENTRY(param->entry_h)));
	nbj=atoi(gtk_entry_get_text(GTK_ENTRY(param->entry_nbj)));
	nbc=atoi(gtk_entry_get_text(GTK_ENTRY(param->entry_nbc)));
	
	if(w<5)
	{	gtk_label_set_text(GTK_LABEL(label_err), "w doit être >=5"); return ;}
	if(h<5)
	{	gtk_label_set_text(GTK_LABEL(label_err), "h doit être >=5"); return ;}
	if(nbj<2)
	{	gtk_label_set_text(GTK_LABEL(label_err), "Le nombre de joueurs doit être >=2"); return ;}
	if(nbj>9)
	{	gtk_label_set_text(GTK_LABEL(label_err), "Le nombre de joueurs doit être <=9"); return ;}
	if(nbc < nbj)
	{	gtk_label_set_text(GTK_LABEL(label_err), "Le nombre de coups doit être >= nb de joueurs"); return ;}

	printf("Création de la partie : %dx%d , %d joueurs max\n", w, h, nbj);

	creer_jeu (m, h, w, nbj, nbc);
}







//____FENÊTRE ACCEPT_JOUEUR____


void add_demande(struct main* m, GtkWidget* Vbox_layout, GtkWidget* label_aff, const char* pseudo, int id);
int aff_new_player_accepted(GtkWidget* label_aff, char* pseudo);
void signal_refuse_player(GtkWidget* bouton, void* param);
void signal_accept_player(GtkWidget* bouton, void* param);
struct param_click_accept_player //signaux des boutons, avec le paramètre
{
  struct main* m; //structure globale
	int id; //id reçu de comserv
	char* pseudo; //chaine malloc du pseudo
	GtkWidget* label; //affichage des joueurs acceptés
};
void destroy_choice_item(GtkWidget* bouton);
void MAJ_scroll_size(GtkWidget* layout_Vbox);
void gestion_decision_admin(struct main* m, GtkWidget* label_aff, int id, char* pseudo, int accepted);	
void signal_click_launch_game(GtkWidget* bouton, void * data);


//Fonction de création de la fenêtre accept_joueur
void creer_fenetre_accept_joueur(struct main* m)
{
	GtkWidget* window;
		GtkWidget* main_Vbox;
			GtkWidget* Hbox_demandes;
				GtkWidget* layout_demandes;
					GtkWidget* Vbox_layout;
				GtkWidget* scrollbar;
			GtkWidget* label_accepted;
			GtkWidget* label_aff_accepted;
			GtkWidget* bouton_launch;
			
	gulong signal_window_destroy_handler;
	
	// Création de la fenêtre
	window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_MOUSE);
	gtk_window_set_title(GTK_WINDOW(window), "Admin : accepter joueur");
	gtk_window_set_default_size(GTK_WINDOW(window), 300, 200);
	
		// Conteneur principal
		main_Vbox=gtk_vbox_new(FALSE, 10);
		gtk_container_add(GTK_CONTAINER(window), main_Vbox);
		
			// Hbox_demandes
			Hbox_demandes=gtk_hbox_new(FALSE, 10);
			gtk_box_pack_start(GTK_BOX(main_Vbox), Hbox_demandes, TRUE,TRUE, 0);
			
				// Widgets de demandes
				layout_demandes=gtk_layout_new(NULL, NULL);
				gtk_layout_set_size(GTK_LAYOUT(layout_demandes), 100, 100);
				gtk_box_pack_start(GTK_BOX(Hbox_demandes), layout_demandes, TRUE,TRUE, 0);
					
					// Vbox layout
					Vbox_layout=gtk_vbox_new(FALSE, 10);
					gtk_container_add(GTK_CONTAINER(layout_demandes), Vbox_layout);
			
				scrollbar=gtk_vscrollbar_new(gtk_layout_get_vadjustment(GTK_LAYOUT(layout_demandes)) );
				gtk_box_pack_start(GTK_BOX(Hbox_demandes), scrollbar, FALSE,FALSE, 0);
				
			label_accepted=gtk_label_new("Joueurs acceptés :");
			gtk_box_pack_start(GTK_BOX(main_Vbox), label_accepted, FALSE,FALSE, 0);
			label_aff_accepted=gtk_label_new("");
			gtk_box_pack_start(GTK_BOX(main_Vbox), label_aff_accepted, FALSE,FALSE, 0);
			
			bouton_launch=gtk_button_new_with_label("Commencer la partie");
			gtk_box_pack_start(GTK_BOX(main_Vbox), bouton_launch, FALSE,FALSE, 0);
			gtk_widget_set_sensitive (bouton_launch, FALSE);


	//Signaux
    //  Clic sur la croix de la fenêtre
    //    On garde signal_window_destroy_handler afin de désactiver ce signal quand on détruit la fenêtre avec gtk_widget_destroy au lieu de cliquer sur la croix.
    //      (cf detruire_fenetres, dans fonctions_fenetres.c)
    signal_window_destroy_handler=g_signal_connect(GTK_OBJECT(window),
  							"destroy", G_CALLBACK(retour_fenetre_connexion_clic_croix), (gpointer) m);

    g_signal_connect(GTK_OBJECT(bouton_launch), "clicked",
  								G_CALLBACK(signal_click_launch_game),
  								 m);


  //Enregistrement de la fenêtre dans la structure globale
  m->fenetre_accept_joueur.adresse = window;
  m->fenetre_accept_joueur.open = false;
  m->fenetre_accept_joueur.Vbox_layout = Vbox_layout;
  m->fenetre_accept_joueur.label_aff_accepted = label_aff_accepted;
  m->fenetre_accept_joueur.bouton_launch = bouton_launch;
  m->fenetre_accept_joueur.nb_joueurs_acceptes = 0;
  m->fenetre_accept_joueur.sig_destroy = signal_window_destroy_handler;
}

//Fonction déclenchée lors d'un clic sur un bouton de refus de joueur.
void signal_refuse_player(GtkWidget* bouton, void* param)
{
	destroy_choice_item(bouton);
	struct param_click_accept_player* p=(struct param_click_accept_player*)  param;
  struct main* m = p->m;
	int id=p->id;
	char* pseudo=p->pseudo;
	GtkWidget* label=p->label;
	free(param); //alloué dynamiquement lors de la création du bouton
	gestion_decision_admin(m, label, id, pseudo, 0);
}

//Fonction déclenchée lors d'un clic sur un bouton d'acceptation de joueur.
void signal_accept_player(GtkWidget* bouton, void* param)
{
	destroy_choice_item(bouton);
	struct param_click_accept_player* p=(struct param_click_accept_player*)  param;
  struct main* m = p->m;
	int id=p->id;
	char* pseudo=p->pseudo;
	GtkWidget* label=p->label;
	free(param); //alloué dynamiquement lors de la création du bouton
	gestion_decision_admin(m, label, id, pseudo, 1);
}

//Fonction qui retire une demande de joueur de la liste des demandes
void destroy_choice_item(GtkWidget* bouton)
{
	GtkWidget* layout_Vbox;
	GtkWidget* Hbox;
	
	//on retire la Hbox qui contient le label d'affichage du pseudo et les 2 boutons
	//de la Vbox du layout
	Hbox=gtk_widget_get_parent(bouton);
	layout_Vbox=gtk_widget_get_parent(Hbox);
	gtk_container_remove(GTK_CONTAINER(layout_Vbox), Hbox);
	//et on détruit la Hbox
	//gtk_widget_destroy(Hbox); //Annulé car fait planter GTK+. Cela doit être fait automatiquement.
	
	//la taille du layout doit être MAJ
	MAJ_scroll_size(layout_Vbox);
}

//Fonction qui ajoute une demande de joueur à la liste des demandes
	// Requiert :
  // Vbox du layout, label d'affichage des joueurs acceptés
	// pseudo, id reçu de Comserv
void add_demande(struct main* m, GtkWidget* Vbox_layout, GtkWidget* label_aff, const char* pseudo, int id)
{
	GtkWidget* new_Hbox;
	GtkWidget* label;
	GtkWidget* button_accept;
	GtkWidget* button_refuse;
	struct param_click_accept_player* param;
	char* buff;
	
	new_Hbox=gtk_hbox_new(FALSE, 10);
	gtk_box_pack_start(GTK_BOX(Vbox_layout), new_Hbox, TRUE,TRUE, 0);
	
	label=gtk_label_new(pseudo);
	gtk_box_pack_start(GTK_BOX(new_Hbox), label, TRUE,TRUE, 0);
	
	button_accept=gtk_button_new_with_label("Accepter");
	gtk_box_pack_start(GTK_BOX(new_Hbox), button_accept, TRUE,TRUE, 0);
	
	button_refuse=gtk_button_new_with_label("Refuser");
	gtk_box_pack_start(GTK_BOX(new_Hbox), button_refuse, TRUE,TRUE, 0);
	
	//___SIGNAUX___
	param= (struct param_click_accept_player*) calloc(1, sizeof(struct param_click_accept_player));
	buff=(char*) calloc(strlen(pseudo)+1, sizeof(char));
	strcpy(buff, pseudo);
  param->m=m;
	param->id=id;
	param->pseudo=buff;
	param->label=label_aff;
	
	g_signal_connect(GTK_OBJECT(button_accept), "clicked",
								G_CALLBACK(signal_accept_player),
								 param);
	g_signal_connect(GTK_OBJECT(button_refuse), "clicked",
								G_CALLBACK(signal_refuse_player),
								 param);
	
	//___MAJ du layout___
	MAJ_scroll_size(Vbox_layout);

  gdk_threads_enter();
	gtk_widget_show_all(new_Hbox);
  gdk_threads_leave();
}

//Fonction qui ajoute le pseudo au label d'affichage des joueurs acceptés dans la partie
//  Attention : possède une variable static qui compte le nombre de joueurs acceptés
//    (tous les 3 joueurs insérés, met un \n)
//  Renvoie le nombre de joueurs acceptés.
int aff_new_player_accepted(GtkWidget* label_aff, char* pseudo)
{
	static int i=0; //tous les 3 joueurs affichés, on affiche sur une nouvelle ligne
	const char* text;
	char* buff;
	
	text=gtk_label_get_text(GTK_LABEL(label_aff));
	buff=(char*) calloc(strlen(text)+strlen(pseudo)+2, sizeof(char));

	if((i%3)!=0 || i==0)
				//si on commence pas à 0, yen a que 2 sur la première ligne
				//si on test pas ==0, on commence par afficher le \n
	{
		sprintf(buff,"%s %s", text, pseudo);
		//ouais, même si ya rien dans le label je met un espace au début, bon ...
	}
	else
	{
		sprintf(buff,"%s\n%s", text, pseudo);
	}
	++i;
	
	gtk_label_set_text(GTK_LABEL(label_aff), buff);
	free(buff); //set_text fait une copy

  return i;
}

//Fonction qui met à jour la taille du layout pour que le scrolling soit OK
//  param : Vbox contenu dans le layout
void MAJ_scroll_size(GtkWidget* layout_Vbox)
{
	GtkRequisition req;
	GtkWidget* layout=gtk_widget_get_parent(layout_Vbox);
	
	//on est obligé de MAJ nous même la taille du layout (pas automatique)
	//sinon la scrollbar ne décale pas comme il faut
	gtk_widget_size_request(layout_Vbox, &req);
	//on se base sur la taille de son contenu, à savoir une Vbox
	gtk_layout_set_size(GTK_LAYOUT(layout), req.width+10, req.height+50);
	//+50 sinon ça scroll pas assez, allez savoir WTF ...
}

//Fonction de gestion du refus (0) ou accord(1) de l'admin
//  ATTENTION : Libère le pseudo
void gestion_decision_admin(struct main* m, GtkWidget* label_aff, int id, char* pseudo, int accepted)
{
  char* message_envoye;
  int i;

	printf("%s (id=%d) -> %d\n",pseudo, id, accepted);
  //Si accepté, affiche le joueur dans la liste des joueurs acceptés
	if(accepted!=0)
	{
		m->fenetre_accept_joueur.nb_joueurs_acceptes = aff_new_player_accepted(label_aff,pseudo);

		if (m->fenetre_accept_joueur.nb_joueurs_acceptes > 1) gtk_widget_set_sensitive (m->fenetre_accept_joueur.bouton_launch, TRUE);
	}
	free(pseudo);

  //Envoie l'information
  message_envoye = concat_string_gfree(concat_string_gfree(concat_string("p ", itoa(id)), " "), itoa(accepted));
  i = 0;
  while (*(message_envoye + i)) i++;
  write(m->sock, message_envoye, i+1);
  printf("Commande 'acceptation/refus de joueur par l'admin' envoyée : %s\n", message_envoye);
  free(message_envoye);
}

//Fonction exécutée en cas de clic sur le bouton "lancer la partie"
//  Envoie la commande S.
void signal_click_launch_game(GtkWidget* bouton, void * data)
{
  struct main* m = (struct main*) data;
  if (m->fenetre_accept_joueur.nb_joueurs_acceptes < 2) return;
  write(m->sock, "S", 2);
  printf("Commande 'Lancement de la partie' envoyée.\n");
}
