#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <netdb.h>
#include <unistd.h>
struct sockaddr_in fserver_addr;
struct hostent *host;
int fserver_fd;

int init_fserver_sk(char *ip) {
    host = gethostbyname(ip);
    if (host == NULL) {
        printf("Error: invalid ip for file server");
        exit(1);
    }
    memset(&fserver_addr, 0, sizeof(fserver_addr));
    fserver_addr.sin_family = AF_INET;
    fserver_addr.sin_addr = *(struct in_addr *)host->h_addr_list[0];
    fserver_addr.sin_port = htons(23);

    fserver_fd = socket(AF_INET, SOCK_STREAM, 0);
    
    return fserver_fd;
}

char *get_file(char* filename) {
    char* file_data = NULL; 
    printf("%s\n", filename);
    if (connect(fserver_fd, (struct sockaddr *) &fserver_addr, sizeof(fserver_addr)) == 0) {
        send(fserver_fd, filename, strlen(filename), 0);
        int file_size;
        recv(fserver_fd,(char*)&file_size, 4, 0);
        if (file_size != 0) {
            file_data = (char *) malloc(file_size);
            char buff[4096];
            while( recv(fserver_fd, buff, 4096, 0) > 0 ) {
                strcat(file_data, buff);
                bzero(buff, 4096);
            }
        }
    }
    shutdown(fserver_fd, SHUT_RDWR);
    close(fserver_fd);
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


