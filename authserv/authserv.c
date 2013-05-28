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

//// [ CRYPTAGE ]

// sha256 crypte un mot en SHA256
// entrée: mot (à crypter) et tampon (où stocker le hash)
void sha256(char* sel, char* mot, char tampon[64]){
	printf("FONCTION SHA256\nSEL %s\nMOT %s\n",sel,mot);

	char toHash[64];
	strcpy(toHash,mot);
	
	// On ajoute un sel pour éviter les attaques de type RAINBOW TABLES
	// Le sel sera composé de 3 lettres du pseudo
	strncat(toHash,sel,3);

	// Initialisation
	unsigned char hash[SHA256_DIGEST_LENGTH];
	SHA256_CTX sha256;
	SHA256_Init(&sha256);
	// Hashage du mot
	SHA256_Update(&sha256, toHash, strlen(toHash));
	// Stockage du résultat (le hashé)
	SHA256_Final(hash, &sha256);

	int i=0;
	for(i=0;i<SHA256_DIGEST_LENGTH;i++){
		// Insertion dans le tampon
		sprintf(tampon+(2*i), "%02x", hash[i]);
	}

	printf("HASHE : %s",tampon);
}

//// [ BASE DE DONNEES ]

// initDb initialise la base de données (crée la table users et insère l'admin)
void initDb(PGconn* conn){
	PGresult   *res;

	// Création de la table (et de ses contraintes)
	res=PQexec(conn,"CREATE TABLE users (name VARCHAR(32) PRIMARY KEY NOT NULL, hash VARCHAR(64) NOT NULL, type CHAR NOT NULL, CONSTRAINT dom_type CHECK (type IN('A','J','0')))");
	if(!res){
		fprintf(stderr,"fail CREATE TABLE");
	}

	// Insertion du tuple admin (admin/motdepasse)
	// (L'énoncé présuppose qu'il n'y a qu'un admin)
	res=PQexec(conn,"INSERT INTO users VALUES ('admin','9dcbf36f844d90127640c889ca77d9fe12b9b3a883f056ef3960b1b022c74e69','A')");

	// Affichage du résultat 
	printf("%s",PQcmdTuples(res));
}

// Vérifie que les identifiants sont corrects
char checkAuth(PGconn* conn, char name[32], char pass[32]){
	char* paramValues[1];
	paramValues[0]=name;

	PGresult *res;

	// Récupération du hash et du type (admin ou user) de l'utilisateur demandé (name)
	res=PQexecParams(conn,"SELECT hash,type FROM users WHERE name=$1",
		1,NULL,(const char* const*) paramValues,NULL,NULL,1);
	if(PQresultStatus(res) != PGRES_TUPLES_OK){
		// Requête invalide
		// Affichage de l'erreur
		fprintf(stderr,"%s",PQerrorMessage(conn));

		// Quitter la base de données puis le serveur
		PQclear(res);
		PQfinish(conn);
		exit(1);
	}

	if(PQntuples(res)==1){
		char hashTry[65];

		// Cryptons le mot de passe essayé
		sha256(name,pass,hashTry);

		// Est-ce le même hash que le bon mot de passe ?
		if(!strcmp(hashTry,PQgetvalue(res,0,0))){
			// Identifiants corrects. Renvoie A ou J (admin ou joueur)
			return PQgetvalue(res,0,1)[0];
		}else{
			// Mauvais mot de passe
			return '0';
		}
	}else{
		// Utilisateur inexistant
		return '0';
	}

	PQclear(res);
}


char createUser(PGconn* conn, char name[32], char pass[32]){
	char* paramValues[2];
	paramValues[0]=name;
	char tampon[64];

	// On crypte le mot de passe
	sha256(name,pass,tampon);
	paramValues[1]=tampon;

	// Insertion du nouveau joueur
	if(PQresultStatus(PQexecParams(conn,"INSERT INTO users VALUES ($1,$2,'J')",
		2,NULL,(const char* const*) paramValues,NULL,NULL,1)) != PGRES_COMMAND_OK){
		fprintf(stderr,"%s",PQerrorMessage(conn));
		return '0';
	}
	return '1';
}


//// [ RESEAU ]

// Affichage d'une chaine de caractères sur la sortie standard
// Entrée : chaine de caractères (char*)
void print(char *s){
	printf("%s", s);
	fflush(stdout);
}

// On vide stdin pour éviter des erreurs imprévues
void flush(){
	int c;
	while ((c = getchar ()) != '\n' && c != EOF);
}
  

// Affichage des informations de cnnexions
void aff_info(struct sockaddr_in* cible){
	printf("[%s:%d]",
		inet_ntoa((struct in_addr) cible->sin_addr),
		ntohs(cible->sin_port)
		);
	fflush(stdout);
}

