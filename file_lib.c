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
#include "file_lib.h"
struct hostent *host;

struct socket_t init_fserver_sk(char *ip) {
    host = gethostbyname(ip);
    if (host == NULL) {
        printf("Error: invalid ip for file server");
        exit(1);
    }
    struct sockaddr_in fserver_addr;
    memset(&fserver_addr, 0, sizeof(fserver_addr));
    fserver_addr.sin_family = AF_INET;
    fserver_addr.sin_addr = *(struct in_addr *)host->h_addr_list[0];
    fserver_addr.sin_port = htons(8080);

    int fserver_fd = socket(AF_INET, SOCK_STREAM, 0);
    struct socket_t soc = { .fd = fserver_fd, .addr = fserver_addr };
    return soc;
}

struct file get_file(char* filename) {
    struct file f;
    char* file_data = NULL; 
    struct socket_t fserver_soc = init_fserver_sk("127.0.0.1");
    int fserver_fd = fserver_soc.fd;
    struct sockaddr_in fserver_addr = fserver_soc.addr;
    if (connect(fserver_fd, (struct sockaddr *) &fserver_addr, sizeof(fserver_addr)) == 0) {
        send(fserver_fd, filename, strlen(filename), 0);
        int file_size;
        recv(fserver_fd,(char*)&file_size, 4, 0);
        if (file_size != 0) {
            char *buff = calloc(1, 4096);
            if (file_size > 0) {
                file_data = (char *) calloc(1, file_size);
                int i;
                int pos = 0;
                while((i = recv(fserver_fd, buff, 1024, 0)) > 0 ) {
                    memcpy(file_data+pos, buff, i);
                    memset(buff, 0 , 1024);
                    pos +=i;
                } 
            }
        }
        f.size = file_size;
        f.data = file_data;
    }
    shutdown(fserver_fd, SHUT_RDWR);
    close(fserver_fd);
    return f;
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


