#include <stdio.h>
#include <stdlib.h>
#include <gtk/gtk.h>
#include <string.h>
#include "comuser.h"

void signal_window_destroy(GtkWidget* widget, void* data)
{
	gtk_main_quit();
	printf("KILL EVERYONE !!\n");
	exit(-1);
}

void do_nothing(GtkWidget* widget, void* data)
{ printf("plop\n");};

// README ##############################
/*

bordel c'est chiant de fermer une fenêtre GTK !

après gtk_main, si tu mets gtk_widget_destroy(window), ça plante
il aime pas, il dit qu'il a plus la boucle de gestion des events, alors pas content

ça marche si on met ça dans une fonction appelée en signal
sauf que ça génère dans tous les cas un window::destroy (donc ferme le programme)

j'ai tenté de changer la fonction de traitement de signal, en mettant
la fonction do_nothing ci-dessus, mais ça n'est pas pris en compte
cf fonction void signal_create_game(GtkWidget* widget, void* data)

Si en fonction de callback on met juste gtk_main_quit, il reprendra en fait l'execution du main,
et ferme même pas la fenêtre

ET MAINTENANT, LA SOLUTION :
quand tu fais gtk_widget_destroy après le gtk_main, il râle parce que
ya plus personne pour gérer l'event
on va donc lui dire de plus le gérer
=> on enregistre le gulong renvoyé par g_signal_connect
et avant le gtk_widget_destroy, on disconnect en utilisant le gulong
Et c'est réglé !


REMARQUES :

fenêtre 1 : très simple, pas grand chose à en redire

fenêtre 2 :
tu constatera que j'ai fait toute l'archi de la gestion des évènements

si t'as un socket à passer en param pour communiquer,
(pour l'utiliser dans la fonction gestion_decision_admin, au hasard) :
passe le en param à add_demande,
ajoute le dans la structure de param pour les clicks
(dans add_demande, insère le dans la structure)
=> tu pourras ainsi y acceder dans les fonctions signal_accepted/refuse_player
et ainsi le refiler à gestion_decision_admin

*/
//#######################################################"

void signal_create_user(GtkWidget* widget, void* data);

void signal_create_game(GtkWidget* widget, void* data);

void creer_fenetre_pre_game (struct toutes_les_fenetres* m)
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
		// on garde l'handler sous le coude pour pouvoir enlever la gestion du singal
		//avant de détruire la fenêtre
	//####################

	window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_MOUSE);
	gtk_window_set_title(GTK_WINDOW(window), "Admin : création partie");

		//main_Vbox
		main_Vbox=gtk_vbox_new(FALSE, 10);
		gtk_container_add(GTK_CONTAINER(window), main_Vbox);

			// cadre de création d'un utilisateur
			frame_new_user=gtk_frame_new("création d'un utilisateur");
			gtk_box_pack_start(GTK_BOX(main_Vbox),
												frame_new_user,
												TRUE, TRUE, 0);
				//table de placement pour la création d'utilisateurs
				table_new_user=gtk_table_new(4, 2, FALSE);
				gtk_container_add(GTK_CONTAINER(frame_new_user), table_new_user);
				
					//widgets partie création utilisateur
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

			// cadre de création d'un utilisateur
			frame_create_part=gtk_frame_new("création de la partie");
			gtk_box_pack_start(GTK_BOX(main_Vbox),
												frame_create_part,
												TRUE, TRUE, 0);
				//table de placement pour la création d'utilisateurs
				table_create_part=gtk_table_new(4, 4, FALSE);
				gtk_container_add(GTK_CONTAINER(frame_create_part), table_create_part);

					//création des widgets création partie
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

	//#### SIGNAUX ##########
	
	//######################################################
	//####### A CHANGER ???????????? #######################
	//######################################################
	
	signal_window_destroy_handler=g_signal_connect(GTK_OBJECT(window),
							"destroy", G_CALLBACK(signal_window_destroy), NULL);
	//dans l'état actuel, => gtk_main_quit(); exit(-1);
	//à toi de voir comment tu veux faire
	
	// IL FAUT ABSOLUMENT GARDER LE signal_window_destroy_handler !!!
	// NECESSAIRE POUR FERMER PROPREMENT LA FENETRE

	// ################################
	
	//création utilisateur
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

	//création partie
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

  //Enregistrement de la fenêtre dans la structure globale.
  m->fenetre_pre_game.adresse = window;
  m->fenetre_pre_game.open = false;
  m->fenetre_pre_game.sig_destroy = signal_window_destroy_handler;
	//gtk_widget_show_all(window);
	//gtk_main();
	//g_signal_handler_disconnect(window, signal_window_destroy_handler);
		//DECONNECTE LE SIGNAL
	//gtk_widget_destroy(window); //on peut la détruire, et il râle pas
}

