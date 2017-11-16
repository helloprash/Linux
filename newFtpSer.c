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
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>

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
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}


int main(void)
{
    int sockfd, clientToken ;  // listen on sock_fd, new connection on clientToken
    struct addrinfo hints, *servinfo, *p;
    struct sockaddr_storage their_addr; // connector's address information
    socklen_t sin_size;
    struct sigaction sa;
    char message[30];
    int yes=1;
    char s[INET6_ADDRSTRLEN];
    int rv;
    int idx=0 ;
    int retVal ;
    int numbytes ;
    char filename[20];
    char TxBuffer[MAXDATASIZE];
    char RxBuffer[MAXDATASIZE];
    FILE *fptr;
    int flag;
    char command[50];

    memset(&hints, 0, sizeof hints);
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
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1)
        {
            perror("server: socket");
            continue;
        }

        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
                                                                sizeof(int)) == -1)
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
        clientToken = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
        if (clientToken == -1)
        {
            perror("accept");
            continue;
        }

        inet_ntop(their_addr.ss_family,
        get_in_addr((struct sockaddr *)&their_addr),s, sizeof s);
        printf("server: got connection from %s\n", s);

        retVal = fork();
        if (retVal == 0)
        {
            // this is the child process
            close(sockfd); // child doesn't need the listener
            strcpy(message, "Welcome to ftp server");
            retVal = send(clientToken, message, strlen(message) , 0);
            if (retVal == -1)
            {
                perror("send");
            }
    while(1)
            {
                memset(&filename[0], 0, sizeof(filename));
                numbytes = recv(clientToken, RxBuffer, 50, 0);
                if (numbytes <= 0)
                {
                   printf("Receive failed \n");
                }
                printf("Received msg from client: %s\n",RxBuffer);
                printf("Bytes:%d\n",numbytes);

                sscanf(RxBuffer,"%s %s",command,filename);
                printf("Command %s\n",command);
                printf("fileName %s\n",filename);
                if (strcasecmp(command,"get") == 0)
                {
                   fptr=fopen(filename,"r");
                   if(fptr==NULL)
                   {
                       perror("file opening:\n");
                   }
                    flag=1;
 while(flag)
                    {
                        memset(&TxBuffer, 0, sizeof TxBuffer);
                        retVal=fscanf(fptr,"%s ",TxBuffer);
                        if(retVal>0)
                        {
                           printf("%s\n",TxBuffer);
                           idx++;
                           printf("Idx:%d\n",idx);
                           numbytes=send(clientToken,TxBuffer,strlen(TxBuffer),0);
                        }
                        else if(retVal == EOF)
                        {
                                strcpy(TxBuffer,"EOF");
                                numbytes=send(clientToken,TxBuffer,strlen(TxBuffer),0);
                            printf("File sent successfully\n");
                                fclose(fptr);
                                flag=0;
                                close(clientToken);
                                exit(0);
                                                }
                                    }
                    fclose(fptr);
                                    printf("the file has been sent succssfully:\n");
                                        close(sockfd);
                }
    /*if(put)
                {
                        numbytes = recv(clientToken, filename, 50, 0);
                    printf("Bytes:%d\n",numbytes);
                    if (numbytes <= 0)
                    {
                        printf("Receive failed \n");
                    }
                    printf("Received msg from client: %s\n",filename);
                                        fprtr=fopen(file,"w");
                    if(fptr==NULL)
                    {
                        perror("error with file open\n");
                    }
                    while(retVal!=EOF)
                    {
                        retVal=fscanf(fptr,"%c",Buffer)
                                        fprintf(fptr,"%c",Buffer);
                                }
                    printf("the file has been received successfully\n");
                    fclose(fptr);
                            }*/

           }
       }
       else if (retVal < 0)
       {
           printf("fork failed \n");
        }
    }
        // The following is in parent code this happens only whe retVal > 0.
    close(clientToken);  // parent doesn't need this
    exit(0);
}

