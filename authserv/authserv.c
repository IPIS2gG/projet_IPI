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
#define SEL_LEN 3
#define NAME_LEN 32

//// [ CRYPTAGE ]

// sha256 crypte un mot en SHA256
// entrée: mot (à crypter) et tampon (où stocker le hash)
void sha256(char* sel, char* mot, char tampon[BUF_LEN])
{
	char toHash[BUF_LEN];
	strcpy(toHash,mot);
	
	// On ajoute un sel pour éviter les attaques de type RAINBOW TABLES
	// Le sel sera composé des SEL_LEN premières lettres du pseudo
	strncat(toHash,sel,SEL_LEN);

	// Initialisation du cryptage (cf. man openssl)
	unsigned char hash[SHA256_DIGEST_LENGTH];
	SHA256_CTX sha256;
	SHA256_Init(&sha256);

	// Hashage
	SHA256_Update(&sha256, toHash, strlen(toHash));

	// Stockage du hash résultant
	SHA256_Final(hash, &sha256);

	// Insertion dans le tampon
	int i=0;
	for(i=0;i<SHA256_DIGEST_LENGTH;i++)
	{
		sprintf(tampon+(2*i), "%02x", hash[i]);
	}

	//[DEBUG]
	printf("\n* FONCTION SHA256\n* SEL  :%s\n* MOT  :%s\n* HASH :%s\n\n",sel,mot,tampon);
}

//// [ BASE DE DONNEES ]

// initDb initialise la base de données (crée la table users et insère l'admin)
void initDb(PGconn* conn)
{
	PGresult   *res;

	// Création de la table (et de ses contraintes)
	res=PQexec(conn,"CREATE TABLE users (name VARCHAR(32) PRIMARY KEY NOT NULL, hash VARCHAR(64) NOT NULL, type CHAR NOT NULL, CONSTRAINT dom_type CHECK (type IN('A','J','0')))");

	if(!res)
	{
		// Echec de la création de la table
		fprintf(stderr,"\n:-(  fail CREATE TABLE\n");
	}

	// Insertion du tuple admin (admin/motdepasse)
	// (L'énoncé présuppose qu'il n'y a qu'un admin)
	// Pour changer de mot de passe, il suffit de printf un appel à sha256 et de remplacer le hash ci-dessous
	res=PQexec(conn,"INSERT INTO users VALUES ('admin','9dcbf36f844d90127640c889ca77d9fe12b9b3a883f056ef3960b1b022c74e69','A')");
	if(!res)
	{
		// Echec de la création de la table
		fprintf(stderr,":-(  fail INSERT INTO\n");
	}else{
		printf(":-) La table users vient d'être créée et initialisée.\n\n");
	}

	//[DEBUG] Affichage du résultat 
	//printf("%s",PQcmdTuples(res));

}

// Offre un token de connexion
PGconn* connexionDb()
{
	// Connexion à la base de données
	PGconn* conn;
	const char *conninfo;
	conninfo="dbname = hashdb";

	conn = PQconnectdb(conninfo);
	if(PQstatus(conn) != CONNECTION_OK)
	{
		fprintf(stderr,"db fail: %s\n", PQerrorMessage(conn)); PQfinish(conn); exit(1);
	}
	else
	{
		printf("\n:-) Connecté à la base de données.");
	}

	// La table users existe-t-elle ?
	if(PQresultStatus(PQexec(conn,"SELECT type FROM users LIMIT 1"))!=PGRES_TUPLES_OK)
	{
		printf("\n\nLa table users n'existe pas\n");
		// table users n'existe pas. Créons là, initialisons l'admin
		initDb(conn);
	}

	return conn;
}


// Supprime la table users, et la recrée.
void dropDb(PGconn* conn)
{
	printf("\n:-? Demande de réinitialisation reçue");
	if(PQresultStatus(PQexec(conn,"DROP TABLE users"))==PGRES_COMMAND_OK)
	{
		printf("\n:-)  TABLE users supprimée.\n");
		initDb(conn);
	}else{
		fprintf(stderr,"\n:-(  TABLE users non supprimée.");
	}
}

