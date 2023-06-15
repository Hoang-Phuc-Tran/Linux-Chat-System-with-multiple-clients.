/*	
*   FILE			: chat-client.c
*	PROJECT			: PROG1970 - Assignment #4
*	PROGRAMMER(S)	: Caine Phung - ID: 6983324
*                     Hoang Phuc Tran - ID: 8789102
*				      Philip Wojdyna - ID: 8640180
*	FIRST VERSION	: 2023-27-03
*	DESCRIPTION		: The chat client is a tool that helps the user talk to other people in a chatroom. It connects 
                      the user to the chatroom, which is run by the chat-server program. The user can type and read 
                      messages from other people who are also using the chat client, messages are shown using a
                      a custom library called ncurses. If there's a problem with the connection to the chatroom or 
                      the server simply stops, the client will be notified via a message in the chat room stating 
                      the server has been stopped.
*/


//Include header file
#include "../inc/chat-client.h"


// Global Variables
WINDOW* message_outgoing;
WINDOW* message_incomming;
WINDOW* title_outgoing;
WINDOW* title_incomming;
char queueMsq[MAXIMUM_MESSAGE][MAXIMUM_CHARACTER_MES];
char nameOfUser[MAXIMUM_NAME_LENGTH] = {0}; 
char nameOfServer[IP_STRING_LENGTH] = {0};
int  id_socket;
bool completed = false;



//Main Function
int main(int argc, char *argv[])
{
    // Validate the agruments
    if ((argc != 3 || argc == 3 && (!validateAgruments(argv[1]) || !validateAgruments(argv[2]))))
    {
        printf("Valid Command: chat-client -user<userID> -server<server userName>\n");
    }
    else
    {
        // Initialize ncurses
        initscr();   
        // Initialize colour
        start_color();
        init_pair(2, 7, 0);   
        init_pair(1, 0, 7);

        // Initializes the windows
        title_incomming     = createWindow(3, COLS + 1, 0, 0, 1);
        title_outgoing     = createWindow(3, COLS + 1, LINES - 9, 0, 1);
        message_incomming   = createWindow(LINES - 12, COLS + 1, 3, 0, 2);
        message_outgoing   = createWindow(6, COLS + 1, LINES - 6, 0, 2);

        // Print the the header
        displayHeader("Outgoing Message");

        // Initializes the socket connection with the server
        id_socket = socket(AF_INET, SOCK_STREAM, 0);
        struct sockaddr_in ipAddress;
        ipAddress.sin_family = AF_INET;
        ipAddress.sin_addr.s_addr = inet_addr(nameOfServer);
        ipAddress.sin_port = htons(SOCKET_NUMBER);

        // Check connecting to the server
        if((connect(id_socket, (struct sockaddr *)&ipAddress, sizeof(ipAddress))) == -1)
        {
            // Fail to connect with server
            displayHeader("Connection failed!\n");
        }
        else
        {
            // We will create 2 threads Which spawns the getMsgFromUser and getMsgFromServer threads
            write(id_socket,nameOfUser,strlen(nameOfUser));
            pthread_t pthread[2];
            pthread_attr_t thread_attr;
            pthread_attr_init(&thread_attr);
            pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_JOINABLE);

            // Spawn the listen/receive
            pthread_create(&pthread[0], &thread_attr, getMsgFromUser, NULL);
            pthread_create(&pthread[1], &thread_attr, getMsgFromServer, NULL);

            // Check the connection with server
            while(!completed);
            addMsgToQueueMessage("***************************************");
            addMsgToQueueMessage("Press any key to exit...");
        }
        
        // deletes all of the ncurses windows after waiting for the user to type a character.
        wgetch(message_outgoing);
        endwin();
    }   

    return 0;
}

/* FUNCTION  : displayHeader
DESCRIPTION  : This function is used to display the header on title_outgoing window.
PARAMETERS   : char* _title - The pointer to the header message
RETURNS      : void
*/
void displayHeader(char* _title)
{    
    // Clear the title windows
    cleaningWindow(title_incomming);
    cleaningWindow(title_outgoing);

    // Look up the title offset where the message is centred, calculate it, then print to that spot in both windows.
    int x = (COLS - (strlen("Username: ") + strlen(nameOfUser)))/2;
    mvwprintw(title_incomming, 1.5, x, "Username: %s", nameOfUser);
    x = (COLS - strlen(_title))/2;
    mvwprintw(title_outgoing, 1.5, x, _title);

    // Refresh the title windows
    wrefresh(title_incomming);
    wrefresh(title_outgoing);
}

