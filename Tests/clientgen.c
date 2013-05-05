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
		printf("   Erreur (serveur déconnecté ?) : %s\n", msg_err);
	}
	else
	{
		printf("   Recu  : -->");
		fflush(stdout);
		write(1, buff, r);
		printf("<-- (%d caractères)\n", r);fflush(stdout);
		if(buff[r-1]=='\0')
		{
			printf("   \'\\0\' à la fin OK\n");fflush(stdout);
		}
		else
		{
			printf("   char final : %d\n",(unsigned char) buff[r-1]);fflush(stdout);
		}
		fflush(stdout);
	}
} 

void writing(int sock, char* buff)
{
	buff+=2;
	int n = strlen(buff)+1;
	printf("   Sending to client -->");fflush(stdout);
	write(1, buff, n);fflush(stdout);
	printf("<--\n"); fflush(stdout);
	if(write(sock, buff, n)!=n)
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
	char ip_buff[16];
	unsigned int port_cible;
	struct sockaddr_in cible;
	struct sockaddr_in my_infos;
	unsigned int sizeof_my_infos=sizeof(my_infos);
	int sock;
	
	short cont;
	char buff[BUF_LEN];
	
	cible.sin_family=AF_INET;
	
	print("Client générique v1.0\n\n");
	
	//choix de l'ip du serveur cible
	cont=0; //pas d'ip correcte
	while(!(cont!=0))//tant que pas d'ip correcte
	{
		//demande d'une IP d'une IP
		print("Ip à contacter -> ");
		scanf("%15s", ip_buff);
		flush();
		//convertion en adresse
		cont=inet_aton(ip_buff, (struct in_addr*) &cible.sin_addr);
		if(cont==0)
		{ //ip incorrecte
			print("Ip entrée incorrecte, recommencez :\n");
		}
		else
		{  print("Ip correcte\n"); }
	}
	//demande d'un port de connexion
	print("Port de connexion -> ");
	scanf("%6ud", &port_cible);
	flush();
	cible.sin_port=htons(port_cible);
	
	//affichage
	print("\nConnection au serveur ");
	aff_info(&cible);
	print(" ...\n");
	
	//création du socket
	sock=socket(AF_INET, SOCK_STREAM, 0);
	if(sock==-1)
	{
		fprintf(stderr,"Erreur -> socket : %s\n", msg_err);
		exit(-1);
	}
	
	//connection du socket
	if(connect(sock, (struct sockaddr*) &cible, sizeof(cible))<0)
	{
		fprintf(stderr,"Erreur -> connect : %s\n", msg_err);
		exit(-1);
	}
	
	//affichage de mes infos de connexion
	getsockname(sock, (struct sockaddr*) &my_infos, &sizeof_my_infos);
	print("connecté !\n\n##########################\n\nMes infos de connection : ");
	aff_info(&my_infos);
	print("\n\n");
	print_man();
	print("\n\n");
	
	
	cont=1;
	while(cont!=0) //boucle principale
	{
		print("action -> ");
		fgets(buff, BUF_LEN-1, stdin); //récupération de la commande
		buff[strlen(buff)-1]='\0';
		//flush();
		switch(buff[0])
		{
			case 'q' : //fermeture du programme
				cont=0;
				break;
			case 'h' :
				print_man(); //affichage de l'aide
				break;
			case 'r' :
				reading(sock); //reception de données
				break;
			case 'w' :
				writing(sock, buff); //envoi du message suivant 'w '
				break;
			default :
				print("commande incorrecte :"); //Affichage du man si erreur
				print_man();
				break;
		}
	}
	print("Arrêt du client ...\n");
	
	return 0;
}
	
	
	
