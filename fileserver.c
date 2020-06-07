#include <stdlib.h> 
#include <stdio.h> 
#include <netdb.h> 
#include <netinet/in.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/uio.h>
#include <fcntl.h>
#include <unistd.h>

#define MAX 100
#define PORT 8080 
#define MAX_CLIENT 20
#define SA struct sockaddr

void sentFile(int sockfd, const char *file_name, int size) { 
    char buff[1025];     

    int fp;
    fp=open(file_name,O_RDONLY);  
    if( fp == -1 ){
        printf("Error IN Opening File..\n");
        return ;
    }
    int i;
    while (( i = read(fp, buff, 1024)) != 0 ) {
        send(sockfd,buff,i,0);  
        memset(buff, 0, 1024);

    }
    close (fp);  
    
    printf("File Sent successfully !!! \n");
    
}

long int findSize(const char *file_name) {
    int size = -1;
    FILE *fp = fopen(file_name, "rb");
    if (fp != NULL) {
        fseek(fp, 0, SEEK_END);
        size = ftell(fp);
        rewind(fp);
    }
    fclose(fp);
    return size;
}

int main() { 
    int sockfd, len, fsize; 
    struct sockaddr_in servaddr, cli; 
    int connfd[MAX_CLIENT] = {0};
    char filename[MAX]; 

    sockfd = socket(AF_INET, SOCK_STREAM, 0); 
    if (sockfd < 0) { 
        printf("socket creation failed...\n"); 
        exit(0); 
    } else
        printf("Socket successfully created..\n"); 
        
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &(int){ 1 }, sizeof(int));
    int fl = fcntl(sockfd, F_GETFL, 0);
    fl |= O_NONBLOCK;
    fcntl(sockfd, F_SETFL, fl);

    bzero(&servaddr, sizeof(servaddr));

    servaddr.sin_family = AF_INET; 
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY); 
    servaddr.sin_port = htons(PORT); 

    if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) < 0) { 
        printf("socket bind failed...\n"); 
        exit(0); 
    } 
    else
        printf("Socket successfully binded..\n"); 


    if (listen(sockfd, 5) < 0) { 
        printf("Error listening\n");
    }

    while (1) {		
        fd_set set;		
        FD_ZERO(&set);
        FD_SET(sockfd, &set);		
        FD_SET(STDIN_FILENO, &set);	
        int maxfd = sockfd;
        for (int i = 0; i < MAX_CLIENT; i++) {
            if (connfd[i] > 0) {
                FD_SET(connfd[i], &set);
                if (connfd[i] > maxfd) maxfd = connfd[i];
            }
        }
        select(maxfd+1, &set, NULL, NULL, NULL);

        if (FD_ISSET(STDIN_FILENO, &set)) {
            memset(filename, 0, sizeof(filename));
            int n = read(STDIN_FILENO, filename, sizeof(filename));
            for (int i = 0; i < MAX_CLIENT; i++) { 
                if (connfd[i] > 0) {
                    write(connfd[i], filename, strlen(filename));
                    break;
                }
            }			
        }

        if (FD_ISSET(sockfd, &set)) {

            len = sizeof(cli); 
            int clientfd = accept(sockfd, (SA*)&cli, &len); 

            fl = fcntl(clientfd, F_GETFL, 0);
            fl |= O_NONBLOCK;
            fcntl(clientfd, F_SETFL, fl);

            for (int i = 0; i < MAX_CLIENT; i++) { 
                if (connfd[i] == 0) {
                    connfd[i] = clientfd;
                    printf("accepted client %d, index #%d\n", clientfd, i);
                    break; 
                }
            }
        }

		for (int i = 0; i < MAX_CLIENT; i++) {
			if (connfd[i] > 0 && FD_ISSET(connfd[i], &set)) {
				if (read(connfd[i], filename, sizeof(filename)) > 0) {
                    
                    
                    fsize = findSize(filename);
                    send(connfd[i], (char*)&fsize, 4, 0);
                    if (fsize > 0) {
                        printf("Sending file\n");
                        sentFile(connfd[i], filename,fsize);
                    }
                    shutdown(connfd[i],SHUT_RDWR);
                    close(connfd[i]);
					connfd[i] = 0;
				}
				else {
					printf("connection %d has disconnected.\n", connfd[i]);
                    shutdown(connfd[i],SHUT_RDWR);
                    close(connfd[i]);
					connfd[i] = 0;
				}
                memset(filename, 0, sizeof(filename));
                break;
			} 
		}
    }
}
