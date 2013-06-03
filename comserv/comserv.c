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

struct param_thread_client //structure de paramètre pour un thread gérant une connexion client
{
	int sock_client_stream; //flux de transmition
	int sock_authserv;
	int* sock_admin;
	bool* connect_ok;
	int id; //id dans la table des threads
	std::map<int, pthread_t>* tab_thread; //table des threads
	std::map<int, char*>* tab_comm;
	std::set<int>* tab_stream;
	struct sockaddr_in client; //client avec lequel on cause
	struct param_partie* info_partie;
};

struct param_thread_authserv
{//structure de paramètre pour le thread gérant la connexion avec authserv
	int sock_authserv;
	int* sock_admin;
	std::map<int, pthread_t>* tab_thread; //table des threads
	std::map<int, char*>* tab_comm;
	std::set<int>* tab_stream;
};

struct param_thread_accept_connect
{
	int sock_client;
	int sock_authserv;
	int* sock_admin;
	struct param_partie* info_partie;
	std::set<int>* tab_stream;
	std::map<int, pthread_t>* tab_thread; //table des threads
	std::map<int, char*>* tab_comm;
	pthread_t* thread_auth;
};

void* thread_accept_connection(void*);
	//fonction d'acceptation des connexion
void* thread_connection_client(void *);
	//fonction de gestion d'une connection client (lancée dans un thread)
void* thread_connection_authserv(void *);
	//fonction de gestion de la connextion authserv (lancée dans un thread)
void aff_thread_info(int id, struct sockaddr_in* client);
	//Affichage des infos d'un thread (id dans la table, client relié)
		
pthread_mutex_t lock_connect; //bloquera l'entrée dans la partie -> chacun son tour

//###############################################
//############### Fonction Main #################
//###############################################

