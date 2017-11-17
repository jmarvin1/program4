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
const char * Usernames[MAX_THREADS];
int currClient = 0;
int nUsernames = 0;
const char * usernames[100];
const char * passwords[100];


//the thread function
void *connection_handler(void *);
int checkUsername(const char *);
void private(const char *);
void broadcast(int);
const char * getUsers(const char *);
int writeUP(const char *, const char *);
const char * getPassword(const char *);

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

int writeUP(const char * username, const char * password) {

    char fileInput[100];

    FILE *fp;
    fp=fopen("users.txt", "a");
    strcpy(fileInput, username);
    strcat(fileInput, ":");
    strcat(fileInput, password);
    strcat(fileInput, "\n");
    fwrite(fileInput, 1, strlen(fileInput), fp);
    fclose(fp);

    return 0;
}

const char * getPassword(const char * username) {
    FILE * fp;
    char * line = NULL;
    size_t len = 0;
    ssize_t read;

    fp = fopen("users.txt", "r");
    if (fp == NULL)
        exit(EXIT_FAILURE);

    while ((read = getline(&line, &len, fp)) != -1) {
        //printf("Retrieved line of length %zu :\n", read);
        //printf("%s\n", line);
        char *ch1;
        ch1 = strtok(line, ":");
        
        char name[100];
        strcpy(name, ch1);
        ch1 = strtok(NULL, ":");
        printf("ch1: %s\n", ch1);
        printf("uname: %s, ch1: %s\n", username, name);
        if (strcmp(username, name) == 0) {
            fclose(fp);
            if (line)
                free(line);
            printf("password: %s\n", ch1);
            return ch1;
        }
        //printf("password\n");
        //printf("%s\n", ch);
    }

    fclose(fp);
    if (line)
        free(line);
    printf("no password found\n");
    return "what";

}

const char * getUsers(const char * currUser) {
    int i;

//    for (i = 0; i < )
}

int checkUsernames(const char * uname) {

    FILE * fp;
    char * line = NULL;
    size_t len = 0;
    ssize_t read;

    fp = fopen("users.txt", "r");
    if (fp == NULL)
        exit(EXIT_FAILURE);

    while ((read = getline(&line, &len, fp)) != -1) {
//        printf("%s\n", line);
        char *ch;
        ch = strtok(line, ":");
        //printf("%s\n", ch);
        printf("ch: %s, uname: %s\n", ch, uname);
        if (strcmp(ch, uname) == 0) {
            fclose(fp);
            if (line)
                free(line);
            printf("user exists\n");
            return 1;
        }
    }

    fclose(fp);
    if (line)
        free(line);
   
    printf("user does not exist\n");
    return -1;
}

/*
 * This will handle connection for each client
 * */
void *connection_handler(void *socket_desc)
{

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
    char thisUser[100];
    strcpy(thisUser, client_message);
    printf("this user is %s\n", thisUser);
    
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
            if (strcmp(client_message, getPassword(thisUser)) == 0) {
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
        printf("new user, creating password\n");
        printf("password created for user: %s\npassword: %s\n", thisUser, client_message);
        writeUP(thisUser, client_message);
    }
    
    message = "SUCCESS";
    Usernames[currClient - 1] = thisUser;
    write(sock, message, strlen(message));
    memset(client_message, 0, 2000);


    if (read_size <= 0 ) {
        printf("error\n");
    }

    //clear the message buffer
    memset(client_message, 0, 2000);


    const char * prompt = "Let's chat!\n\n\n";

    write(sock , prompt, strlen(prompt));
     
    //Receive a message from client
    while( (read_size = recv(sock , client_message , 2000 , 0)) > 0 )
    {
        //end of string marker
		client_message[read_size] = '\0';
	
        if (strcmp(client_message, "B") == 0) {
            printf("broadcast\n");
            message = "broad\n";
        } else if (strcmp(client_message, "P") == 0) {
            printf("private message\n");
            message = "private message, input user to chat\n";
        } else if (strcmp(client_message, "E") == 0) {
            printf("exit\n");
            message = "exit\n";
        } else {
            printf("incorrect input\n");
            message = "incorrect input\n";
        }

		//Send the message back to client
        write(sock , message, strlen(message));
		memset(client_message, 0, 2000);

        write(sock , prompt, strlen(prompt));
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
