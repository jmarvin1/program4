/*
    C socket server example, handles multiple clients using threads
    Compile
    gcc server.c -lpthread -o server
*/
 
#include<stdio.h>
#include<string.h>    //strlen
#include<stdlib.h>    //strlen
#include<sys/socket.h>
#include<arpa/inet.h> //inet_addr
#include<unistd.h>    //write
#include<pthread.h> //for threading , link with lpthread
#define MAX_THREADS 10

int clients[MAX_THREADS] = {0};
int currClient = 0;
int nUsernames = 0;
const char * usernames[100];
const char * passwords[100];


//the thread function
void *connection_handler(void *);
int checkUsername(const char *);

int main(int argc , char *argv[])
{
    int socket_desc , client_sock , c;
    struct sockaddr_in server , client;
     
    //Create socket
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1)
    {
        printf("Could not create socket");
    }
    puts("Socket created");
     
    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(41030);
     
    //Bind
    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        //print the error message
        perror("bind failed. Error");
        return 1;
    }
    puts("bind done");
     
    //Listen
    listen(socket_desc , 3);
     
    //Accept and incoming connection
    puts("Waiting for incoming connections...");
    c = sizeof(struct sockaddr_in);
     
     
    //Accept and incoming connection
    puts("Waiting for incoming connections...");
    c = sizeof(struct sockaddr_in);
	pthread_t thread_id;
	
    while( (client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c)) )
    {
        if (currClient >= MAX_THREADS) {
            puts("max number of connections reached. Accepting no more clients");
            continue;
        }
        puts("Connection accepted");
        
        clients[currClient++] = client_sock;

        if( pthread_create( &thread_id , NULL ,  connection_handler , (void*) &client_sock) < 0)
        {
            perror("could not create thread");
            return 1;
        }
         
        //Now join the thread , so that we dont terminate before the thread
        //pthread_join( thread_id , NULL);
        puts("Handler assigned");
    }
     
    if (client_sock < 0)
    {
        perror("accept failed");
        return 1;
    }
     
    return 0;
}
 
int checkUsernames(const char * uname) {

    int i;

    for (i = 0; i < nUsernames; i++) {
        if (strcmp(uname, usernames[i]) == 0) {
            return i;
        }
    }
    
    return -1;
}

/*
 * This will handle connection for each client
 * */
void *connection_handler(void *socket_desc)
{

    usernames[nUsernames] = "marv";
    passwords[nUsernames] = "smarv";
    nUsernames++;
    //Get the socket descriptor
    int sock = *(int*)socket_desc;
    int read_size;
    char *message , client_message[2000], server_message[2000];
     
    //Send some messages to the client
    message = "Greetings! Welcome to chat! Enter a username\n";
    write(sock , message , strlen(message));

    read_size = recv(sock , client_message , 2000 , 0);
    if (read_size <= 0) {
        printf("error!");
    }
    //end of string marker
    client_message[read_size] = '\0';
    printf("message: %s\n", client_message);
    
    int check = checkUsernames(client_message);
    const char * thisUser = client_message;
    if (check == -1) {
        printf("Welcome new user!\n");     
        printf("Enter a password for us to save!\n");
        message = "Enter a password for us to save!\n";
    } else {
        printf("Welcome returning user!\nEnter password:");
        message = "Welcome returning user!\nEnter password:";
    }
    //Send the message back to client
    write(sock , message , strlen(message));
    memset(client_message, 0, 2000);
    read_size = recv(sock, client_message, 2000, 0);

    client_message[read_size] = '\0';
    if (check >= 0) {
        printf("Check: %d\n", check);
        printf("returning user\n");
        while(1) {
            if (strcmp(client_message, passwords[check]) == 0) {
                printf("password matched\n");
                break;
            } else {
                printf("incorrect password.\nTry again:");
                message = "Incorrect password.\nTry again:";
                write(sock, message, strlen(message));
                memset(client_message, 0, 2000);
                read_size = recv(sock, client_message, 2000, 0);
            }
        }
    } else {
        usernames[nUsernames] = thisUser;
        printf("new user, creating password\n");
        passwords[nUsernames] = client_message;
        printf("password created for user: %s\npassword: %s\n", usernames[nUsernames], passwords[nUsernames]);
        nUsernames++;
    }

    message = "SUCCESS";
    write(sock, message, strlen(message));
    memset(client_message, 0, 2000);


    if (read_size <= 0 ) {
        printf("error\n");
    }

    //clear the message buffer
    memset(client_message, 0, 2000);


    message = "Now type something and i shall repeat what you type \n";
    write(sock , message , strlen(message));
     
    //Receive a message from client
    while( (read_size = recv(sock , client_message , 2000 , 0)) > 0 )
    {
        //end of string marker
		client_message[read_size] = '\0';
		
		//Send the message back to client
        write(sock , client_message , strlen(client_message));
		
		//clear the message buffer
		memset(client_message, 0, 2000);
    }
     
    if(read_size == 0)
    {
        puts("Client disconnected");
        fflush(stdout);
    }
    else if(read_size == -1)
    {
        perror("recv failed");
    }
         
    return 0;
} 
