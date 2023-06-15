/*	
*   FILE			: chat-client.h
*	PROJECT			: PROG1970 - Assignment #4
*	PROGRAMMER(S)	: Caine Phung - ID: 6983324
*                     Hoang Phuc Tran - ID: 8789102
*				      Philip Wojdyna - ID: 8640180
*	FIRST VERSION	: 2023-27-03
*	DESCRIPTION		: This file contains the header files, constants, and prototypes for chat-client.
*/

//Include
#include <netinet/in.h>
#include <pthread.h>
#include <signal.h>
#include <arpa/inet.h>
#include <curses.h>
#include <ifaddrs.h>
#include <net/if.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <termios.h>
#include <time.h>

//Constants
#define SOCKET_NUMBER          5000
#define MAXIMUM_MESSAGE        10
#define MAXIMUM_CHARACTER_MES  80
#define HALF_CHARACTER_MES     40
#define TIME_STRING_LENGTH     11
#define IP_STRING_LENGTH       20
#define MAXIMUM_NAME_LENGTH    15
#define NAME_FIVE_CHARACTERS   5
#define INDENTATION_INPUT        (COLS / 2) - 20

//Prototypes
char getCharacterInput(char*);
void displayHeader(char*);
void addMsgToQueueMessage(char*);
void chunkMessage(char*, char*, char*);
void sendingMsg(char*, char*, char*, char*);
void *getMsgFromUser();
void *getMsgFromServer();
bool validateAgruments(char *);
void cleaningWindow(WINDOW*);
WINDOW* createWindow(int, int, int, int, int);
