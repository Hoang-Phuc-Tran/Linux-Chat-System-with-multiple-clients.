/*	
*   FILE			: chat-server.c
*	PROJECT			: PROG1970 - Assignment #4
*	PROGRAMMER(S)	: Caine Phung - ID: 6983324
*                     Hoang Phuc Tran - ID: 8789102
*				      Philip Wojdyna - ID: 8640180
*	FIRST VERSION	: 2023-27-03
*	DESCRIPTION		: The chat-server is a server that enables multiple chat clients to connect to it. 
					  Once connected, chat clients can send and receive messages with each other. The chat-server 
					  program manages the connections and handles any disconnections. The chat client program serves 
					  as the user's gateway to the chatroom, which is managed by the chat-server program.
*/


//Include header file
#include "../inc/chat-server.h"


//Global Variables
char userName[CLIENT_MAX][USERNAME_MAX];        
int numSock = 0;                                
bool connection = false;
int numSockArray[CLIENT_MAX];
pthread_t userThread[CLIENT_MAX];

//Main Function
int main(int argc, char *argv[])
{
	
	//Clear global variables
	memset(numSockArray, 0, sizeof(numSockArray));
	memset(userName, 0, sizeof(userName));
    int socket;

    //Connect
    if (initializeConnection(&socket) < 0) { 
		perror("Binding Error");
    	exit(1);
	}

    //Start threads for listening for user, and status of user
	pthread_t listenerThread = createThread(clientListener, (void*)&socket);
	pthread_t statusThread = createThread(clientStatus, NULL);

	//Active clients checked
	while (numSock > 0 || !connection){ 
		sleep(1);
	}

	pthread_cancel(listenerThread);

    close(socket);
    return 0;
}

// FUNCTION     : deleteUser
// DESCRIPTION  : Delete specific user from global index according to passed index.
// PARAMETERS   : int index
// RETURNS   	: void
void deleteUser(int index)
{

	memmove(&numSockArray[index], &numSockArray[index+1], sizeof(numSockArray) - sizeof(numSockArray[index]));
	memmove(&userName[index], &userName[index+1], sizeof(userName) - sizeof(userName[index]));
	memmove(&userThread[index], &userThread[index+1], sizeof(userThread) - sizeof(userThread[index]));	

	//Subtract the count of connected clients		    		
	numSock--;
}



// FUNCTION     : diffSocket
// DESCRIPTION  : Any live sockets recieve a message, but not the passed socket.
// PARAMETERS   : int idSock, char* message 
// RETURNS      : void
void diffSocket(int idSock, char* message)
{   

    //Go through all connected sockets
    for (int i = 0; i < numSock; i++)
    {
        //Check if socket is not equal to passed socket
        if (numSockArray[i] != idSock)
        {
            //Display message
            write(numSockArray[i], message, strlen(message));
        }
    }
}

// FUNCTION      : storeMessage
// DESCRIPTION   : Gets message from passed socket and stores it.
// PARAMETERS    : void *sockId
// RETURNS       : void
void *storeMessage(void *sockId)
{
    
    //Array to store msg
    char messageBuff[MESSAGE_MAX];

    //sockId casted to an int, and assigned to socket int
    int socket = *(int*)sockId;

    //Read int
    int r;

    while(true)
    {

        //Sleep for 10 microsecs
        usleep(10);

        //Buffer cleared
        memset(messageBuff, 0, MESSAGE_MAX);

        //Read msg
        r = read(socket,messageBuff,MESSAGE_MAX - 1);

        //Check first character, and check if not empty
        if (messageBuff[INDEX] == '>' && messageBuff[0] != 0)
        {
            //Format cleaned
            messageBuff[INDEX] = '<';
            messageBuff[INDEX+1] = '<';

            //Send message to other sockets
            diffSocket(socket, messageBuff);
        }

        //Check if message is equal to >>bye<< string, which will disconnect client
        else if (messageBuff[0] != 0)
        {
            if (strcmp(messageBuff, ">>bye<<") == 0)
            {
                //Finds index of socket, and index is stored in x.
                int x = 0;

                for (int i = 0; i < numSock; i++)
                {
                    if (numSockArray[i] == socket)
                        x = i;
                }
                
                //Creates message to be sent to chatroom
                sprintf(messageBuff, "%s left the chatroom", userName[x]);
                
                //Display message to entire chatroom
                diffSocket(socket, messageBuff);
                
                //Remove user
                deleteUser(x);
                
                //Exit thread
                pthread_exit(NULL);
            }
        }
    }
}



