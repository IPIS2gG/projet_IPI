#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <unistd.h> 
#include <arpa/inet.h>
#include <netinet/in.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <errno.h> 
#include <signal.h>
#include <sys/wait.h>

#include "openssl/sha.h"
#include "libpq-fe.h"

#define msg_err strerror(errno)
#define BUF_LEN 100

//// RESEAU
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

void writing(int sock, char* buff)
{
	buff+=2;
	printf("   Sending to client '%s' ...\n", buff);
	if(write(sock, buff, BUF_LEN-2)!=BUF_LEN-2)
	{
		print("   Erreur durant l'envoi des données ...\n");
	}
	else
	{
		print("   Données envoyées !\n");
	}
}


//// CRYPTAGE

// sha256 prend un mot et un buffer, et remplie le sha256 associé au mot dans le buffer.
void sha256(char* mot, char tampon[64]){
	// Initialisation
	unsigned char hash[SHA256_DIGEST_LENGTH];
	SHA256_CTX sha256;
	SHA256_Init(&sha256);
	// Hashage du mot
	SHA256_Update(&sha256, mot, strlen(mot));
	// Stockage du résultat (le hashé)
	SHA256_Final(hash, &sha256);
	int i=0;
	for(i=0;i<SHA256_DIGEST_LENGTH;i++){
		sprintf(tampon+(2*i), "%02x", hash[i]);
	}
}

//// BASE DE DONNEES

// exit_nicely quitte proprement le programme (on se déconnecte de la base de données)
static void exit_nicely(PGconn *conn){
	PQfinish(conn); exit(1);
}

// initDb crée la table users et insère le tuple admin.
void initDb(PGconn* conn){
	PGresult   *res;
	res=PQexec(conn,"CREATE TABLE users (name VARCHAR(32) PRIMARY KEY NOT NULL, hash VARCHAR(64) NOT NULL, type CHAR NOT NULL, CONSTRAINT dom_type CHECK (type IN('A','J','0')))");
	if(!res){
		fprintf(stderr,"fail CREATE TABLE");
	}
	res=PQexec(conn,"INSERT INTO users VALUES ('alexandre','967520ae23e8ee14888bae72809031b98398ae4a636773e18fff917d77679334','A')");
	printf("%s",PQcmdTuples(res));
}

// Vérifie que les identifiants sont corrects
void checkAuth(PGconn* conn, char name[32], char pass[32]){
	char* paramValues[1];
	paramValues[0]=name;
	PGresult *res;
	res=PQexecParams(conn,"SELECT hash,type FROM users WHERE name=$1",
		1,NULL,paramValues,NULL,NULL,1);
	if(PQresultStatus(res) != PGRES_TUPLES_OK){
		fprintf(stderr,"fail %s",PQerrorMessage(conn));
		PQclear(res); exit_nicely(conn);
	}
	if(PQntuples(res)==1){
		char hashTry[65];
		sha256(pass,hashTry);

		if(!strcmp(hashTry,PQgetvalue(res,0,0))){
			printf("%s\n",PQgetvalue(res,0,1));
		}else{
			printf("ko\n");
		}
	}else{
		printf("Cet utilisateur n'existe pas.");
	}
	PQclear(res);
}


void createUser(PGconn* conn, char name[32], char pass[32]){
	char* paramValues[2];
	paramValues[0]=name;
	char tampon[64];
	sha256(pass,tampon);
	paramValues[1]=tampon;


	if(PQresultStatus(PQexecParams(conn,"INSERT INTO users VALUES ($1,$2,'J')",
		2,NULL,paramValues,NULL,NULL,1)) != PGRES_COMMAND_OK){
		fprintf(stderr,"failoubet %s",PQerrorMessage(conn));

	}

}


//// FONCTION PRINCIPALE
int main(int argc, char const *argv[]){
	if(argc!=1){
		fprintf(stderr, "usage: %s\n",argv[0]); exit(1);
	}

	// Connexion à la base de données
	const char *conninfo;
	PGconn     *conn;
	conninfo="dbname = hashdb";
	conn = PQconnectdb(conninfo);
	if(PQstatus(conn) != CONNECTION_OK){
		fprintf(stderr,"db fail: %s", PQerrorMessage(conn)); exit_nicely(conn);
	}else{
		printf("Connecté OK.\n");
	}

	// La table users existe-t-elle ?
	if(PQresultStatus(PQexec(conn,"SELECT type FROM users LIMIT 1"))!=PGRES_TUPLES_OK){
		// On la crée et on initialise l'admin.
		initDb(conn);
		printf("La table users n'existait pas et vient d'être créée et initialisée.");
	}

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
	if(sock==-1){
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
					close_prgm=0;
					break;
				case 'c' : //fermeture du programme
					cont=0;
					break;
				case 'r' :
					reading(sock_stream); //reception de données
					break;
				case 'w' :
					writing(sock_stream, buff); //envoi du message suivant 'w '
					break;
				default :
					print("commande incorrecte");
					break;
			}
		}
		print("Deconnexion.");
		close(sock_stream);
	}
	print("Arret.");


/*
	// Réception d'un nom d'utilisateur et de son mot de passe
	char choix='E';
	char name[32];
	char pass[32];

	while(1){
		printf("Choix ? ");
		if(scanf("%c",&choix) ==-1){
			fprintf(stderr,"fail: scanf choix");	exit(1);
		}
		while(getchar()!='\n');

		if(choix=='A'){
			printf("Pseudo ? ");
			if(scanf("%s", name)==-1){
				fprintf(stderr,"fail: scanf1");	exit(1);
			}while(getchar()!='\n');
			printf("Mot de passe ? ");
			if(scanf("%s", pass)==-1){
				fprintf(stderr,"fail: scanf2"); exit(1);
			}while(getchar()!='\n');

			createUser(conn,name,pass);
		}else if(choix=='C'){
			printf("Pseudo ? ");
			if(scanf("%s", name)==-1){
				fprintf(stderr,"fail: scanf1");	exit(1);
			}while(getchar()!='\n');
			printf("Mot de passe ? ");
			if(scanf("%s", pass)==-1){
				fprintf(stderr,"fail: scanf2"); exit(1);
			}while(getchar()!='\n');

			checkAuth(conn,name,pass);
		}else{
			printf("Choix incorrect (%c). Entrez: A\n",choix);
		}

		choix='E';
	}
*/
	PQfinish(conn);
	return 0;
}
