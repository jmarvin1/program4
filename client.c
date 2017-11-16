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

#define BUFFER 8128

//upload function 
int upld(int s)
{
    //send upld action
  char action [BUFFER];
  printf("Enter string to send\n");
  scanf("%s",action);
    int sizeSent;
    //char action [BUFFER] = "UPLD\0";
    if ((sizeSent = send(s, action, strlen(action), 0)) < 0)
    {
        perror("Error sending action\n");
        close(s);
        //free(buf);
        //fclose(fp);
        exit(1);
    }
    
    /*int rSize;
    char rBuffer[BUFFER];
    if ((rSize = recv(s, rBuffer, BUFFER, 0)) <= 0) 
    {
        perror("Error receiving First ack\n");
        close(s);
        printf("size of received: %d\n", rSize);
        printf("From Server: %s\n", rBuffer);
        exit(1);
    }
    if (strcmp(rBuffer, "ACK") == 0)
    { 
        char sizeFile [BUFFER];
        
        int tmpInt = (int)(strlen(buf));
        uint32_t sizeOfFile = htonl(tmpInt);
        sprintf(sizeFile, "%" PRIu32, sizeOfFile);
        printf("%" PRIu32 " yeet\n", sizeOfFile);
        
        if ((sizeSent = send(s, sizeFile, strlen(sizeFile), 0)) < 0)
        {
            perror("Error sending sizeFile\n");
            close(s);
            free(buf);
            fclose(fp);
            exit(1);
        }
        if ((sizeSent = send(s, buf, strlen(buf), 0)) < 0)
        {
            perror("Error sending UPLD file\n");
            close(s);
            free(buf);
            fclose(fp);
            exit(1);
        }
    }
    */
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
	//check username against users.txt
    //if((filepointer=fopen("users.txt","r"))!=NULL){
        //read file
	//prepare hostname 
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
        //printf("size of received: %d\n", rSize);
        //printf("From Server: %s\n", rBuffer);
        exit(1);
    }
    //send username
    int sizeSent;
    if ((sizeSent = send(s, username, strlen(username), 0)) < 0)
    {
        perror("Error sending sizeFile\n");
        close(s);
        //free(buf);
        //fclose(fp);
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
    // int sizeSent;
    if ((sizeSent = send(s, pass, strlen(pass), 0)) < 0)
    {
        perror("Error sending password\n");
        close(s);
        //free(buf);
        //fclose(fp);
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
        //send RTS
        char action[BUFFER]= "RTS\0";
        if((sizeSent = send(s, action, strlen(action),0))< 0)
        {
            perror("Error sending RTS\n");
            close(s);
            exit(1);
        }
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
    
    //prompts and function calls   
    while (1) 
    {
        printf("Enter a command to perform:\n\t");
        printf("DWLD: Download\n\tUPLD: Upload\n\tLIST: List\n\tMDIR: ");
        printf("Make Directory\n\tRDIR: Remove Directory\n\tCDIR: ");
        printf("Change Directory\n\tDELF: Delete File\n\tQUIT: Exit\n");
        
        char inputAction [BUFFER];
        scanf("%s", inputAction);
        if (strcmp(inputAction, "UPLD") == 0)
        {
            printf("UPLD\n");
            upld(s);
        } 
       
        else if (strcmp(inputAction, "QUIT") == 0)
        {
            int sizeSent;
	        char action [BUFFER] = "QUIT\0";
    	    if ((sizeSent = send(s, action, strlen(action), 0)) < 0)
    	    {
        	    perror("Error sending action MDIR\n");
        	    close(s);
        	    exit(1);
    	    }
            close(s);
            return 0;
        } else {
            printf("error: improper action: try again\n");
        } 
    }
    close(s);
    return 0;    
}