void signal_create_user(GtkWidget* widget, gpointer data)
{
	//LOCK GTK OK
	struct toutes_les_fenetres* m = (struct toutes_les_fenetres*) data;
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

void signal_create_game(GtkWidget* widget, gpointer data)
{
	//LOCK GTK OK
	struct toutes_les_fenetres* m = (struct toutes_les_fenetres*) data;
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
	if(nbc < 10 * nbj)
	{	gtk_label_set_text(GTK_LABEL(label_err), "Le nombre de coups doit être >= 10 * nb de joueurs"); return ;}

	printf("Création de la partie : %dx%d , %d joueurs max\n", w, h, nbj);

	creer_jeu (m, h, w, nbj, nbc);
	//gtk_main_quit();
	//gtk_widget_destroy(window); marcherai, mais créé un window::destroy, pas cool !
}

//####################################################################
//######## 2ème FENETRE ############################################
//###################################################################

void add_demande(struct toutes_les_fenetres* m, GtkWidget* Vbox_layout, GtkWidget* label_aff, const char* pseudo, int id);
	// ajoute une demande avec les boutons etc ...
	// Vbox du layout, label d'affichage des joueurs acceptés
	// pseudo, id reçu de Comserv

int aff_new_player_accepted(GtkWidget* label_aff, char* pseudo);
	//ajoute le pseudo au label d'affichage des joueurs acceptés dans la partie
	//possède une variable static (tous les 3 joueurs insérés, met un \n)
	
void signal_refuse_player(GtkWidget* bouton, void* param); //param malloc
void signal_accept_player(GtkWidget* bouton, void* param); //param malloc
struct param_click_accept_player //signaux des boutons, avec le paramètre
{
  struct toutes_les_fenetres* m;
	int id; //id reçu de comserv
	char* pseudo; //chaine malloc du pseudo
	GtkWidget* label; //affichage des joueurs acceptés
};

void destroy_choice_item(GtkWidget* bouton);
	//retire dans la zone de scroll le bouton et ce qui va avec (label ...)
	//MAJ la taille du layout
	
void MAJ_scroll_size(GtkWidget* layout_Vbox);
	//param : Vbox contenu dans le layout
	//met à jour la taille du layout pour que le scrolling soit OK
	
void gestion_decision_admin(struct toutes_les_fenetres* m, GtkWidget* label_aff, int id, char* pseudo, int accepted);
	//### A TOI DE GERER ICI #######
	//gestion du refus (0) ou accord(1) de l'admin
	//pseudo alloué dynamiquement
	
void signal_click_launch_game(GtkWidget* bouton, void * data);
	// click sur le bouton "lancer la partie"
	// ferme la fenêtre

void creer_fenetre_accept_joueur(struct toutes_les_fenetres* m)
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
	
	//création de la fenêtre
	window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_MOUSE);
	gtk_window_set_title(GTK_WINDOW(window), "Admin : accepter joueur");
	gtk_window_set_default_size(GTK_WINDOW(window), 300, 200);
	
		//main_Vbox
		main_Vbox=gtk_vbox_new(FALSE, 10);
		gtk_container_add(GTK_CONTAINER(window), main_Vbox);
		
			//Hbox_demandes
			Hbox_demandes=gtk_hbox_new(FALSE, 10);
			gtk_box_pack_start(GTK_BOX(main_Vbox), Hbox_demandes, TRUE,TRUE, 0);
			
				//widgets de demandes
				layout_demandes=gtk_layout_new(NULL, NULL);
				gtk_layout_set_size(GTK_LAYOUT(layout_demandes), 100, 100);
				gtk_box_pack_start(GTK_BOX(Hbox_demandes), layout_demandes, TRUE,TRUE, 0);
					
					//Vbox layout
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
			
			
	//#### SIGNAUX ##########
	
	//######################################################
	//####### A CHANGER ???????????? #######################
	//######################################################
	
	signal_window_destroy_handler=g_signal_connect(GTK_OBJECT(window),
							"destroy", G_CALLBACK(signal_window_destroy), NULL);
	//dans l'état actuel, => gtk_main_quit(); exit(-1);
	//à toi de voir comment tu veux faire
	
	// IL FAUT ABSOLUMENT GARDER LE signal_window_destroy_handler !!!
	// NECESSAIRE POUR FERMER PROPREMENT LA FENETRE

	// ################################

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

	//######################################################
	//####### A VIRER => TESTS #############################
	//######################################################
	/*
	char buff[30];
	int i;
	for(i=0; i<30; ++i)
	{
		sprintf(buff, "testplayer%d", i);
		add_demande(Vbox_layout,label_aff_accepted, buff, i);
	}

	gtk_widget_show_all(window);
	
	gtk_main();
	
	g_signal_handler_disconnect(window, signal_window_destroy_handler);
		//DECONNECTE LE SIGNAL
	gtk_widget_destroy(window); //on peut la détruire, et il râle pas*/
}

