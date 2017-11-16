/*
** server.c -- a stream socket server demo
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <dirent.h>

#define PORT "3490"  // the port users will be connecting to
#define MAXDATASIZE 100 // max number of bytes we can get at once
#define BACKLOG 10     // how many pending connections queue will hold

void sigchld_handler(int s)
{
    // waitpid() might overwrite errno, so we save and restore it:
    int saved_errno = errno;

    while(waitpid(-1, NULL, WNOHANG) > 0);

    errno = saved_errno;
}


// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) 
	{
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(void)
{
    int sockfd, new_fd ;  // listen on sock_fd, new connection on new_fd
    struct addrinfo hints, *servinfo, *p;
    struct sockaddr_storage their_addr; // connector's address information
    socklen_t sin_size;
    struct sigaction sa;
    int yes=1;
    char s[INET6_ADDRSTRLEN];
    int rv;
    int retVal ;
    int numbytes ;
    int flag;
    char TxBuff[2048];
    char RxBuff[2048];
    char message[2048];
    char fileName[50];
    char folder[50];
    FILE *fptr;


    memset(&hints, 0, sizeof hints);
    memset(&TxBuff, 0, sizeof TxBuff);
    memset(&RxBuff, 0, sizeof RxBuff);
    memset(&message, 0, sizeof message);
    memset(&folder, 0, sizeof folder);
    
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // use my IP

    if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0)
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and bind to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next)
    {
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
        {
            perror("server: socket");
            continue;
        }

        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
		{
            perror("setsockopt");
            exit(1);
        }

        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) 
		{
            close(sockfd);
            perror("server: bind");
            continue;
        }

        break;
    }

    freeaddrinfo(servinfo); // all done with this structure

    if (p == NULL)
    {
        fprintf(stderr, "server: failed to bind\n");
        exit(1);
    }

    retVal = listen(sockfd, BACKLOG);
    if (retVal == -1)
    {
        perror("listen");
        exit(1);
    }

    sa.sa_handler = sigchld_handler; // reap all dead processes
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    retVal = sigaction(SIGCHLD, &sa, NULL);

    if (retVal == -1)
    {
        perror("sigaction");
        exit(1);
    }

    printf("server: waiting for connections...\n");

    while(1)
    {  // main accept() loop
        sin_size = sizeof their_addr;
        new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
        if (new_fd == -1)
        {
            perror("accept");
            continue;
        }

        inet_ntop(their_addr.ss_family, get_in_addr((struct sockaddr *)&their_addr), s, sizeof s);
        printf("server: got connection from %s\n", s);

        retVal = fork();
        if (retVal == 0)
        {
            // THIS IS THE CHILD PROCESS
            close(sockfd); // child doesn't need the listener
           
            while(1)
            {
                if((numbytes = recv(new_fd, RxBuff, strlen(RxBuff), 0)) <= 0) // Recv operation
		 	    {
		 		    perror("Receive failed...");
			    }
			    RxBuff[numbytes] = '\0';
			    printf("Operation: %s",RxBuff);
           	    
           	/*-------Send List of files------*/
			    if(strcasecmp(RxBuff, "List") == 0)
				{
					DIR *d;
                    struct dirent *dir;
                    d = opendir(".");
                    if (d)
                    {
                        while ((dir = readdir(d)) != NULL)
                        {
                        	memset(&TxBuff, 0, sizeof TxBuff);
                            sprintf(TxBuff,"%s\n", dir->d_name);
                            if((numbytes = send(new_fd, TxBuff, sizeof TxBuff, 0)) <= 0)
			                {
			        	        perror("Send file list failed");
					        }
                        }
                        memset(&TxBuff, 0, sizeof TxBuff);
                        strcpy(TxBuff, "EOF");
					    numbytes = send(new_fd, TxBuff, sizeof TxBuff, 0);
                        closedir(d);
                    }
                    return(0);
				}    
			
			/*-------Change Directory------*/
			    else if(strcasecmp(RxBuff,"cd") == 0)
			    {
			    	if((numbytes = recv(new_fd, folder, strlen(folder), 0)) <= 0) // Recv folder name
		 	        {
		 		        perror("Receive failed...");
			        }
			        
			    	if( chdir(folder) == 0 ) 
					{
        				sprintf(TxBuff, "Directory changed to %s\n", folder);
        				if((numbytes = send(new_fd, TxBuff, sizeof TxBuff, 0)) <= 0)
			            {
			        	    perror("Send folder change failed");
					    }
    				} 
					else 
					{
        				perror("folder");
    				}
				}
			/*---------Send to Client***GET***----------*/
                else if(strcasecmp(RxBuff,"Get") == 0)
                {
                	if((numbytes = recv(new_fd, fileName, strlen(fileName), 0)) <= 0) // Recv filename
		 	        {
		 		        perror("Receive filename failed...");
			        }
			        
                	fptr = fopen(fileName, "r");	
		            if(fptr == NULL)
		            {
			            printf("%s\n",strerror(errno));
			            strcpy(TxBuff, "File does not exist");
			            if((numbytes = send(new_fd, TxBuff, sizeof TxBuff, 0)) <= 0)
			            {
			        	    perror("Send failed");
					    }
		            } 
		        
		            //Reading file and sending
		            flag=1;
		            while(flag == 1)
                    {
                	    memset(&message, 0, sizeof message);
                        retVal = fscanf(fptr,"%s",message) ; /* Reading from the file */
                        if (retVal > 0)
                        {
                            if((numbytes = send(new_fd, message, sizeof message, 0)) <=0)
                            {
                   	   	         perror("Send failed...");
			                }
                        }
                        else if (retVal == EOF)
                        {
                            strcpy(message, "EOF");
						    if((numbytes = send(new_fd, message, sizeof message, 0)) <=0)
                            {
                   	   	         perror("Send failed...");
			                }
                            flag = 0; 
                            fclose(fptr);
                        }
                    }
				}
				
				/*-------Receive from client***PUT***------*/
				else if(strcasecmp(RxBuff,"Put") == 0)
				{
					if((numbytes = recv(new_fd, fileName, strlen(fileName), 0)) <= 0) // Recv filename
		 	        {
		 		         perror("Receive filename failed...");
			        }
			        
					fptr = fopen(fileName, "w");	
		            if(fptr == NULL)
		            {
		            	printf("%s\n",strerror(errno));
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
                close(new_fd);
                exit(0);	
			}
            close(new_fd);
            exit(0);
        }
        else if (retVal < 0)
        {
             printf("fork failed \n");
        }

        // The following is in parent code this happens only whe retVal > 0.
        close(new_fd);  // parent doesn't need this
    }
    return 0;
}