// FUNCTION     : clientListener
// DESCRIPTION  : Creates listener for incoming users.
// PARAMETERS   : void *tempSock
// RETURNS      : void
void *clientListener(void *tempSock)
{ 
    //Declare variables
    socklen_t clientInfo;
    int newSock;
    int sockId = *(int*)tempSock;

    //Declare struct to store client address
    struct sockaddr_in clientAddress;
    
    //Loop until conditions are met
    while (!connection || numSock > 0)
    {
        //If max clients is reached, sleep for one second
        while(numSock == CLIENT_MAX - 1){ 
            sleep(1); 
        }

        //Listen for client
        listen(sockId,5);

        //Store info of client
        clientInfo = sizeof(clientAddress);
        newSock = accept(sockId, (struct sockaddr *)&clientAddress, &clientInfo);

        //Socket set to non-blocking mode
        fcntl(newSock, F_SETFL, O_NONBLOCK);

        //If connection successful
        if (newSock >= 0)
        {

            //Connection bool set to true
            connection = true;
            while(true)
            {
                //Check and store username
                if (userName[numSock] != NULL && read(newSock,userName[numSock],sizeof(userName[numSock])) >= 0)
                {
                    //Create message alerting users a new user has joined
                    char msg[MESSAGE_MAX];
                    memset(msg, 0, MESSAGE_MAX);                    
                    sprintf(msg, "[SERVER] >> User %s entered the chatroom.", userName[numSock]);
                    diffSocket(sockId, msg);

                    break;  
                }
            }        

            //New thread for user messages
            pthread_t msgThread = createThread(storeMessage, (void*)&newSock);

            //Store ID
            userThread[numSock] = msgThread;

            //New socket stored in numSockArray, and numSock count is increased
            numSockArray[numSock++] = newSock;
        }
    }
}



// FUNCTION     : clientStatus
// DESCRIPTION  : Checks the connection status of connected clients.
// PARAMETERS   : void
// RETURNS      : void
void *clientStatus(void)
{

    //Loop while there are active sockets
	while (numSock > 0 || !connection)
	{

        //Look through active sockets
		for (int i = 0; i < numSock; i++)
		{
			int activeSocket = numSockArray[i];
		    int sizeOfBuffer = MESSAGE_MAX;
		    char *buff = malloc(sizeOfBuffer);
	        memset(buff,0,sizeOfBuffer);
			
            //Check if data can be received
	        if (!(recv(activeSocket,buff,sizeOfBuffer, MSG_PEEK) < 0) && buff != NULL)
	        {

                //Send to the client, if that operation fails that means the client has disconnected in which other users will 
                //be informed and that user is removed from active socket list
	            if (send(activeSocket, buff, sizeof(buff), MSG_NOSIGNAL) == -1) 
	            {
					sprintf(buff, "[SERVER] >> User %s left the chatroom", userName[i]);
	    			diffSocket(activeSocket, buff);
	    			deleteUser(i);
	    			break;
	            }           
	        }

	        free(buff);
		}

		sleep(1);
	}
}



// FUNCTION     : createThread
// DESCRIPTION  : Creates new thread with passed parameters.
// PARAMETERS   : void* thrFunc, void* thrArg
// RETURNS      : -1 (if connection fails)
pthread_t createThread(void* thrFunc, void* thrArg)
{	

	//Thread variables
	pthread_t newThreads;
    pthread_attr_t attr;

	//Set to default
    pthread_attr_init(&attr);

	//Set to joinable
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

	//Start new thread	
	pthread_create(&newThreads, &attr, thrFunc, thrArg);

	//ID of new thread returned
	return newThreads;
}



// FUNCTION     : initializeConnection
// DESCRIPTION  : Starts connection with server from user.
// PARAMETERS   : int *sockId 
// RETURNS      : -1 (if bind fails)
int initializeConnection(int *sockId)
{

    struct sockaddr_in serverAddress;

    //New socket
    *sockId = socket(AF_INET, SOCK_STREAM, 0);
	
    //If error in creating socket, display error message
    if (*sockId < 0)
    {
        perror("Error opening the socket");
        exit(1);
    }

    //Clear struct
    memset((char *) &serverAddress, 0, sizeof(serverAddress));

    //Set values of struct
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(P_NUM);

    //Connect to address and port number
    return bind(*sockId, (struct sockaddr *) &serverAddress, sizeof(serverAddress));
}