/* FUNCTION  : addMsgToQueueMessage
DESCRIPTION  : This function is used to add a message to the queue and then display the message to the output_message window.
PARAMETERS   : char* message - The pointer to the message
RETURNS      : void
*/
void addMsgToQueueMessage(char* message)
{
    // Add the message to the queue if it is longer than 13 characters and is not a repetition of the preceding message.
    if (strcmp(queueMsq[MAXIMUM_MESSAGE - 1], message) != 0 && strlen(message) > 13)
    {
        // shift the queue up to one.
        memmove(queueMsq[0], queueMsq[1], sizeof(queueMsq) - sizeof(queueMsq[0]));
        strcpy(queueMsq[MAXIMUM_MESSAGE - 1], message);    

        int xCoordinate;
        int yCoordinate;

        // Get the current x/y coordinates of the cursor
        getyx(message_outgoing, yCoordinate, xCoordinate);

        // clear the window
        cleaningWindow(message_incomming);

        // Loop to print all the messages
        for(int i = 0; i < MAXIMUM_MESSAGE; i++)
        {
            // Calculate the x/y coordinates
            int xOffset = (COLS - strlen(queueMsq[i]))/2 + 1;
            int yOffset = LINES - 22.5;
            // Print all of the messages centered in the console
            mvwprintw(message_incomming, i + yOffset, xOffset, "%s", queueMsq[i]);
        }    

        // Reprint the header
        displayHeader("Outgoing Message");
        // Refresh the window
        wrefresh(message_incomming);
        wrefresh(message_outgoing);

        // Move the cursor back to the coordinates
        wmove(message_outgoing, yCoordinate, xCoordinate);
    }
}

/* FUNCTION  : chunkMessage
DESCRIPTION  : This function is used to chunk the user's message if the message is over 40 characters.
PARAMETERS   : char* userMessage - The pointer to the user's userInput
               char* parcel1 - The pointer to parcel1 which is chunked from the user's userInput
               char* parcel2 - The pointer to parcel2 which is chunked from the user's userInput
RETURNS      : void
*/
void chunkMessage(char* userMessage, char* parcel1, char* parcel2)
{
    int counter = HALF_CHARACTER_MES;
    bool chunkMessage = false;

    while ((userMessage[counter] != ' ' && counter > 0)) 
    {
        if (strlen(userMessage) - counter >= HALF_CHARACTER_MES)
        {
            chunkMessage = true;
            counter = HALF_CHARACTER_MES;
            break;
        }
        counter--;
    }
    if (chunkMessage)
    {
        strncpy(parcel1, userMessage, counter);
        strncpy(parcel2, userMessage + counter, strlen(userMessage) - counter);
    }
    else
    {
        strncpy(parcel1, userMessage, counter);
        strncpy(parcel2, userMessage + counter + 1, strlen(userMessage) - counter);
    }
}

/* FUNCTION  : sendingMsg
DESCRIPTION  : This function is used to send the message to the server, we will format the string
                before we send it to th server
PARAMETERS   : char* ipAdress  - The IP of the client
               char* userName  - The userName of the client
               char* userMsg   - The message from the user
               char* stringTime - The current time
RETURNS      : void
*/
void sendingMsg(char* ipAdress, char* userName, char* userMsg, char* stringTime)
{
    // Initialize message
    char _message[MAXIMUM_CHARACTER_MES];
    memset(_message, 0, MAXIMUM_CHARACTER_MES);

    // format the _message string before sending it to server
    sprintf(_message, "%-16s [%-5s] >> %-40s %9s", ipAdress, userName, userMsg, stringTime);

    // Write msg to the server
    write(id_socket,_message,strlen(_message));
    //Add the message to Queue
    addMsgToQueueMessage(_message);    
}

/* FUNCTION  : getMsgFromServer
DESCRIPTION  : This function is used to check is the socket is still connected and will receive messages from the server.
PARAMETERS   : none
RETURNS      : void
*/
void *getMsgFromServer()
{
    char _buf[80] = {0};

    while(!completed)
    {
        memset(_buf,0,MAXIMUM_CHARACTER_MES - 1);
        if (!(read(id_socket,_buf,MAXIMUM_CHARACTER_MES + 1) < 0) && _buf != NULL)
        {   
            // confirm the connection
            if (send(id_socket, _buf, MAXIMUM_CHARACTER_MES, MSG_NOSIGNAL) == -1)
            {   
                // The connection has been broken
                addMsgToQueueMessage("The connection with the server has been broken");
                close(id_socket);
                completed = true;
                break;
            }
            else
            {
                // Check the junk message 
                if (_buf[0] != 0 && strlen(_buf) > 7)
                {
                    addMsgToQueueMessage(_buf);
                }                
            }            
        }
    }
}

