/*
** client.c -- a stream socket client demo
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <arpa/inet.h>

#define PORT "3490" // the port client will be connecting to 


// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(int argc, char *argv[])
{
    int sockfd;  
    struct addrinfo hints, *servinfo, *p;
    int rv;
    int numbytes;
    char s[INET6_ADDRSTRLEN];
    int flag;
    int retVal;
    char listOfFiles[] = "letter.txt\nhello.txt";
    char TxBuff[2048];
    char RxBuff[2048];
    char message[2048];
    char fileName[50];
    buffer[2048];
    FILE *fptr;

    if (argc != 2) {
        fprintf(stderr,"usage: client hostname\n");
        exit(1);
    }

    memset(&hints, 0, sizeof hints);
    memset(&TxBuff, 0, sizeof TxBuff);
    memset(&fileName, 0, sizeof fileName);
    memset(&buffer, 0, sizeof buffer);
    
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ((rv = getaddrinfo(argv[1], PORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and connect to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("client: socket");
            continue;
        }

        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("client: connect");
            continue;
        }

        break;
    }

    if (p == NULL) {
        fprintf(stderr, "client: failed to connect\n");
        return 2;
    }

    inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
            s, sizeof s);
    printf("client: connecting to %s\n", s);

    freeaddrinfo(servinfo); // all done with this structure

    while(1)
    {
        scanf("%s %s",TxBuff,fileName);
        sprintf(buffer,"%s %s",TxBuff,fileName);
        
        printf("Command:%s\n",TxBuff);
        printf("fileName:%s\n",fileName);
        
        /*-----List of Files-----*/
        if(strcasecmp(TxBuff,"List") == 0)
        {
        	printf("%s",listOfFiles);
		}
		
        /*------Receive from Server***GET***------*/
		 else if(strcasecmp(TxBuff, "Get")== 0)
		 {
		 	if((numbytes = send(sockfd, buffer, sizeof buffer, 0)) <= 0)  // Send Operation
		    {
		 	    perror("Send failed...");
		 	    continue;
		    }
			
			fptr = fopen(fileName,"w");
			if(fptr == NULL)
			{
				printf("%s\n",strerror(errno));
				continue;
			}
			//Receiving loop
			while(1)
			{
				memset(&RxBuff, 0, sizeof RxBuff);
				if((numbytes = recv(sockfd, RxBuff, sizeof RxBuff, 0)) <= 0)
                {
                   	perror("Receive failed...");
	            }
	            
	            if(strcmp(RxBuff, "EOF") == 0)
	            {
	            	fclose(fptr);
	                break;
	            }
	            else
	                fprintf(fptr, "%s", RxBuff);
			}
		 }
		 
		 /*-----Send to Server***PUT***----*/
		 else if(strcasecmp(TxBuff, "Put") == 0)
		 {
		 	if((numbytes = send(sockfd, TxBuff, sizeof TxBuff, 0)) <= 0)  // Send Operation
		    {
		 	    perror("Send failed...");
		 	    continue;
		    }
		    
		 	scanf("Enter the filename to be Sent: %s",fileName);    
		 	if((numbytes = send(sockfd, fileName, sizeof fileName, 0)) <= 0)  // Send Filename
		 	{
		 		perror("Send failed...");
			}
			
			fptr = fopen(fileName, "r");
			if(fptr == NULL)
			{
				printf("%s\n",strerror(errno));
				continue;
			}
			
			 //Reading file and sending
		    flag=1;
		    while(flag == 1)
            {
                memset(&message, 0, sizeof message);
                retVal = fscanf(fptr,"%s",message) ; /* Reading from the file */
                if (retVal > 0)
                {
                    if((numbytes = send(sockfd, message, sizeof message, 0)) <=0)
                    {
                   	   	perror("Send failed...");
			        }
                }
                else if (retVal == EOF)
                {
                    strcpy(message, "EOF");
					if((numbytes = send(sockfd, message, sizeof message, 0)) <=0)
                    {
                   	   	perror("Send failed...");
			        }
                    flag = 0; 
                    fclose(fptr);
                }
            }
		 }	
	}
    close(sockfd);

    return 0;
}