void signal_refuse_player(GtkWidget* bouton, void* param)
{
	destroy_choice_item(bouton);
	struct param_click_accept_player* p=(struct param_click_accept_player*)  param;
  struct toutes_les_fenetres* m = p->m;
	int id=p->id;
	char* pseudo=p->pseudo;
	GtkWidget* label=p->label;
	free(param); //alloué dynamiquement lors de la création du bouton
	gestion_decision_admin(m, label, id, pseudo, 0);
}

void signal_accept_player(GtkWidget* bouton, void* param)
{
	destroy_choice_item(bouton);
	struct param_click_accept_player* p=(struct param_click_accept_player*)  param;
  struct toutes_les_fenetres* m = p->m;
	int id=p->id;
	char* pseudo=p->pseudo;
	GtkWidget* label=p->label;
	free(param); //alloué dynamiquement lors de la création du bouton
	gestion_decision_admin(m, label, id, pseudo, 1);
}

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
	//gtk_widget_destroy(Hbox);
		//=> en fait ça fait planter GTK, ça doit le faire tout seul =)
	
	//la taille du layout doit être MAJ
	MAJ_scroll_size(layout_Vbox);
}

void add_demande(struct toutes_les_fenetres* m, GtkWidget* Vbox_layout, GtkWidget* label_aff, const char* pseudo, int id)
{
	//ajoute une demande dans le layout
	
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
	
	//##### SIGNAUX #####
	
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
	
	//### MAJ du layout ###########
	MAJ_scroll_size(Vbox_layout);

  gdk_threads_enter();
	gtk_widget_show_all(new_Hbox);
  gdk_threads_leave();
}

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

void gestion_decision_admin(struct toutes_les_fenetres* m, GtkWidget* label_aff, int id, char* pseudo, int accepted)
{
  char* message_envoye;
  int i;

  printf("_______________TEST SOCK gda : %d_____________\n", m->sock);
	// ATTENTION #####################
	//pseudo toujours alloué dynamiquement
	printf("%s (id=%d)  -> %d\n",pseudo, id, accepted);
	if(accepted!=0)
	{
		m->fenetre_accept_joueur.nb_joueurs_acceptes = aff_new_player_accepted(label_aff,pseudo);

		if (m->fenetre_accept_joueur.nb_joueurs_acceptes > 1) gtk_widget_set_sensitive (m->fenetre_accept_joueur.bouton_launch, TRUE);
	}
	free(pseudo);

  message_envoye = concat_string_gfree(concat_string_gfree(concat_string("p ", itoa(id)), " "), itoa(accepted));
  i = 0;
  while (*(message_envoye + i)) i++;
  write(m->sock, message_envoye, i+1);
  printf("Commande 'acceptation/refus de joueur par l'admin' envoyée : %s\n", message_envoye);
  free(message_envoye);
}

void signal_click_launch_game(GtkWidget* bouton, void * data)
{
  struct toutes_les_fenetres* m = (struct toutes_les_fenetres*) data;
  if (m->fenetre_accept_joueur.nb_joueurs_acceptes < 2) return;
  write(m->sock, "S", 2);
  printf("Commande 'Lancement de la partie' envoyée.\n");
}

//###########################################################################################
//############# ##### ##### ##### ### #### ##################################################
//#############  ###  #### # #### ###  ### ##################################################
//############# # # # ### ### ### ### # ## #################################################
//############# ## ## ###     ### ### ## # ##################################################
//############# ##### ### ### ### ### ###  ##################################################
//############# ##### ### ### ### ### #### ##################################################
//###########################################################################################

/*
int main(int argc, char** argv)
{
	gtk_init(&argc, &argv);
	create_window_create_game();
	printf("fenetre 2 maintenant !!!\n");
	create_window_accept_user();
	printf("lancement de la partie\n");
	return 0;
}
*/
