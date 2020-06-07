#include "http.h"
#include "cookie.h"
#include <time.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>

struct http_header parse_http_req(char* buff,struct sockaddr_in addr) {
     struct http_header h;
     int length = 0;
     h.ip = inet_ntoa(addr.sin_addr);
     char* header;
     char* headers[30];
     int i = 0;
     header = strtok(buff, "\n");
     while (header !=NULL) {
         headers[i++] = header;
         header = strtok(NULL, "\n");
     }
     while (*(headers[0]+4+length) != ' ') length++;
     h.request = headers[0];
     h.request[strlen(h.request) - 1] ='\0';
     h.path = (char*) malloc(length+1);
     strncpy(h.path, h.request+4, length);
     h.path[length] = '\0';
     
     return h;
     
}
struct tm *get_current_time() {
     time_t now;
     struct tm *ptm;
     time(&now);
     ptm = gmtime(&now);
     return ptm;

}

void http_logging(struct http_header req, struct tm *t, int size, int status) {
    char time_str[100];
    strftime(time_str,100, "%d/%b/%Y:%H:%M:%S %z", t);
    printf("%s [%s] \"%s\" %d %d\n",req.ip, time_str, req.request,status, size); 
}

char* http_respone_200(struct http_header req, char* data,int size) {
     struct tm *ptm = get_current_time();
     char time_str[100];
     char *header = malloc(1000);
     strftime(time_str, 100, "%a, %d %b %G %T GMT", ptm);
     
     sprintf(header, "HTTP/1.1 200 OK \nDate:%s\nServer:USTH\nContent-Length: %d\nContent-type: text/html\nConnection: Closed\n\n", time_str, size);
     
     header = realloc(header, strlen(header)+size+1);
     http_logging(req, ptm, strlen(header)+size, 200);
     return header;
}

char* http_respone_404(struct http_header req, char* data,int size) {
     struct tm *ptm = get_current_time();
     char time_str[100];
     char *header = malloc(1000);
     strftime(time_str, 100, "%a, %d %b %G %T GMT", ptm);
     
     sprintf(header, "HTTP/1.1 404 Not Found \nDate:%s\nServer:USTH\nContent-Length: %d\nContent-type: text/html\nSet-Cookie: name=USTH; Max-Age=86400\nConnection: Closed\n\n", time_str, size);
     
     header = realloc(header, strlen(header)+size+1);
     http_logging(req, ptm,strlen(header)+size, 404);
     return header;
}


