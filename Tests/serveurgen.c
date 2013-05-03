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

#define msg_err strerror(errno)
#define BUF_LEN 100

void print(char *s)
{	printf("%s", s); fflush(stdout); }

void flush()
{ int c;
  while ((c = getchar ()) != '\n' && c != EOF);}
  
void aff_info(struct sockaddr_in* cible)
{
	printf("[%s::%d]",
		inet_ntoa((struct in_addr) cible->sin_addr),
		ntohs(cible->sin_port)
		);
	fflush(stdout);
}

void print_man()
{
	print(
"   Commandes :\n\
     r -> attente de réception de données (taille 50 max)\n\
     w message -> envoi du message\n\
     h -> affiche cette aide\n\
     c -> ferme la connexion\n\
     q -> ferme le programme\n");
}

void reading(int sock)
{
	char buff[BUF_LEN];
	int r;
	print("   Waiting for incomming data ...\n");
	r=read(sock, buff, BUF_LEN);
	if(r<=0)
	{
		printf("   Erreur (client déconnecté ?) : %s\n", msg_err);
	}
	else
	{
		buff[r]='\0';
		printf("   Recu  : '%s'\n", buff);
		fflush(stdout);
	}
}

void writing(int sock, char* buff, int nb_car)
{
	buff+=2;
	printf("   Sending to client '%s' ...\n", buff);
	if(write(sock, buff, nb_car)!=nb_car)
	{
		print("   Erreur durant l'envoi des données ...\n");
	}
	else
	{
		print("   Données envoyées !\n");
	}
}

//######################################################"

int main(int argc, char** argv)
{
	unsigned int mon_port;
	struct sockaddr_in mes_infos;
	struct sockaddr_in client;
	unsigned int sizeof_client=sizeof(client);
	int sock;
	int sock_stream;
	
	short cont;
	short close_prgm;
	char buff[BUF_LEN];
	
	mes_infos.sin_family=AF_INET;
	mes_infos.sin_addr.s_addr=INADDR_ANY;
	print("Serveur générique v1.0\n\n");
	
	//demande d'un port de connexion
	print("Port d'écoute -> ");
	scanf("%6ud", &mon_port);
	flush();
	mes_infos.sin_port=htons(mon_port);
	
	print("Initialisation et ouverture du port ...\n");
	//création du socket
	sock=socket(AF_INET, SOCK_STREAM, 0);
	if(sock==-1)
	{
		fprintf(stderr,"Erreur -> socket : %s\n", msg_err);
		exit(-1);
	}
	
	//connection du socket
	if(bind(sock, (struct sockaddr*) &mes_infos, sizeof(mes_infos))<0)
	{
		fprintf(stderr,"Erreur -> bind : %s\n", msg_err);
		exit(-1);
	}
	
	if(listen(sock, 1)!=0)
	{
		fprintf(stderr,"Erreur -> listen : %s\n", msg_err);
		exit(-1);
	}
	
	print("Serveur prêt à écouter !\n\n#################\n\n");
	print_man();
	print("\n#################\n\n");
	
	close_prgm=0;
	while(close_prgm==0)
	{
		//affichage de mmon port d'attente
		printf("Attente de connexion sur le port %d ...\n",
				ntohs(mes_infos.sin_port));
		sock_stream=accept(sock, (struct sockaddr*) &client, &sizeof_client);
		if(sock_stream<0)
		{
			fprintf(stderr,"Erreur -> accept : %s\n", msg_err);
			exit(-1);
		}
		
		print("Connexion acceptée ");
		aff_info(&client);
		print(" !\n");
		
		cont=1;
		while(cont!=0) //boucle d'interaction pour cette connexion
		{
			print("action -> ");
			fgets(buff, BUF_LEN-1, stdin); //récupération de la commande
			buff[strlen(buff)-1]='\0';
			//flush();
			switch(buff[0])
			{
				case 'q' : //fermeture du programme
					cont=0;
					close_prgm=1;
					break;
				case 'c' : //fermeture du programme
					cont=0;
					break;
				case 'h' :
					print_man(); //affichage de l'aide
					break;
				case 'r' :
					reading(sock_stream); //reception de données
					break;
				case 'w' :
					writing(sock_stream, buff, strlen(buff)); //envoi du message suivant 'w '
					break;
				default :
					print("commande incorrecte :"); //Affichage du man si erreur
					print_man();
					break;
			}
		}
		print("Fermeture de la connection avec le client ...\n\n");
		close(sock_stream);
	}
	print("Arrêt du serveur ...\n");
	
	return 0;
}
	
	
	
