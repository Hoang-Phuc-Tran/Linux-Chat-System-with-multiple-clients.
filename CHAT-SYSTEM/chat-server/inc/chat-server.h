/*	
*   FILE			: chat-server.h
*	PROJECT			: PROG1970 - Assignment #4
*	PROGRAMMER(S)	: Caine Phung - ID: 6983324
*                     Hoang Phuc Tran - ID: 8789102
*				      Philip Wojdyna - ID: 8640180
*	FIRST VERSION	: 2023-27-03
*	DESCRIPTION		: This file contains the header files, constants, and prototypes for chat-server.
*/

//Include
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>

//Constants
#define P_NUM 		        5000
#define USERNAME_MAX		15
#define INDEX 				25
#define MESSAGE_MAX			80
#define CLIENT_MAX			10

//Prototypes
pthread_t createThread(void* thrFunc, void* thrArg);
void deleteUser(int index);
void diffSocket(int idSock, char* message);
void *storeMessage(void *sockId);
void *clientListener(void *tempSock);
void *clientStatus(void);
int initializeConnection(int *sockId);