int main(int argc, char** argv)
{
	//gestion clients
	int sock_client;
	unsigned int mon_port; //mes infos de connection pour attendre clients
	struct sockaddr_in mes_infos; //de connection au clients
	
	//gestion serveur auth.
	struct sockaddr_in mes_infos_authserv; //infos coté serveur d'authentification
	unsigned int sizeof_mes_infos_authserv=sizeof(mes_infos_authserv);
	struct sockaddr_in authserv; //info récupérés de accept, qu'on transmettra au thread
	int sock_authserv;
	
	char ip_buff[16]; //choix des infos de connexion
	unsigned int port_cible_authserv;
	
	//mémoire partagée
	std::map<int, pthread_t> tab_thread; //table des thread ouverts
		//utilisé pour authserv et les clients
	std::map<int, char*> tab_comm; //map de communication (chaque thread à une "boite au lettres")
		//idem
	std::set<int> tab_stream;
		//ensembles des stream ouverts à fermer une fois la partie commencée
	int sock_admin=-1; //variable partagée
		//socket de l'admin -> authserv en a besoin
	
	//vars de création des threads
	pthread_t thread_auth; //qui gère l'écoute de authserv
	pthread_t thread_accept;
	struct param_thread_authserv* param_authserv;
	struct param_thread_accept_connect* param_accept;

	struct param_partie info_partie; //informations pour débuter la partie
	std::set<int>::iterator it_stream;
	std::map<int, pthread_t>::iterator it_thread;
	unsigned int i;
	
	short cont;
	
	//###### Début du programme ######
	
	print("Serveur de jeu IPI 2013\n\n");
	
	//gestion des signaux
	signal(SIGUSR1, do_nothing);
	signal(SIGUSR2, do_nothing);
	
	//init mutex
	if(pthread_mutex_init(&lock_connect, NULL)!=0)
	{
		fprintf(stderr, "Erreur d'init d'un mutex\n");
		exit(-1);
	}
		
	//#### connection à authserv
	
	print("Informations de connection au serveur d'authentification\n\n");
	
	authserv.sin_family=AF_INET;
	
	//choix de l'ip du serveur cible
	cont=0; //pas d'ip correcte
	while(!(cont!=0))//tant que pas d'ip correcte
	{
		//demande d'une IP d'une IP
		print("Ip à contacter -> ");
		scanf("%15s", ip_buff);
		flush();
		//convertion en adresse
		cont=inet_aton(ip_buff, (struct in_addr*) &authserv.sin_addr);
		if(cont==0)
		{ //ip incorrecte
			print("Ip entrée incorrecte, recommencez :\n");
		}
		else
		{  print("Ip correcte\n"); }
	}
	
	print("Port de connexion -> "); //récupération du port d'écoute du serveur depuis stdin
	scanf("%6ud", &port_cible_authserv);
	flush();
	authserv.sin_port=htons(port_cible_authserv);
	
	print("Initialisation de la connexion ...\n");
	
	//création du socket
	sock_authserv=socket(AF_INET, SOCK_STREAM, 0);
	if(sock_authserv==-1)
	{
		fprintf(stderr,"Erreur -> socket : %s\n", msg_err);
		exit(-1);
	}
	
	printf("Connection à authserv ");
	aff_info(&authserv);
	print(" ...\n");
	
	//connection au serveur
	if(connect(sock_authserv, (struct sockaddr*) &authserv, sizeof(authserv))<0)
	{
		fprintf(stderr,"Erreur -> connect : %s\n", msg_err);
		exit(-1);
	}
	
	getsockname(sock_authserv, (struct sockaddr*) &mes_infos_authserv, &sizeof_mes_infos_authserv);
	print("connecté ");
	aff_info(&mes_infos_authserv);
	print(" !\n");
	
	//ouverture du thread d'écoute de authserv
	param_authserv=(struct param_thread_authserv*) calloc(1, sizeof(struct param_thread_authserv));
	param_authserv->sock_authserv=sock_authserv; //socket d'écoute
	param_authserv->tab_thread=&tab_thread; //map des threads client pour transmettre infos
	param_authserv->tab_comm=&tab_comm; //boite aux lettres des clients
	param_authserv->sock_admin=&sock_admin; //le stream de l'admin
	if(pthread_create(&thread_auth, NULL, thread_connection_authserv, param_authserv)!=0)
	{
		fprintf(stderr,"Création d'un thread échouée : %s\n", msg_err);
		exit(-1); //on ne peut pas continuer
	}
	
	//ouverture du serveur (initialisation)
	print("\nInformation d'ouverture du serveur :\n\n");
	
	//init de mes infos
	mes_infos.sin_family=AF_INET;
	mes_infos.sin_addr.s_addr=INADDR_ANY;
	
	print("Initialisation ...\n");
	
	//création du socket
	sock_client=socket(AF_INET, SOCK_STREAM, 0);
	if(sock_client==-1)
	{
		fprintf(stderr,"Erreur -> socket : %s\n", msg_err);
		exit(-1);
	}

	cont=0;
	while(cont==0)
	{
		//demande d'un port de connexion
		print("Port d'écoute -> "); //récupération du port d'écoute du serveur depuis stdin
		scanf("%6ud", &mon_port);
		flush();
		mes_infos.sin_port=htons(mon_port);
		
		printf("Ouverture du port %d ...\n",ntohs(mes_infos.sin_port));
		fflush(stdout);
		cont=1;
		//bind du socket
		if(bind(sock_client, (struct sockaddr*) &mes_infos, sizeof(mes_infos))<0)
		{
			fprintf(stderr,"Erreur -> bind : %s\n", msg_err);
			cont=1;
		}
	}
	
	//paramétrage de l'écoute
	if(listen(sock_client, 1)!=0)
	{
		fprintf(stderr,"Erreur -> listen : %s\n", msg_err);
		exit(-1);
	}
	
	//terminé
	printf("Serveur prêt à écouter sur le port %d !\n\n",
				ntohs(mes_infos.sin_port));
	
	//création du thread d'écoute des connections
	param_accept=(struct param_thread_accept_connect*) calloc(1, 
					sizeof(struct param_thread_accept_connect));
	param_accept->sock_client=sock_client; //socket de connexion des clients
	param_accept->sock_authserv=sock_authserv; //socket du serveur d'authentification (demande d'auth)
	param_accept->tab_stream=&tab_stream; //ensembles des connexions ouvertes
	param_accept->info_partie=&info_partie; //infos de la partie (changée par l'admin, gardée ensuite)
	param_accept->tab_thread=&tab_thread; //map des threads clients (pour en déclarer)
	param_accept->tab_comm=&tab_comm; //boite aux lettres (pour récupérer infos)
	param_accept->sock_admin=&sock_admin; //stream de l'admin
	if(pthread_create(&thread_accept, NULL, thread_accept_connection, param_accept)!=0)
	{
		fprintf(stderr,"Création d'un thread échouée : %s\n", msg_err);
		exit(-1); //on ne peut pas continuer
	}
	
	print("Thread principal : attente du début de la partie ....\n");
	pause();
	print("Thread printicpal reprend la main\n");
	print("Fermeture des connection ouvertes inutiles\n");
	for(it_stream=tab_stream.begin(); it_stream!=tab_stream.end(); ++it_stream)
	{
		printf("   fermeture socket %d\n", *it_stream);
		close(*it_stream);
	}
	print("Destruction des threads\n");
	for(it_thread=tab_thread.begin(); it_thread!=tab_thread.end(); ++it_thread)
	{
		printf("   fermeture thread d'ID %d ...\n", it_thread->first);
		pthread_cancel(it_thread->second);
	}
	print("   fermeture thread écoute clients\n");
	pthread_cancel(thread_accept);
	print("   fermeture thread écoute de authserv\n");
	pthread_cancel(thread_auth);
	print("   destruction mutex\n");
	pthread_mutex_destroy(&lock_connect);
	
	launch_game(&info_partie); //lancement du jeu avec les infos
	
	//fermeture de tous les stream_restant
	for(i=0; i<info_partie.tab_stream.size(); ++i)
	{
		close(info_partie.tab_stream[i]);
	}
	print("Serveur shutting down ...\n");
	return EXIT_SUCCESS;
}