//// [ IDENTIFIANTS UTILISATEUR ]

// Vérifie que les identifiants sont corrects
char checkAuth(PGconn* conn, char name[NAME_LEN], char pass[NAME_LEN])
{
	char* paramValues[1];
	paramValues[0]=name;

	PGresult *res;

	// Récupération du hash et du type (admin ou user) de l'utilisateur demandé (name)
	res=PQexecParams(conn,"SELECT hash,type FROM users WHERE name=$1",
		1,NULL,(const char* const*) paramValues,NULL,NULL,1);

	if(PQresultStatus(res) != PGRES_TUPLES_OK)
	{
		// Requête invalide
		// Affichage de l'erreur
		fprintf(stderr,":-( fail requete : %s",PQerrorMessage(conn));

		// Quitter la base de données puis le serveur
		PQclear(res);
		PQfinish(conn);
		exit(1);
	}

	if(PQntuples(res)==1)
	{
		char hashTry[65];

		// Cryptons le mot de passe essayé
		sha256(name,pass,hashTry);

		// Est-ce le même hash que le bon mot de passe ?
		if(!strcmp(hashTry,PQgetvalue(res,0,0)))
		{
			// Identifiants corrects. Renvoie A ou J (admin ou joueur)
			return PQgetvalue(res,0,1)[0];
		}
		else
		{
			// Mauvais mot de passe
			return '0';
		}
	}
	else
	{
		// Utilisateur inexistant
		return '0';
	}

	PQclear(res);
}


