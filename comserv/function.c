#include <stdio.h> 
#include <stdlib.h>
#include <iostream> 
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

#include "function.h"

void print(const char* s)
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
void do_nothing(int)
{ return; }