// ####################################################################

void* thread_accept_connection(void* param)
{
	struct param_thread_accept_connect* info=(struct param_thread_accept_connect*) param;
	//recopie de toutes les infos
	int sock_client=info->sock_client;
	int sock_authserv=info->sock_authserv;
	int* sock_admin=info->sock_admin;
	std::set<int>* tab_stream=info->tab_stream;
	std::map<int, pthread_t>* tab_thread=info->tab_thread; //table des threads
	std::map<int, char*>* tab_comm=info->tab_comm;
	struct param_partie* info_partie=info->info_partie;
	
	//gestion des clients
	struct sockaddr_in client; //info récupérés de accept, qu'on transmettra au thread
	unsigned int sizeof_client=sizeof(client);
	
	//gestion des threads
	int id=0; //clé dans la map
	struct param_thread_client* param_client; //param du thread
	
	bool connect_ok=false; //variable qui autorise ou non les connection (admin a créé la partie ?)
	short cont;
	
	int sock_client_stream;
	
	cont=1;
	while(cont!=0) //boucle d'acceptation des connections
	{
		print("Attente de connexion ...\n");
		sock_client_stream=accept(sock_client, (struct sockaddr*) &client, &sizeof_client);
		printf("Connexion détectée => stream[%d]\n", sock_client_stream);fflush(stdout);
		if(sock_client_stream<0)
		{
			fprintf(stderr,"La connexion a échoué : %s\n", msg_err);
		}
		else
		{
			if(id>99)
			{
				fprintf(stderr,"Nombre limite de connexion atteint\n");
			}
			else
			{
				//on créé le param du thread quon s'apprète à créer
				param_client= (struct param_thread_client*) calloc(1,
												sizeof(struct param_thread_client));
				param_client->id=id;
				param_client->sock_client_stream=sock_client_stream;
				param_client->sock_authserv=sock_authserv;
				param_client->sock_admin=sock_admin; //pointeur
				param_client->tab_thread=tab_thread; //pointeur
				param_client->tab_comm=tab_comm; //pointeur
				param_client->tab_stream=tab_stream; //pointeur
				param_client->client=client; //recopie du client
				param_client->info_partie=info_partie; //pointeur
				param_client->connect_ok=&connect_ok;

				(*tab_thread)[id];
					//ajout d'un thread dans la table (on créé la case, g++ créé le contenu)
		
				//création d'un nouveau thread
				if(pthread_create(&(*tab_thread)[id], NULL, thread_connection_client, param_client)!=0)
				{
					fprintf(stderr,"Création d'un thread échouée : %s\n", msg_err);
					tab_thread->erase(id); //destruction du thread;
					free(param_client); //on détruit le paramètre
				}
				else
				{
					print("Connexion OK\n");
					id++;
				}
			}
		}
	}
	
	return NULL;
}

