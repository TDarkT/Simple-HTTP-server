#ifndef FILE_LIB_H
#define FILE_LIB_H

int init_fserver_sk(char* ip);
int get_file_size(FILE *fp);
char *get_file_content(FILE *fp, int size);
char *get_file(char* filename);

#endif
