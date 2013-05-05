#ifndef HEADER_FUNCTION
#define HEADER_FUNCTION

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

#define msg_err strerror(errno)

void print(const char* s); //affiche, avec un fflush(stdout) à la fin (évite des bugs à la con)
void flush(); //vide l'entrée standard
void aff_info(struct sockaddr_in* cible); //affiche [IP::port] sans \n ou autre
void do_nothing(int);

#endif