// ########################################################

void* thread_connection_client(void * param)
{
	struct param_thread_client* infos; //cast du param
	infos= (struct param_thread_client*) param;
	
	//copie "inutile", mais plus agréable à lire (et peu de données)
	int id=infos->id;
	int sock_client_stream=infos->sock_client_stream;
	struct sockaddr_in client=infos->client;
	
	int sock_authserv=infos->sock_authserv;
	int* sock_admin=infos->sock_admin;
	bool* connect_ok=infos->connect_ok;
	
	std::map<int, pthread_t>* tab_thread=infos->tab_thread;
	std::map<int, char*>* tab_comm=infos->tab_comm;
	std::set<int>* tab_stream=infos->tab_stream;
	
	struct param_partie* info_partie=infos->info_partie;
	
	std::map<int, pthread_t>::iterator it_thread;
	std::map<int, char*>::iterator it_comm;
	
	int r; //nbr char reçus
	int n; // strlen(buff_write)
	char buff_read[50]; //buffer de lecture/écriture
	char buff_write[50];
	char type_rep; //type de réponse
	char char_rep; //caractère dans la réponse
	int int_rep; //entier dans une réponse
	int w,h,nb_coups,nb_max; //envoi de l'admin
	char pseudo[17]; //chaine du pseudo de l'utilisateur au bout du fil
	char mdp[17];
	char* rep_comm=NULL; //chaine dans la boite au lettres (tab_comm)
	char* buff_copy=NULL; //recopie d'un contenu dans tab_comm (par l'admin)
	
	bool admin=false; //est-ce un thread d'admin ?
	
	bool cont;
	
	free(param); //destruction du paramètre devenu inutile
	
	//on ajoute ce stream au set des stream à fermer au lancement de la partie
	tab_stream->insert(sock_client_stream);
	
	//affichage de mes infos
	print("   Nouvelle connexion ");
	aff_thread_info(id, &client);
	print("\n"); fflush(stdout);
	
	//###### CONNEXION ###############
	//on attends sa demande de connexion
	r=read(sock_client_stream, buff_read, 50);
	if(r<=0)
	{
		printf("   Connexion avec le client perdue : %s", msg_err);
		aff_thread_info(id, &client);
		print(" \n");
		close(sock_client_stream);
		tab_thread->erase(id);
		tab_stream->erase(sock_client_stream);
		return NULL;
	}
	//affichage reception
	print("   ");
	aff_thread_info(id, &client);
	print(" reçoit -->");
	write(1, buff_read, r);
	printf("<-- (%d char.) \n", r);
	//traitement protocole
	type_rep=buff_read[0];
	if(type_rep!='C')
	{
		print("   ");
		aff_thread_info(id, &client);
		print(" Erreur protocole [C pseudo(16) mdp(16)]\n");
		close(sock_client_stream);
		tab_thread->erase(id);
		tab_stream->erase(sock_client_stream);
		return NULL;
	}
	//extraction informations
	sscanf(buff_read, "%*c %16s %16s", pseudo, mdp);
	print("   ");
	aff_thread_info(id, &client);
	printf(" means type=%c pseudo='%s' mdp='%s'\n", type_rep, pseudo, mdp);
	fflush(stdout);
	//création de l'envoi à authserv
	sprintf(buff_write, "%c %d %s %s", type_rep, id, pseudo, mdp);
	n=strlen(buff_write)+1;
	//affichage des données envoyées
	print("   ");
	aff_thread_info(id, &client);
	print(" envoi à authserv -->");
	write(1, buff_write, n);
	printf("<-- (%d char.) \n", n);
	//envoi proporement dit
	if(write(sock_authserv, buff_write, n)!=n)
	{
		print("   ");
		aff_thread_info(id, &client);
		printf(" Erreur write : %s\n", msg_err);
		close(sock_client_stream);
		tab_thread->erase(id);
		tab_stream->erase(sock_client_stream);
		return NULL;
	}
	print("   ");
	aff_thread_info(id, &client);
	print(" -> données en envoyées !\n");
	//######## ATTENTE REPONSE AUTHSERV
	print("   ");
	aff_thread_info(id, &client);
	print(" -> attente de réponse ...\n");
	pause(); //on attends de recevoir un signal (SIGUSR1) pour regarder dans notre buff la réponse
	//normalement, réponse de athserv dans tab_comm[id]
	print("   ");
	aff_thread_info(id, &client);
	print(" -> réveillé !\n");
	it_comm=tab_comm->find(id); //recherche du message
	if(it_comm==tab_comm->end())
	{
		//non trouvé -> erreur
		print("   ");
		aff_thread_info(id, &client);
		printf(" Erreur communication interne : réponse authserv non trouvée\n");
		close(sock_client_stream);
		tab_thread->erase(id);
		tab_stream->erase(sock_client_stream);
		return NULL;
	}
	rep_comm=it_comm->second;
	tab_comm->erase(id); //on supprime ma boite de reception (j'ai l'adresse qu'elle contenait)
	r=strlen(rep_comm)+1;
	//affichage reception
	print("   ");
	aff_thread_info(id, &client);
	print(" réponse authserv (transmis) -->");
	write(1, rep_comm, r);
	printf("<-- (%d char.) \n", r);
	//extraction infos
	sscanf(rep_comm, "%c %*d %c", &type_rep, &char_rep);
	free(rep_comm);
	rep_comm=NULL;
	print("   ");
	aff_thread_info(id, &client);
	printf(" means type=%c rep='%c'\n", type_rep, char_rep);
	if(type_rep!='c' || (char_rep!='0' && char_rep!='A' && char_rep!='J'))
	{
		print("   ");
		aff_thread_info(id, &client);
		print(" Erreur protocole [c ID(2) (0|A|J)]\n");
		close(sock_client_stream);
		tab_thread->erase(id);
		tab_stream->erase(sock_client_stream);
		return NULL;
	}
	if(char_rep=='J' && !*connect_ok)
	{
		//joueur, pas d'admin
		char_rep='W'; //on doit dire au client de repasser
		print("   ");
		aff_thread_info(id, &client);
		print(" pas d'admin -> W\n");
	}
	if(char_rep=='A')
	{
		//on est admin !!!
		admin=true;
		print("   ");
		aff_thread_info(id, &client);
		print(" est maintenant admin !\n");
	}
	
	//réponse au client
	sprintf(buff_write, "%c %c", type_rep, char_rep);
	write(sock_client_stream, buff_write, strlen(buff_write)+1); //envoi de la rep au client
	print("   ");
	aff_thread_info(id, &client);
	print(" réponse au client envoyée\n");
	
	if(char_rep=='0') //mes id étaient incorrect =>fini pour moi
	{
		print("   Fin de connexion ");
		aff_thread_info(id, &client);
		print("\n");
	
		close(sock_client_stream); //fermeture de la connexion
		tab_thread->erase(id); //on s'enlève de la table des threads
		tab_stream->erase(sock_client_stream);
	
		return NULL;
	}
	
	//AUTHENTIFICATION OK -> ADMIN ou ATTENTE REP ADMIN
	
	if(char_rep=='W')
	{
		//le client devra se reconnecter => fin de cette connexion
		print("   Fin de connexion ");
		aff_thread_info(id, &client);
		close(sock_client_stream); //fermeture de la connexion
		tab_thread->erase(id); //on s'enlève de la table des threads
		tab_stream->erase(sock_client_stream);
		return NULL;
	}
	
	if(admin) //si je suis admin
	{
		//#### ADMIN ##############
		
		*sock_admin=sock_client_stream; //admin paré à l'acceptation de connections

		//ajout de l'admin dans la partie
		info_partie->tab_stream.push_back(sock_client_stream);
		rep_comm = (char*) calloc(16, sizeof(char)); //rep_comm => char*
		strcpy(rep_comm, pseudo);
		info_partie->tab_pseudo.push_back(rep_comm);
		//on ne doit plus fermer ce stream au lancement de la partie
		tab_stream->erase(sock_client_stream);
		
		// ### première étape, administration de la partie
		cont = true;
		while(cont)
		{
			r=read(sock_client_stream, buff_read, 50); //on attends que l'admin parle
			if(r<=0)
			{
				fprintf(stderr, "Connexion à l'admin perdue, le programme doit fermer\n");
				exit(-1);
			}
			printf("   Reçu de admin : -->");fflush(stdout);
			write(1, buff_read, r);
			printf("<-- (%d caract.)\n", r);
			switch(buff_read[0])
			{
				case 'A' ://Création nouvel user
					//copy bète et mechante à authserv
					print("   Admin : copié à authserv\n");
					write(sock_authserv, buff_read, r);
					break;
				case 'N' :
					//extraction des infos
					sscanf(buff_read, "%*c %2d %2d %4d %2d", &w, &h,&nb_coups, &nb_max); //lecture de la réponse
					printf("   Admin means : type:N w='%d' h='%d' nb_max='%d'\n",
													w,h,nb_max);
					fflush(stdout);
					info_partie->w=w;
					info_partie->h=h;
					info_partie->nb_coups=nb_coups;
					info_partie->nb_max=nb_max;
					print("   Admin : partie créée\n");
					cont=false;
					break;
				default :
					fprintf(stderr, "   Admin : erreur protocole\n");
					fflush(stderr);
					break;
			}
		}
		
		print("   Admin : les joueurs peuvent se connecter\n");
		*connect_ok=true;
		
		while(true)
		{
			r=read(sock_client_stream, buff_read, 50); //on attends que l'admin parle
			if(r<=0)
			{
				fprintf(stderr, "Connexion à l'admin perdue, le programme doit fermer\n");
				exit(-1);
			}
			printf("   Reçu de admin : -->");fflush(stdout);
			write(1, buff_read, r);
			printf("<-- (%d caract.)\n", r);
			switch(buff_read[0])
			{
				case 'p' :
					//extraction des infos
					sscanf(buff_read, "%*c %2d %1c", &int_rep, &char_rep); //lecture de la réponse
					printf("   Admin means : type:p id='%d' rep='%c'\n",
													int_rep, char_rep);
					fflush(stdout);
					//on place la réponse dans la map de comm
					buff_copy = (char*) calloc(r, sizeof(char)); 
					strncpy(buff_copy, buff_read, r);
					(*tab_comm)[int_rep]=buff_copy;
					//et on envoi un signal pour réveiller le thread correspondant
					it_thread=tab_thread->find(int_rep);
					if(it_thread==tab_thread->end())
					{
						fprintf(stderr, "   Admin : erreur protocole : id thread inconnu\n");
						fflush(stderr);
						free(buff_copy);
					}
					else
					{
						print("   Admin : thread id ok\n");
						pthread_kill(it_thread->second, SIGUSR1);
						print("   Admin : réponse transmise\n");
					}
					break;
				case 'S' :
					kill(getpid(), SIGUSR2); //on prévient le thread principal
					pause(); //plus rien à faire ici, on va de toute manière être tué
					break;
				default :
					fprintf(stderr, "   Admin : erreur protocole\n");
					fflush(stderr);
					break;
			}
		}
			
	}
	else //### JOUEUR
	{
		//#### CONFIRMATION ADMIN #########
		
		if((int) info_partie->tab_stream.size() >= info_partie->nb_max+1) 
		{	//il n'y a déjà plus de place dans la partie
			//on envoi directement au client le refus de l'admin
			print("   ");
			aff_thread_info(id, &client);
			print(" -> partie complète : envoi refus admin\n");
			print("   ");
			aff_thread_info(id, &client);
			print(" fin de connexion\n");
			write(sock_client_stream, "J 0", 4);
			close(sock_client_stream);
			tab_thread->erase(id);
			tab_stream->erase(sock_client_stream);
			return NULL;
		}			
		
		//demande d'acceptation à l'admin
		sprintf(buff_write, "P %d %s", id, pseudo);
		print("   ");
		aff_thread_info(id, &client);
		print(" -> envoi demande confirm admin ...\n");
		write(*sock_admin, buff_write, strlen(buff_write)+1);
		print("   ");
		aff_thread_info(id, &client);
		print(" -> attente confirm admin ...\n");
		pause(); //on attends ici une réponse de l'admin
		print("   ");
		aff_thread_info(id, &client);
		print(" -> réveillé !\n");
		it_comm=tab_comm->find(id);
		if(it_comm==tab_comm->end())
		{
			//non trouvé -> erreur
			print("   ");
			aff_thread_info(id, &client);
			printf(" Erreur communication interne : réponse admin non trouvée\n");
			close(sock_client_stream);
			tab_thread->erase(id);
			tab_stream->erase(sock_client_stream);
			return NULL;
		}
		rep_comm=it_comm->second;
		r=strlen(rep_comm)+1;
		//affichage reception
		print("   ");
		aff_thread_info(id, &client);
		print(" réponse admin (transmis) -->");
		write(1, rep_comm, r);
		printf("<-- (%d char.) \n", r);
		//extraction infos
		sscanf(rep_comm, "%c %*d %c", &type_rep, &char_rep);
		free(rep_comm);
		rep_comm=NULL;
		if(type_rep!='p' || (char_rep!='0' && char_rep!='1'))
		{
			print("   ");
			aff_thread_info(id, &client);
			print(" Erreur protocole [p ID(2) (0|1)]\n");
			close(sock_client_stream);
			tab_thread->erase(id);
			tab_stream->erase(sock_client_stream);
			return NULL;
		}
		//réponse au client
		print("   ");
		aff_thread_info(id, &client);
		printf(" means type=%c rep='%c'\n", type_rep, char_rep);
		
		if(char_rep=='1') //Accepté
		{
			//### gestion de l'entrée dans la partie
			pthread_mutex_lock(&lock_connect); //chacun son tour
				// -> évite de possible double entrée pour une place
				
			if((int) info_partie->tab_stream.size() < info_partie->nb_max+1) 
			{							//+1 pour l'admin qui a aussi des infos dedans
				//il reste une place
				//on accepte le joueur
				write(sock_client_stream, "J 1", 4); //envoi au client
				print("   ");
				aff_thread_info(id, &client);
				print(" joueur accepté dans la partie\n");
				
				//ajout du joueur dans la partie
				info_partie->tab_stream.push_back(sock_client_stream);
				rep_comm = (char*) calloc(16, sizeof(char)); //rep_comm => char*
				strcpy(rep_comm, pseudo);
				info_partie->tab_pseudo.push_back(rep_comm);
				//on ne doit plus fermer ce stream au lancement de la partie
				tab_stream->erase(sock_client_stream);
				pthread_mutex_unlock(&lock_connect);//on libère ce morceaux de code
				return NULL; //on en a terminé avec ce joueur
			}
		
			pthread_mutex_unlock(&lock_connect);
		}
		//joueur refusé, soit parce que refusé, soit plus de place
		write(sock_client_stream, "J 0", 4); //envoi au client
		print("   ");
		aff_thread_info(id, &client);
		print(" joueur refusé dans la partie\n");
	}
	
	print("   Fin de connexion ");
	aff_thread_info(id, &client);
	
	close(sock_client_stream); //fermeture de la connexion
	tab_thread->erase(id); //on s'enlève de la table des threads
	tab_stream->erase(sock_client_stream);
	
	return NULL;
}

