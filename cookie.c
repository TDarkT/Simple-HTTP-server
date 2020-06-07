#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netdb.h>
#include<unistd.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<errno.h>
#include "cookie.h"

struct hostent *host;


struct socket_t init_ckserver_sk(char *ip) {
    struct sockaddr_in ckserver_addr;
    host = gethostbyname(ip);
    if(host == NULL) {
        printf("Error: invalid ip for cookie server");
        exit(1);
    }

    memset(&ckserver_addr, 0, sizeof(ckserver_addr));
    ckserver_addr.sin_family = AF_INET;
    ckserver_addr.sin_addr = *(struct in_addr*) host->h_addr_list[0];
    ckserver_addr.sin_port = htons(8784);
     
    int ckserver_fd = socket(AF_INET, SOCK_STREAM, 0);
    struct socket_t soc = { .fd = ckserver_fd, .addr = ckserver_addr};
    return soc;
}

void set_cookie(char* buff) {
    buff = strtok(buff, "\r");
    buff[strlen(buff)] = ';';
    char *data = malloc(strlen(buff) + 4);
    strcat(data, "SET ");
    strcat(data, buff);
    struct socket_t cookie_serv = init_ckserver_sk("127.0.0.1");
    int ckserver_fd = cookie_serv.fd;
    struct sockaddr_in ckserver_addr = cookie_serv.addr;
    if (connect(ckserver_fd, (struct sockaddr *) &ckserver_addr, sizeof(ckserver_addr)) == 0) {
        if (setsockopt(ckserver_fd, SOL_SOCKET, SO_REUSEADDR, &(int){1}, 4) == -1);
            fprintf(stderr, "%s\n", strerror(errno));
        int fl = fcntl(ckserver_fd, F_GETFL, 0);
        fl |= O_NONBLOCK;
        fcntl(ckserver_fd, F_SETFL, fl);
        send(ckserver_fd, data, strlen(data),0);

    }
    shutdown(ckserver_fd, SHUT_RDWR);
    close(ckserver_fd);
}
cookie creat_cookie(char* str) {
    cookie client_cookie;
    char* key = strtok(str,"="); 

}

