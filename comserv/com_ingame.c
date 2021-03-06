#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <unistd.h> 
#include <arpa/inet.h>
#include <netinet/in.h> 
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h> 
#include <signal.h>
#include <sys/wait.h>
#include <pthread.h>
#include <map>
#include <vector>
#include <set>

#include "function.h"
#include "com_ingame.h"
#include "gestpart.h"

extern bool aff_debug;

void launch_game(struct param_partie* infos)
{
	int w=infos->w;
	int h=infos->h;
	int nb_coups=infos->nb_coups;
	std::vector<int> tab_stream=infos->tab_stream; //copies, on s'en fou, c'est des int et petite taille
	std::vector<char*> tab_pseudo=infos->tab_pseudo;
	unsigned int nb_joueur=tab_pseudo.size()-1;
	unsigned int i;
	
	partie* part;
	
	char buff_read[50];
	int r;
	int n;
	char* buff;
	char* aff_map;
	
	int joueur_courant=1;
	int joueur_precedent=1;
	int joueur_ecoute=-1;
	
	bool cont; //boucle principale
	bool stop; //partie finie
	bool cont_waiting; //attends la réponse du joueur courant
	
	fd_set readfs; //ensembles des fd lu par select
	fd_set writefs;
	int max_fs=-1; //necessaire pour select
	
	int ret; //retour de select
	int ret_play; //retour play
	int x,y; //position de jeu donnée par le joueur
	
	int joueur_gagnant;
	
	for(i=0; i<tab_stream.size(); ++i)
	{
		if(tab_stream[i]>max_fs)
		{
			max_fs=tab_stream[i];
		}
	}
	
	if(aff_debug){
		print("\n\n###############################\n\nDEBUT DE LA PARTIE\n\n");
		printf("map : %d x %d   nb_coups : %d\n", w, h, nb_coups);
		printf("Admin : %s [%d]\n", tab_pseudo[0], tab_stream[0]);
		for(i=1; i<=nb_joueur; ++i)
		{
			printf("Player : %s [%d]\n", tab_pseudo[i], tab_stream[i]);
		}
		print("\n\n");
		print("Communications des informations aux joueurs et à l'admin\n");}

	//création de la chaine
	buff=(char*) calloc(12+(nb_joueur)*17, sizeof(char));
	sprintf(buff, "s %d %d %d", w, h, nb_joueur);
	for(i=1; i<=nb_joueur; ++i)
	{
		sprintf(buff, "%s %s", buff, tab_pseudo[i]);
	}
	//envoi à tous
	n=strlen(buff)+1;
	for(i=0; i<tab_stream.size(); ++i)
	{
		write(tab_stream[i], buff, n);
	}

	if(aff_debug){
		print("-->");
		write(1, buff, n);
		printf("<-- (%d char)\n", n);fflush(stdout);}

	free(buff);
	
	if(aff_debug){print("Initialisation de la partie .... ");}
	part = init(w,h,nb_joueur, nb_coups);
	if(part==NULL)
	{
		fprintf(stderr, "fatal error => unable to init game");
		exit(-1);
	}
	if(aff_debug){print("ok\n");}
	
	cont=true;
	stop=false;

	buff=(char*) calloc(30+w*h, sizeof(char));
	while(cont)
	{
		//envoi du nouvel etat de jeu
		if(stop)
		{
			joueur_precedent=joueur_courant;
			joueur_courant=10;
		}
		if(aff_debug){print("Envoi à tous le nouvel etat de jeu\n");}
		aff_map=getmap(part);
		sprintf(buff, "T %d %d %d %s", joueur_courant,
									joueur_precedent,
									getscore(part, joueur_precedent),
									aff_map);
		
		
		free(aff_map);
		n=strlen(buff)+1;
		
		if(aff_debug){
			print("-->");
			write(1, buff, n);
			printf("<-- (%d char)\n", n);fflush(stdout);}
		
		for(i=0; i<tab_stream.size(); ++i)
		{
			write(tab_stream[i], buff, n);
		}

		if(stop)
		{ cont = false;}
		else
		{
			cont_waiting=true;
			while(cont_waiting)
			{
				//écoute des gens
				FD_ZERO(&readfs);
				FD_ZERO(&writefs);
				for(i=0; i<tab_stream.size(); ++i) //on écoutera aussi l'admin
				{
					if(tab_stream[i]!=-1)
					{
						FD_SET(tab_stream[i], &readfs);
						FD_SET(tab_stream[i], &writefs);
					}
				}
			
				if(aff_debug){
				printf("Attente de communication (réponse attendue de [%s - %d])\n",
								tab_pseudo[joueur_courant], tab_stream[joueur_courant]);
				fflush(stdout);}

				ret=select(max_fs+1, &readfs, NULL, NULL, NULL);
				if(ret<0)
				{
					fprintf(stderr, "Erreur : seclect %s\n", msg_err);
					exit(-1);
				}

				joueur_ecoute=-1;
			
				//on regarde qui a parlé
				if(FD_ISSET(tab_stream[joueur_courant], &readfs))
				{
					//joueur courant a parlé
					
					if(aff_debug)
						{printf("ecoute stream [%d] ...\n", tab_stream[joueur_courant]);fflush(stdout);}
					joueur_ecoute=joueur_courant;
					r=read(tab_stream[joueur_courant], buff_read, 50);
					if(r<=0)
					{
						printf("Erreur : client (%d) déconnecté\n", tab_stream[joueur_courant]);
						//on retire ce joueur de la table, et on change de joueur
						close(tab_stream[joueur_courant]);
						tab_stream[joueur_courant]=-1;
						cont_waiting=false;
						if(aff_debug)
							{printf("boucle de recherche de joueur (precedant [%d])...\n", joueur_precedent);fflush(stdout);}
						joueur_precedent=joueur_courant;
						do
						{
							joueur_courant=joueur_suivant(part, joueur_courant);
							printf("test joueur_courant [%d]\n", joueur_courant);
							fflush(stdout);
						}
						while(tab_stream[joueur_courant]==-1 && joueur_courant!=joueur_precedent);

						if(aff_debug)
							{printf("fin boucle de recherche de joueur !!!\n");fflush(stdout);}

							//on cherche un nouveau joueur tant que les joueurs suivants sont déco ou
							//qu'on doit demander à celui qui vient de déco de rejouer
						
						if(joueur_courant==joueur_precedent)
						{
							//le prochain joueur qui peut jouer est celui qui vient de déco
							print("Erreur : plus de joueurs connectés");
							stop=true;
						}

					}
					else
					{
						if(aff_debug){
							printf("Joueur courant [%s - %d] (stream %d) a parlé -->",
															tab_pseudo[joueur_courant],
															joueur_courant, tab_stream[joueur_courant]);
							write(1, buff_read, r);
							printf("<-- (%d char)\n", r);
							fflush(stdout);}
						//traitement
						if(buff_read[0]!='P')
						{
							print("Erreur protocole [P x y] -> ignorée\n");
						}
						else
						{
							//Extraction des infos
							sscanf(buff_read, "%*c %2d %2d", &x, &y);

							if(aff_debug){
								printf("Player joue en x=%d y=%d\n", x, y);}

							ret_play=play(part, x,y,joueur_courant);

							if(aff_debug){
								printf("Gestpart répond %d\n", ret_play);
								fflush(stdout);}

							switch(ret_play)
							{
								case -1 :
									//erreur de jeu
									//[buf permet d'envoyer T joueur score map, donc M map ...]
									aff_map=getmap(part);
									sprintf(buff, "M %s", aff_map);
									free(aff_map);
									if(aff_debug){print("Envoi de 'M map' au joueur courant\n");}
									write(tab_stream[joueur_courant], buff, strlen(buff)+1);
									
									if(aff_debug){
										print("-->");
										write(1, buff, strlen(buff)+1);
										printf("<-- (%d char)\n", strlen(buff)+1);fflush(stdout);}
									
									//on laisse cont_waiting=true -> on attend toujours le même joueur
									break;
								case 0 :
									if(aff_debug){print("Victoire d'un joueur\n");}
									cont_waiting=false;
									stop=true;
									break;
								default :
									if(aff_debug){print("Coup correct\n");}

									joueur_precedent=joueur_courant;
									joueur_courant=ret_play;
									if(tab_stream[joueur_courant]==-1)
									{
										if(aff_debug)
											{printf("boucle de recherche de joueur ...\n");fflush(stdout);}
										do
										{
											joueur_courant=joueur_suivant(part, joueur_courant);
										}
										while(tab_stream[joueur_courant]==-1 && joueur_courant!=joueur_precedent);
										if(aff_debug)
											{printf("fin boucle de recherche de joueur !!!\n");fflush(stdout);}

										if(joueur_courant==joueur_precedent)
										{
											//le prochain joueur qui peut jouer est celui qui vient de déco
											print("Erreur : plus de joueurs connectés");
											stop=true;
										}
									}
									if(aff_debug){
									printf("Au joueur [%s - %d] de jouer !\n", tab_pseudo[joueur_courant],
															tab_stream[joueur_courant]);}
									cont_waiting=false;
									break;
							}
						}
					}		
				}
		
				for(i=0; i<tab_stream.size(); ++i)
				{
					//on n'écoute pas le joueur courant
					if((int) i!=joueur_ecoute && FD_ISSET(tab_stream[i], &readfs))
					{
						fflush(stdout);
						if(aff_debug)
						{printf("ecoute stream [%d] ...\n", tab_stream[i]);fflush(stdout);}
						r=read(tab_stream[i], buff_read, 50);
						if(r<=0)
						{
							printf("Erreur : client déconnecté (%d)\n", tab_stream[i]);
							tab_stream[i]=-1;
						}
							if(aff_debug){
							printf("   ERREUR : socket %d a parlé -->", tab_stream[i]);
							write(1, buff_read, r);
							printf("<-- (%d char)\n", r);
							fflush(stdout);
							print("   ==> communication ignorée\n");}
					}
				}
			}
		}
	}
	free(buff);
	//determinantion du gagnant
	joueur_gagnant=getvainqueur(part);
	//envoi à tous des scores finaux [BOURINAGE]
	buff=(char*) calloc(15, sizeof(char));

	if(aff_debug){print("Envoi des données de victoire\n");}

	sprintf(buff, "W %d", joueur_gagnant);

	n=strlen(buff)+1;
	for(i=0; i<=tab_stream.size(); ++i)
	{
		write(tab_stream[i], buff, n);
	}

	if(aff_debug){
		print("-->");
		write(1, buff, n);
		printf("<-- (%d char)\n", n);fflush(stdout);
		free(buff);}
	
	print("Jeu terminé\n");
}
	
