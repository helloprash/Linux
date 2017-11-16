#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <dirent.h>
#include <arpa/inet.h>

#define PORT "3490" // the port client will be connecting to

#define MAXDATASIZE 100 // max number of bytes we can get at once

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
    int sockfd, numbytes;
    char buf[MAXDATASIZE];
    struct addrinfo hints, *servinfo, *p;
    int rv;
    int retVal ;
    char s[INET6_ADDRSTRLEN];
    char Buffer[2*1024];
    FILE *fp;
    char filename[]={"test.txt\nsample.txt\nfile.txt\nresult.txt\n"};
    int flag;
    struct dirent *ent;
    DIR *dir;
    char command[10];
    char file[10];
    char status[20];
    char Txbuffer[2048];
    int idx=0;   
 
   if (argc != 2) 
   {
        fprintf(stderr,"usage: client hostname\n");
        exit(1);
    }

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ((rv = getaddrinfo(argv[1], PORT, &hints, &servinfo)) != 0)
	{
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }
    // loop through all the results and connect to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next)
    {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1)
        {
            perror("client: socket");
            continue;
        }

        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1)
        {
            close(sockfd);
            perror("client: connect");
            continue;
        }

        break;
    }

    if (p == NULL) 
	{
        fprintf(stderr, "client: failed to connect\n");
        return 2;
    }

    inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
            s, sizeof s);
    printf("client: connecting to %s\n", s);
    freeaddrinfo(servinfo); // all done with this structure
    numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0);//received welcome to the server
    if (numbytes == -1)
    {
        perror("recv");
        exit(1);
    }
    buf[numbytes] = '\0';
    printf("client: received '%s'\n",buf);

    while(1)
    {
        memset(&buf,0,sizeof(buf));
        memset(&status,0,sizeof(status));
        memset(&file,0,sizeof(file));
        memset(&Txbuffer,0,sizeof(Txbuffer));
        /* file transfer starts*/
        printf("type any command\n");
        printf("1.List\n");
        printf("2.Get file\n");
        printf("3.Put file\n");
  
        scanf("%s %s",command,file);
        sprintf(status,"%s %s",command,file);// buf contains command(list/get) and file contains the filename
  
   /* dislay the filenames when user types list command*/ 
        if(strcasecmp(command,"list")==0)
        {
           printf("%s",filename);
        }
    /* sending filename to the server when user tyes get filename */
        if(strcasecmp(command,"get")==0)
        {
            retVal=send(sockfd,status,strlen(status),0);
            if(retVal<=0)
            {
                perror("send:\n");
            }
            fp=fopen(file,"w");
            if(fp==NULL)
            {
               perror("error with file open\n");
               
            }
            flag=1;
            while(flag)
            {
                memset(&Buffer, 0, sizeof Buffer);
            	numbytes=recv(sockfd,Buffer,sizeof(Buffer), 0);
                if(numbytes <= 0)
                {
                     perror("File receive failed\n");
                }
                printf("Bytes recv:%d\n",numbytes);
                Buffer[numbytes] = '\0';
                printf("Buff:%s\n",Buffer);
                idx++;
                printf("Idx=%d",idx);
                if(numbytes<=0)
                {
                    perror("receive failed:");
                }
                if(Buffer == "EOF")
                {
                	printf("the file has been received successfully\n");
                	fclose(fp);
                        flag=0;
                	close(sockfd);
                	exit(0);
		}
		else
                {
		   fprintf(fp,"%s",Buffer);
                }
	    }
        }
        if(strcasecmp(command,"put")==0)
        {
        	retVal=send(sockfd,status,strlen(status),0);
            if(retVal<=0)
            {
                perror("send:\n");
            }
            
		    fp=fopen(file,"r");
            if(fp==NULL)
            {
                perror("file opening:\n");
            }
            while(flag)
            {
                retVal=fscanf(fp,"%s",Txbuffer);
				if(retVal==EOF)
				{
					printf("the file has been sent successfully\n");
					flag=0;
					fclose(fp);
                	close(sockfd);
                	exit(0);	 
				}
				else	
                {
				    numbytes=send(sockfd,Txbuffer,strlen(Txbuffer),0);
                    if (numbytes <=0)
                    {
                       perror("send");
                    }
				}
			}    	
        }
    }
}