//################### AUTHSERV #################################

void* thread_connection_authserv(void * param)
{
	struct param_thread_authserv* infos;
	infos= (struct param_thread_authserv*) param;
	int sock_authserv=infos->sock_authserv;
	int* sock_admin=infos->sock_admin;
	std::map<int, pthread_t>* tab_thread=infos->tab_thread;
	std::map<int, pthread_t>::iterator it_thread;
	
	std::map<int, char*>* tab_comm=infos->tab_comm;
	short cont=1;
	
	char buff_read[50];
	//char type_rep; //type de réponse
	int int_rep; //entier pour contenu les infos de la réponse
	//char buff_rep[30]; //contiendra une sous chaine de la réponse
	char char_rep; //contiendra un caractère de la réponse
	char* buff_copy;
	int r;
	
	while(cont!=0)
	{
		r=read(sock_authserv, buff_read, 50);
		if(r<=0)
		{
			fprintf(stderr, "Connexion à authserv perdue, le programme doit fermer\n");
			exit(-1);
		}
		printf("      Reçu de authserv : -->");fflush(stdout);
		write(1, buff_read, r);
		printf("<-- (%d caract.)\n", r);
		
		switch(buff_read[0])
		{
			case 'c' :
				//réponse à une demande de connexion
				sscanf(buff_read, "%*c %2d %1c", &int_rep, &char_rep); //lecture de la réponse
				printf("      Authserv (format protocole) : type:c id='%d' rep='%c'\n", int_rep, char_rep);
				fflush(stdout);
				//on place la réponse dans la map de comm
				buff_copy = (char*) calloc(r, sizeof(char)); 
				strncpy(buff_copy, buff_read, r);
				(*tab_comm)[int_rep]=buff_copy;
				//et on envoi un signal pour réveiller le thread correspondant
				it_thread=tab_thread->find(int_rep);
				if(it_thread==tab_thread->end())
				{
					fprintf(stderr, "      Authserv : erreur protocole : id thread inconnu\n");
					fflush(stderr);
					free(buff_copy);
				}
				else
				{
					print("      Authserv : thread id ok\n");
					pthread_kill(it_thread->second, SIGUSR1);
					print("      Authserv : réponse transmise\n");
				}
				break;
			case 'a' :
				if(*sock_admin<0)
				{
					print("      Authserv : erreur => pas d'amin !!\n");
				}
				else
				{
					//copie bête et méchante
					write(*sock_admin, buff_read, r);
					print("      Authserv => envoyé à l'admin\n");
				}
				break;
			default :
				fprintf(stderr, "      Authserv : erreur protocole\n");
				fflush(stderr);
				break;
		}
	}
	return NULL;
}

void aff_thread_info(int id, struct sockaddr_in* client)
{
	printf("Thread : id=%d client=", id);
	aff_info(client);
}
	
	
	
	
