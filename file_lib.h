#ifndef FILE_LIB_H
#define FILE_LIB_H
#include <stdio.h>
#include <netdb.h>
struct socket_t {
    int fd;
    struct sockaddr_in addr;
};
struct file {
    int size;
    char *data;
};
struct socket_t init_fserver_sk(char* ip);
int get_file_size(FILE *fp);
char *get_file_content(FILE *fp, int size);
struct file get_file(char* filename);

#endif
