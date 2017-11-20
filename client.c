//James Marvin, Michael Burke
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <inttypes.h>
#include <ctype.h>
#include <sys/time.h> 
#include <pthread.h>
#define BUFFER 8128
int ACTIVE = 1;
int private_message_action(int s)
{
    //send P to server
    char action[BUFFER]="P\0";
    int sendSize;
    if((sendSize = send(s, action, strlen(action), 0))<0)
    {
        perror("Error sending the P command\n");
        close(s);
        exit(1);
    }
    return 0;
}
   
int private_message_user_choice(int s){
   printf("Choose an user from the list to talk to: \n");
   return 0;
}
 
int private_message_send_message(int s){
   printf("Enter the message you want to send: \n");
   return 0;
}
int broadcast_message_action(int s){
     //send B to server
    char action[BUFFER]="B\0";
    int sendSize;
    if((sendSize = send(s, action, strlen(action), 0))<0)
    {
        perror("Error sending the B command\n");
        close(s);
        exit(1);
    }
    return 0;
}
int broadcast_send_message(int s){
    printf("Enter the message you want to send: \n");
    return 0;
}
    

void *handle_server_stuff(void *sock){
    //handle lines from other clients
    while(ACTIVE){

        int s = *(int *)sock;
        int rSize;
        char message[BUFFER];
        char message2[BUFFER+1];
        
        rSize=recv(s, message, BUFFER, 0);
        strcpy(message2,message);
        //check to see if it's a data message
        char * token = strtok(message2," ");
        if(strcmp(token,"USERLIST")==0)
        {
            printf("%s\n",message);
            private_message_user_choice(s);
        }
        else if(strcmp(message, "ACKUSERCHOICE")==0)
        {
            private_message_send_message(s);
        }
        else if(strcmp(message, "ACKPRIVATEMESSAGESENT")==0)
        {
            printf("%s\n",message);
        }
        else if(strcmp(message, "ACKB")==0)
        {
            broadcast_send_message(s);
        }
        else if(strcmp(message, "ACKBROADCASTMESSAGESENT")==0)
        {    
            printf("%s\n",message);
        }
        else
        {
            printf("%s\n",message);
        }
        bzero(message,BUFFER);
        bzero(message2,BUFFER);
   }
   return 0;

}
int main(int argc, char * argv[])
{
	struct hostent *hp;
	struct sockaddr_in sin;
	char *host;
    char *username;
	int s;
	int port;
	//Handle arguments 
	if(argc==4)
	{
		host=argv[1];
		port= atoi(argv[2]);
        username = argv[3];
	}
	else
	{
		fprintf(stderr,"Inappropriate number of arguments. Need 2 arguments\n");
		exit(1);
	}
    hp = gethostbyname(host);
	if(!hp)
	{
		fprintf(stderr, "Can't find host\n");
		exit(1);
	}

	//prepare address
	bzero((char *)&sin, sizeof(sin));
	sin.sin_family= AF_INET;
	bcopy(hp->h_addr, (char*)&sin.sin_addr,hp->h_length);
	sin.sin_port = htons(port);

	//create a socket
	if((s=socket(PF_INET,SOCK_STREAM,0))<0)
	{
		perror("error creating socket\n");
		exit(1);
	}
	
	//connet the socket to the server
	if(connect(s, (struct sockaddr *)&sin, sizeof(sin))<0)
	{
		perror("error connecting socket to server\n");
		close(s);
		exit(1);
	}

    //receive initial message from server
    int rSize;
    char rBuffer[BUFFER];
    if ((rSize = recv(s, rBuffer, BUFFER, 0)) <= 0) 
    {
        perror("Error receiving First ack\n");
        close(s);
       exit(1);
    }
    //send username
    int sizeSent;
    if ((sizeSent = send(s, username, strlen(username), 0)) < 0)
    {
        perror("Error sending sizeFile\n");
        close(s);
       exit(1);
    }
    //receive prompt for password
    char passBuff[BUFFER];
    if ((rSize = recv(s, passBuff, BUFFER, 0))<=0)
    {
        perror("Error receiving password prompt\n");
        close(s);
        exit(1);
    }
    char pass[BUFFER];
    printf("%s\n",passBuff);
    scanf("%s",pass);
    //send pass to server
    if ((sizeSent = send(s, pass, strlen(pass), 0)) < 0)
    {
        perror("Error sending password\n");
        close(s);
        exit(1);
    }
   //wait for success/failure of password given
    char successBuff[BUFFER];
    if(( rSize = recv(s, successBuff, BUFFER, 0))<=0)
    {
        perror("Error receiving confirmation of password\n");
        close(s);
        exit(1);
    }
    if(strcmp(successBuff,"SUCCESS")==0)
    {
        //password given was good
    }
    else
    {
        while(strcmp(successBuff, "SUCCESS")!=0)
        {

            bzero(successBuff,BUFFER);
            bzero(pass,BUFFER);
            printf("WRONG! enter pass again:\n");
            scanf("%s",pass);
            //send pass
            if((sizeSent = send(s, pass, strlen(pass),0)) < 0)
            {
                perror("Error sending new pass\n");
                close(s);
                exit(1);
            }
            if((rSize = recv(s, successBuff, BUFFER, 0) <=0))
            {
                perror("error receiving confirmation of other pass attempt\n");
                close(s);
                exit(1);
            }
         } 
     }
    //create thread to handle messages from other clients
    pthread_t thread;
    int rc = pthread_create(&thread, NULL, handle_server_stuff, (void*)&s);
    if(rc)
    {
        perror("Error creating thread\n");
        close(s);
        exit(1);
    }
    //prompts and function calls    
    
    printf("Enter a command to perform:\n");
    printf("P for private message\n");
    printf("B for broadcast message\n");
    printf("E for exit\n");
    while(1)
    {   
        //printf(">> "); 
        char inputAction [BUFFER];
        scanf("%s", inputAction);
        if (strcmp(inputAction, "P") == 0)
        {
            private_message_action(s);
        }
        else if (strcmp(inputAction, "B") == 0)
        {
           broadcast_message_action(s);
        } 
        else if (strcmp(inputAction, "E") == 0)
        {
            int sizeSent;
	        char action [BUFFER] = "E\0";
    	    if ((sizeSent = send(s, action, strlen(action), 0)) < 0)
    	    {
        	    perror("Error sending action E\n");
        	    close(s);
        	    exit(1);
    	    }
            ACTIVE=0;
            pthread_join(thread,NULL);
            close(s);
            return 0;
            //exit(0);
        } 
        else 
        {
            //printf("error: improper action: try again\n");
            if(sizeSent = send(s, inputAction, strlen(inputAction), 0)<0)
            {
                perror("Error sending action\n");
                close(s);
                exit(1);
            }
        } 
    }
    close(s);
    return 0;    
}
