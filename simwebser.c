#include<netinet/in.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/socket.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<unistd.h>

int main() 
{
    int create_socket, new_socket;
    struct sockaddr_in address,client;
    socklen_t addrlen = sizeof(address);
    int bufsize = 1024;
    char *buffer = malloc(bufsize);
    FILE *fptr;
    int yes = 1;
    char fileName[40];

       if ((create_socket = socket(AF_INET, SOCK_STREAM, 0)) > 0)
       {
	   printf("The socket was created\n");
       }

       setsockopt(create_socket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

       memset(&address, 0, sizeof address);
       memset(&client, 0, sizeof client);

       address.sin_family = AF_INET;
       address.sin_addr.s_addr = INADDR_ANY;
       address.sin_port = htons(80);

       if (bind(create_socket, (struct sockaddr *) &address, sizeof(address)) == 0)
       {
	   printf("Binding Socket\n");
       }
				         
       while (1) 
       {
	   if (listen(create_socket, 10) < 0) 
	   {
	        perror("server: listen");
		exit(1);
	   }
           printf("Waiting for connections..\n");
	   if ((new_socket = accept(create_socket, (struct sockaddr *) &client, &addrlen)) < 0) 
	   {
		perror("server: accept");
		exit(1);
           }

	   if (new_socket > 0)
	   {
		 printf("The Client is connected...\n");
	   }

	   recv(new_socket, buffer, bufsize, 0);
	   printf("%s\n", buffer);
	   write(new_socket, "HTTP/1.1 200 OK\n", 16);
	   write(new_socket, "Content-length: 46\n", 19);
	   write(new_socket, "Content-Type: text/html\n\n", 25);
	   
	   strcpy(fileName,"/var/www/html/index.html");
	   fptr = fopen(fileName,"w");
           fprintf(fptr,"<HTML>");
	   fprintf(fptr,"<H1><center>Prashanth Kumar</center></H1>");
	   fprintf(fptr,"<p><H2>Contact details</H2></p>");
	   fprintf(fptr,"<a href=https://facebook.com/prashrox7>Facebook</a><br>");
	   fprintf(fptr,"<a href=https://twitter.com/prashanth77me>Twitter</a><br>");
	   fprintf(fptr,"Mobile:+919902327331<br>");
           fprintf(fptr,"</HTML>");
	   fclose(fptr);

	   close(new_socket);
       }
       close(create_socket);
       return 0;
}
        