/* FUNCTION  : validateAgruments
DESCRIPTION  : This function is used to parse and vailidate the command line agruments.
PARAMETERS   : char *argv - The array of command line arguments.
RETURNS      : boolean
*/
bool validateAgruments(char *argvCli)
{
    bool validated = true;

    // Check the string starts with "-user" 
    if (strstr(argvCli, "-user") == argvCli) 
    {
        if (strlen(nameOfUser) == 0) 
        {
            // Copy the argument into nameOfUser
            strncpy(nameOfUser, argvCli + strlen("-user"), MAXIMUM_NAME_LENGTH);
        }
        else 
        {
            validated = false;
        }
    }
    // Check the string starts with "-server" 
    else if (strstr(argvCli, "-server") == argvCli)
    {
        if (strlen(nameOfServer) == 0)
        {
            // Copy the argument into nameOfServer
            strcpy(nameOfServer, argvCli + strlen("-server"));
        }
        else
        {
            validated = false;
        }
    }
    else
    {
        // The argument is not valid
        validated = false;
    }

    return validated;
}

/* FUNCTION  : addMsgToQueueMessage
DESCRIPTION  : This function is used to create a new window
PARAMETERS   : int height        : The height the window
               int width         : The width the window
               int xcoordinate   : The x-coordinate of window
               int ycoordinate   : The y-coordinate of window
               int color         : The color
RETURNS      : the pointer WINDOW
*/
WINDOW* createWindow(int heightWindow, int widthWindow, int yCoordinator, int xCoordinator, int color)
{
    WINDOW* _window;
    _window = newwin(heightWindow, widthWindow, yCoordinator, xCoordinator);
    wbkgd(_window, COLOR_PAIR(color));
    keypad(_window, TRUE);
    scrollok(_window, TRUE);
    box(_window,0,0);
    wmove(_window,1,1);
    wrefresh(_window);    
    return _window;
}

/* FUNCTION  : cleaningWindow
DESCRIPTION  : This function is used to clear the given window.
PARAMETERS   : WINDOW* win
RETURNS      : void
*/
void cleaningWindow(WINDOW* _window)
{
	int xCoordinator;
    int yCoordinator;
	getmaxyx(_window, yCoordinator, xCoordinator);

	for (int i = 0;  i < yCoordinator; i++)
	{
		wmove(_window, i, 0);
		wclrtoeol(_window);
	}
	box(_window, 0, 0);
}

/* FUNCTION  : getCharacterInput
DESCRIPTION  : This function is used to get a single character of the user's userInput, and suitably manipulates the supplied string.
PARAMETERS   : char* stringInput - The pointer to the inprocessing string
RETURNS      : char
*/
char getCharacterInput(char* stringInput)
{
    // Get the keystroke from the user and reprint the header text     
    char ch = wgetch(message_outgoing);    

    displayHeader("Outgoing Message");

    // Check if user's userInput is backspace or if the userInput is empty
    if (ch == 7 && strlen(stringInput) > 0)
    {   
        // Clear the last character and decrease the current index if the user presses backspace.
        stringInput[strlen(stringInput) - 1] = 0;                    
    }
    // If the character is valid, we increment the current index and add this new character to the string
    else if (strlen(stringInput) < MAXIMUM_CHARACTER_MES && ch > 31 && ch < 127)
    {
        stringInput[strlen(stringInput)] = ch;
    }
    // If the the current index reaches the maximum size, display the warning in the header
    else if (strlen(stringInput) >= MAXIMUM_CHARACTER_MES)
    {
        displayHeader("Maximum size of message have reached!");
    }
    return ch;
}

