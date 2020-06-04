#include "http.h"
#include <time.h>

struct http_header parse_http_req(char* buff) {
     struct http_header h;
     int length = 0;
     
     char* path = strtok(buff, "\n");
     h.request = strdup(path);
     path = path+4;
     
     while (*(path+length) != ' ') length++;
     h.path = (char*) malloc(length+1);
     strncpy(h.path, path, length);
     h.path[length] = '\0';
     //printf("%d %s\n",length, h.path);
     return h;
     
}

char* http_respone_200(char* data,int size) {
     time_t now;
     struct tm *ptm;
     char time_str[100];
     char *header =(char*) malloc(1000);
     time(&now);
     ptm = gmtime(&now);
      
     strftime(time_str, 100, "%a, %d %b %G %T GMT", ptm);
     sprintf(header, "HTTP/1.1 200 OK \nDate:%s\nServer:USTH\nContent-Length: %d\nContent-type: text/html\nConnection: Closed\n\n", time_str, size);
     return header;
}
