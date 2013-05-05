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

void launch_game(struct param_partie* infos)
{
	int w=infos->w;
	int h=infos->h;
	std::vector<int> tab_stream=infos->tab_stream; //copies, on s'en fou, c'est des int et petite taille
	std::vector<char*> tab_pseudo=infos->tab_pseudo;
	unsigned int nb_joueur=tab_pseudo.size()-1;
	char* buff;
	char buff_read[50];
	int r;
	unsigned int i;
	char* aff_map;
	int n;
	int joueur_courant=1;
	int joueur_precedant=1;
	bool cont; //boucle principale
	fd_set readfs; //ensembles des fd lu par select
	int max_fs=-1; //necessaire pour select
	int ret; //retour de select
	
	for(i=0; i<tab_stream.size(); ++i)
	{
		if(tab_stream[i]>max_fs)
		{
			max_fs=tab_stream[i];
		}
	}
		
	print("\n\n###############################\n\nDEBUT DE LA PARTIE\n\n");
	printf("map : %d x %d\n", w, h);
	printf("Admin : %s [%d]\n", tab_pseudo[0], tab_stream[0]);
	for(i=1; i<=nb_joueur; ++i)
	{
		printf("Player : %s [%d]\n", tab_pseudo[i], tab_stream[i]);
	}
	print("\n\n");
	print("Communications des informations aux joueurs et à l'admin\n");
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
	free(buff);
	
	print("Initialisation de la partie .... ");
	partie* part = init(w,h,nb_joueur);
	if(part==NULL)
	{
		print("fatal error !");
		exit(-1);
	}
	print("ok\n");
	
	cont=true;
	buff=(char*) calloc(12+w*h, sizeof(char));
	while(cont)
	{
		//envoi du nouvel etat de jeu
		print("Envoi à tous le nouvel etat de jeu\n");
		aff_map=getmap(part);
		sprintf(buff, "T %d %d %s", joueur_courant, 
									getscore(part)[joueur_precedant-1],
									aff_map);
		free(aff_map);
		n=strlen(buff)+1;
		for(i=0; i<tab_stream.size(); ++i)
		{
			write(tab_stream[i], buff, n);
		}
		
		//écoute des gens
		FD_ZERO(&readfs);
		for(i=0; i<tab_stream.size(); ++i) //on écoutera aussi l'admin
		{
			FD_SET(tab_stream[i], &readfs);
		}
		
		printf("Attente de communication (réponse attendue de %d)\n", tab_stream[joueur_courant]);
		fflush(stdout);
		ret=select(max_fs+1, &readfs, NULL, NULL, NULL);
		if(ret<0)
		{
			printf("Erreur : seclect %s\n", msg_err);
			exit(-1);
		}
		
		//on regarde qui a parlé
		if(FD_ISSET(tab_stream[joueur_courant], &readfs))
		{
			//joueur courant a parlé
			printf("Joueur courant (%d) a parlé -->", tab_stream[i]);
			fflush(stdout);
			r=read(tab_stream[i], buff_read, 50);
			write(1, buff_read, r);
			printf("<-- (%d char)\n", r);
			fflush(stdout);
			//traitement
		}
		else
		{
			for(i=0; i<tab_stream.size(); ++i)
			{
				if(FD_ISSET(tab_stream[i], &readfs))
				{
					printf("   ERREUR : socket %d a parlé -->", tab_stream[i]);
					fflush(stdout);
					r=read(tab_stream[i], buff_read, 50);
					write(1, buff_read, r);
					printf("<-- (%d char)\n", r);
					fflush(stdout);
					print("   ==> communication ignorée\n");
				}
			}
		}
	}
	free(buff);
	
	
	pause();
}
	