char createUser(PGconn* conn, char name[NAME_LEN], char pass[NAME_LEN]){
	char* paramValues[2];
	paramValues[0]=name;
	char tampon[BUF_LEN];

	// On crypte le mot de passe
	sha256(name,pass,tampon);
	paramValues[1]=tampon;

	// Insertion du nouveau joueur
	if(PQresultStatus(PQexecParams(conn,"INSERT INTO users VALUES ($1,$2,'J')",
		2,NULL,(const char* const*) paramValues,NULL,NULL,1)) != PGRES_COMMAND_OK)
	{
		// Requete invalide
		fprintf(stderr,":-( requete invalide : %s",PQerrorMessage(conn));
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

// Lecture dans la socket. Renvoie 0 pour se déconnecter d'un COMSERV agissant anormalement, 1 sinon
int reading(PGconn* conn, int sock){

	char buff[BUF_LEN];
	int r;

	print("\n - - - Prêt pour une demande de COMSERV - - -\n");

	r=read(sock, buff, BUF_LEN);
	if(r<=0)
	{
		fprintf(stderr,"COMSERV disconnected : %s\n", msg_err);
		return 0;
	}
	else
	{

		buff[r]='\0';
		printf("<<< Recu: '%s'\n", buff);

		if(buff[0]=='C')
		{
			// COMSERV demande une vérification d'identifiants
			char protocole[2];
			char id[2];
			char name[NAME_LEN];
			char pass[NAME_LEN];

			// Récupération des informations
			if(sscanf(buff,"%s %s %s %s", protocole, id, name, pass)==-1)
			{
				fprintf(stderr,"fail: sscanf user");
				return 0;
			}

			// On modifie le buffer pour le renvoyer
			buff[0]='c';

			// MAX 99 JOUEURS D'APRES LE PROTOCOLE
			if(buff[3]==' ')
			{
				// l'identifiant est à un chiffre
				buff[4]=checkAuth(conn,name,pass);
				buff[5]='\0';
			}
			else if(buff[4]==' ')
			{
				// l'identifiant est à deux chiffres
				buff[5]=checkAuth(conn,name,pass);
				buff[6]='\0';
			}
			else
			{
				// L'identifiant dépasse 99. COMSERV agit anormalement. Déconnexion.
				return 0;
			}

			// Envoi d'informations
			printf(">>> Envoi: '%s'\n", buff);

			if(write(sock, buff, strlen(buff)+1)!=strlen(buff)+1)
			{
				print("fail: write joueur\n");
				return 0;
			}

			fflush(stdout);

		}
		else if(buff[0]=='A')
		{
			// COMSERV souhaite la création d'un utilisateur
			char protocole[2];
			char name[NAME_LEN];
			char pass[NAME_LEN];

			// Récupération des informations
			if(sscanf(buff,"%s %s %s", protocole, name, pass)==-1)
			{
				fprintf(stderr,"fail: sscanf admin\n");
				return 0;
			}

			// Modification du buffer pour le renvoyer
			buff[0]='a';
			buff[3+strlen(name)]=createUser(conn,name,pass);
			buff[4+strlen(name)]='\0';
	
			// Envoi...
			printf(">>> Envoi: '%s'\n", buff);

			if(write(sock, buff, strlen(buff)+1)!=strlen(buff)+1)
			{
				fprintf(stderr,"fail: write admin\n");
				return 0;
			}

			fflush(stdout);
		}
	}
	return 1;
}

//// FONCTION PRINCIPALE
int main(int argc, char const *argv[]){

	// Vérification du nombre d'arguments
	if(argc>2)
	{
		fprintf(stderr, "usage: %s\n",argv[0]);
		exit(1);
	}
	print("AUTHSERV 1.0\n");

	// Connexion à la base de données
	PGconn *conn=connexionDb();

	// Demande-t-on de réinitialiser la base de données?
	if(argc==2)
	{
		dropDb(conn);
	}

	// Préparation des données serveurs
	unsigned int mon_port;
	struct sockaddr_in mes_infos;
	mes_infos.sin_family=AF_INET;
	mes_infos.sin_addr.s_addr=INADDR_ANY;

	struct sockaddr_in client;
	unsigned int sizeof_client=sizeof(client);

	int sock;
	int sock_stream;
		
	
	// Port d'écoute ?
	print("\nPort d'écoute: ");
	scanf("%6ud", &mon_port);
	flush();
	mes_infos.sin_port=htons(mon_port);

	printf("\nCréation du socket: ");	
	// Création du socket
	sock=socket(AF_INET, SOCK_STREAM, 0);
	if(sock==-1)
	{
		fprintf(stderr,"\n :-( FAIL SOCKET : %s\n", msg_err);
		exit(1);
	}
	printf("réussie. :-) \nConnexion du socket: ");	

	// Connexion du socket
	if(bind(sock, (struct sockaddr*) &mes_infos, sizeof(mes_infos))<0)
	{
		fprintf(stderr,"\n :-( FAIL BIND : %s\n", msg_err);
		exit(1);
	}
	printf("réussie. :-)\n");	
	
	// Ecoute du socket
	if(listen(sock, 1)!=0)
	{
		fprintf(stderr,"\nFAIL LISTEN : %s\n", msg_err);
		exit(-1);
	}

	short cont,close_prgm;

	close_prgm=0;
	// Tant que la fermeture du programme n'est pas demandée
	while(close_prgm==0)
	{
		printf("--- En attente de la connexion de COMSERV sur le port %d ---\n",
			ntohs(mes_infos.sin_port));

		// En attente de la connexion de COMSERV
		sock_stream=accept(sock, (struct sockaddr*) &client, &sizeof_client);
		if(sock_stream<0)
		{
			fprintf(stderr,"\n:-( FAIL ACCEPT : %s\n", msg_err);
			exit(1);
		}
		
		print("---> COMSERV connecté <---\n");
		aff_info(&client);
		print("\n\n");
		
		cont=1;
		while(cont!=0)
		{
			// On lit une demande
			cont=reading(conn, sock_stream);
			sleep(0.1);
		}

		print("\nX X X Deconnexion de COMSERV. X X X\n");
		close(sock_stream);
	}

	print("X X X Deconnexion de la base de données. X X X\n");
	PQfinish(conn);

	print("Terminé.\n");
	return 0;
}