/* FUNCTION  : addMsgToQueueMessage
DESCRIPTION  : This threaded function is used to get the user's message userInput, chunk and  sends it to the server.
PARAMETERS   : None
RETURNS      : void
*/
void *getMsgFromUser()
{
    int _socket;
    struct ifreq _ifreq;
    char parcel1[HALF_CHARACTER_MES + 1]; 
    char parcel2[HALF_CHARACTER_MES + 1];
    char userInput[MAXIMUM_CHARACTER_MES];
    char userName[NAME_FIVE_CHARACTERS];
    char stringTime[TIME_STRING_LENGTH];
    char _ipAddress[IP_STRING_LENGTH];
    
    // Chunk five characters from the nameOfUser
    strncpy(userName, nameOfUser, NAME_FIVE_CHARACTERS);

    // clear the _ipAddress string 
    memset(_ipAddress, 0, IP_STRING_LENGTH);

    // gets the current IP of the machine
    _socket = socket(AF_INET, SOCK_DGRAM, 0);
    _ifreq.ifr_addr.sa_family = AF_INET;
    snprintf(_ifreq.ifr_name, IFNAMSIZ, "ens33");
    ioctl(_socket, SIOCGIFADDR, &_ifreq);

    // Format the ip address
    sprintf(_ipAddress, "%s", inet_ntoa(((struct sockaddr_in *)&_ifreq.ifr_addr)->sin_addr));
    close(_socket); 
    
    while(!completed)
    {
        // Clear the strings
        memset(stringTime, 0, TIME_STRING_LENGTH);
        memset(userInput, 0, MAXIMUM_CHARACTER_MES);
        memset(parcel1, 0, HALF_CHARACTER_MES + 1);
        memset(parcel2, 0, HALF_CHARACTER_MES + 1);

        // clear the userInput buffer
        flushinp();
        // clear the window
        cleaningWindow(message_outgoing);
        // Display the message indicator
        mvwprintw(message_outgoing, 2, INDENTATION_INPUT - (strlen(">") + 1), ">"); 
        // Move the cursor to the start position 
        wmove(message_outgoing, 2, INDENTATION_INPUT);

        // Comparing to the enter key when getting character from user's input
        while ((getCharacterInput(userInput) != 10 || strlen(userInput) == 0) && !completed)
        {   
            // clear the window
            cleaningWindow(message_outgoing);
            // Display the message indicator
            mvwprintw(message_outgoing, 2, INDENTATION_INPUT - (strlen(">") + 1), ">");  
            
            // Reprint the string
            if (strlen(userInput) > HALF_CHARACTER_MES)
            {           
                char parcel1[HALF_CHARACTER_MES + 1];
                char parcel2[HALF_CHARACTER_MES + 1];
                memset(parcel1, 0, HALF_CHARACTER_MES + 1);
                memset(parcel2, 0, HALF_CHARACTER_MES + 1);

                // Split the user's input in 2 strings
                chunkMessage(userInput, parcel1, parcel2);

                // Print the two strings
                mvwprintw(message_outgoing, 2, INDENTATION_INPUT, "%s", parcel1);  
                mvwprintw(message_outgoing, 2 + 1, INDENTATION_INPUT, "%s", parcel2);  
            }
            // If it's not over 40 characters we send only one parcel
            else
            {
                mvwprintw(message_outgoing, 2, INDENTATION_INPUT, "%s", userInput);  
            }
        }
        
        // gets the current IP of the machine and saves it to the given char pointer
        time_t current_time;
        struct tm* time_info;
        time(&current_time);
        time_info = localtime(&current_time);
        strftime(stringTime, TIME_STRING_LENGTH, "(%H:%M:%S)", time_info);

        // Check if the program lost connection to the server
        if (!completed)
        {
            // Check exit string
            if(strcmp(userInput,">>bye<<")==0)    
            {
                // Initialize mutex
                pthread_mutex_t mutexsum = PTHREAD_MUTEX_INITIALIZER;  
                completed = true;
                write(id_socket,">>bye<<",strlen(">>bye<<"));
                // Clear the window
                cleaningWindow(message_outgoing);
                // ends the current thread
                pthread_mutex_destroy(&mutexsum);
                pthread_exit(NULL);
            }    
            // If message is larger than 40 characters, we slpit it 2 strings
            else if (strlen(userInput) > HALF_CHARACTER_MES)
            {   
                chunkMessage(userInput, parcel1, parcel2);
                sendingMsg(_ipAddress, userName, parcel1, stringTime);
                sendingMsg(_ipAddress, userName, parcel2, stringTime);  
            }
            // If it's not over 40 characters we send only one parcel
            else
            {
                sendingMsg(_ipAddress, userName, userInput, stringTime);                
            }
        }
    }
}

