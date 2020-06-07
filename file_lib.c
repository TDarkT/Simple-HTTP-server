#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>
struct sockaddr_in fserver_addr;
struct hostent *host;
int fserver_fd;

int init_fserver_sk(char *ip) {
    host = gethostbyname(ip);
    if (host == NULL) {
        printf("Error: invalid ip for file server");
        exit(1);
    }
    int yes =1;
    memset(&fserver_addr, 0, sizeof(fserver_addr));
    fserver_addr.sin_family = AF_INET;
    fserver_addr.sin_addr = *(struct in_addr *)host->h_addr_list[0];
    fserver_addr.sin_port = htons(8080);

    fserver_fd = socket(AF_INET, SOCK_STREAM, 0);
    return fserver_fd;
}

char *get_file(char* filename) {
    char* file_data = NULL; 
    if (connect(fserver_fd, (struct sockaddr *) &fserver_addr, sizeof(fserver_addr)) == 0) {
        printf("Getting requested file\n");
        if (setsockopt(fserver_fd, SOL_SOCKET, SO_REUSEADDR, &(int){1}, 4) == -1);
            fprintf(stderr, "%s\n", strerror(errno));
        int fl = fcntl(fserver_fd, F_GETFL, 0);
        fl |= O_NONBLOCK;
        fcntl(fserver_fd, F_SETFL, fl);

        int b = send(fserver_fd, filename, strlen(filename), 0);
        int file_size;
        int a = recv(fserver_fd,(char*)&file_size, 4, 0);
        if (file_size != 0) {
            char buff[4096];
            if (file_size > 0) {
                file_data = (char *) malloc(file_size);
                while(recv(fserver_fd, buff, 100, 0) > 0 ) {
                    strcat(file_data, buff);
                    memset(buff, 0 , 100);
                }
            }
        }
    }else {
        fprintf(stderr, "Can't connect to file server: %s\n", strerror(errno));
    }
    shutdown(fserver_fd, SHUT_RDWR);
    close(fserver_fd);
    printf("conection closed\n");
    return file_data;
}

int get_file_size(FILE *fp) {
    int size = -1;
    if (fp != NULL) {
        fseek(fp, 0, SEEK_END);
        size = ftell(fp);
        rewind(fp);
    }
    return size; 
}

char *get_file_content(FILE *fp, int size) {
    char *buff = (char *) malloc(size+1);
    int s = fread(buff, 1, size, fp);
    fclose(fp);
    return buff;
}