// Lecture dans la socket
void reading(PGconn* conn, int sock){

	char buff[BUF_LEN];
	int r;

	print(" - - - Prêt pour une demande de COMSERV...\n");

	r=read(sock, buff, BUF_LEN);
	if(r<=0){
		fprintf(stderr,"fail: COMSERV disconnected : %s\n", msg_err);

		// Quitter la base de données puis le serveur
		PQfinish(conn);
		exit(1);

	}else{

		buff[r]='\0';
		printf("Recu: '%s'\n", buff);

		if(buff[0]=='C'){
			// COMSERV demande une vérification d'identifiants
			char protocole[2];
			char id[2];
			char name[32];
			char pass[32];

			// Récupération des informations
			if(sscanf(buff,"%s %s %s %s", protocole, id, name, pass)==-1){
				fprintf(stderr,"fail: sscanf user");
				exit(1);
			}

			// On modifie le buffer pour le renvoyer
			buff[0]='c';

			// MAX 99 JOUEURS
			if(buff[3]==' '){
				// l'identifiant est à un chiffre
				buff[4]=checkAuth(conn,name,pass);
				buff[5]='\0';
			}else{
				// l'identifiant est à deux chiffres
				buff[5]=checkAuth(conn,name,pass);
				buff[6]='\0';
			}

			printf("Envoi: '%s'\n", buff);
			if(write(sock, buff, strlen(buff)+1)!=strlen(buff)+1){
				print("fail: write joueur\n");
			}

			fflush(stdout);

		}else if(buff[0]=='A'){
			// COMSERV souhaite la création d'un utilisateur
			char protocole[2];
			char name[32];
			char pass[32];

			// Récupération des informations
			if(sscanf(buff,"%s %s %s", protocole, name, pass)==-1){
				fprintf(stderr,"fail: sscanf admin\n");
				exit(1);
			}

			// Modification du buffer pour le renvoyer
			buff[0]='a';
			buff[3+strlen(name)]=createUser(conn,name,pass);
			buff[4+strlen(name)]='\0';
	
			// 
			printf("Envoi: '%s'\n", buff);
			if(write(sock, buff, strlen(buff)+1)!=strlen(buff)+1){
				fprintf(stderr,"fail: write admin\n");
			}

			fflush(stdout);
		}
	}
}

//// FONCTION PRINCIPALE
int main(int argc, char const *argv[]){
	char tampontest[64];
	sha256("admin","motdepasse",tampontest);

	// Vérification du nombre d'arguments
	if(argc!=1){
		fprintf(stderr, "usage: %s\n",argv[0]);
		exit(1);
	}

	// Connexion à la base de données
	const char *conninfo;
	PGconn *conn;
	conninfo="dbname = hashdb";

	conn = PQconnectdb(conninfo);
	if(PQstatus(conn) != CONNECTION_OK){
		fprintf(stderr,"db fail: %s\n", PQerrorMessage(conn)); PQfinish(conn); exit(1);
	}else{
		printf("Connecté à la base de données.\n");
	}

	// La table users existe-t-elle ?
	if(PQresultStatus(PQexec(conn,"SELECT type FROM users LIMIT 1"))!=PGRES_TUPLES_OK){
		// table users n'existe pas. Créons là, initialisons l'admin
		initDb(conn);
		printf("La table users n'existait pas et vient d'être créée et initialisée.\n");
	}


	// Préparation des données serveurs
	unsigned int mon_port;
	struct sockaddr_in mes_infos;
	struct sockaddr_in client;
	unsigned int sizeof_client=sizeof(client);
	int sock;
	int sock_stream;
	
	short cont;
	short close_prgm;
	
	mes_infos.sin_family=AF_INET;
	mes_infos.sin_addr.s_addr=INADDR_ANY;
	
	// Port d'écoute ?
	print("Port d'écoute: ");
	scanf("%6ud", &mon_port);
	flush();
	mes_infos.sin_port=htons(mon_port);

	printf("\nCréation du socket: ");	
	// Création du socket
	sock=socket(AF_INET, SOCK_STREAM, 0);
	if(sock==-1){
		fprintf(stderr,"\nFAIL SOCKET : %s\n", msg_err);
		exit(1);
	}
	printf("réussie.\nConnexion du socket: ");	

	// Connexion du socket
	if(bind(sock, (struct sockaddr*) &mes_infos, sizeof(mes_infos))<0){
		fprintf(stderr,"\nFAIL BIND : %s\n", msg_err);
		exit(1);
	}
	printf("réussie.\n");	
	
	// Ecoute du socket
	if(listen(sock, 1)!=0){
		fprintf(stderr,"\nFAIL LISTEN : %s\n", msg_err);
		exit(-1);
	}

	close_prgm=0;
	// Tant que la fermeture du programme n'est pas demandée
	while(close_prgm==0){
		printf("--- En attente de COMSERV sur le port %d ---\n",
			ntohs(mes_infos.sin_port));

		// En attente de la connexion de COMSERV
		sock_stream=accept(sock, (struct sockaddr*) &client, &sizeof_client);
		if(sock_stream<0){
			fprintf(stderr,"\nFAIL ACCEPT : %s\n", msg_err);
			exit(1);
		}
		
		print("---> COMSERV connecté <---\n");
		aff_info(&client);
		print("\n\n");
		
		cont=1;
		while(cont!=0){
			// On lit une demande
			reading(conn, sock_stream);
			sleep(0.1);
		}

		print("\nXXX - Deconnexion de COMSERV.\n");
		close(sock_stream);
	}

	print("XXX - Deconnexion de la base de données.\n");
	PQfinish(conn);

	print("Terminé.\n");
	return 0;
}
