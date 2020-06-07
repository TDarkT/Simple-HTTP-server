#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>        // printf()
#include <string.h>       // memset()
#include <arpa/inet.h>    // close()
#include <unistd.h>       // close()
#include <sys/time.h>     // FD_SET(), FD_ISSET(), FD_ZERO(), FD_CLR()
#include <stdlib.h>       // atoi()
#include <sys/select.h>   // select()
#include <ctype.h>        // isprint()
#include "storage.h"

#define PORT 8784
#define MAXCONN 30
#define BUFSIZE 1024

/* Socket data */
int listenfd;
int serverfds[MAXCONN];
char buf[BUFSIZE];
socklen_t slen = sizeof(struct sockaddr_in);
struct sockaddr_in saddr;

char KEYNOTFOUND_MESSAGE[] = "Key not found";
char INVALIDREQ_MESSAGE[]  = "Invalid request";

int init_server() {
    ///////////////////////////////// COOKIE ///////////////////////////////////

    // Create socket
    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        return -1;

    // Setting address
    struct sockaddr_in saddr;
    memset(&saddr, 0, sizeof(saddr));
    saddr.sin_family      = AF_INET;
    saddr.sin_addr.s_addr = htonl(INADDR_ANY);
    saddr.sin_port        = htons(PORT);

    // Enable address reuse
    int opt = 1;
    if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt)) < 0) {
        printf("[!] Unable to allow multiple connections!\n");
        return -1;
    }

    // Binding...
    if (bind(listenfd, (struct sockaddr*) &saddr, sizeof(saddr)) < 0) {
        printf("[!] Cannot bind!\n");
        return -1;
    }

    // Listening...
    if (listen(listenfd, MAXCONN) < 0) {
        printf("[!] Cannot listen!\n");
        return -1;
    }
    
    printf("Listening on port %d\n", PORT);
    ///////////////////////////////// SERVER ///////////////////////////////////
    for (int i = 0; i < MAXCONN; ++i)
        serverfds[i] = 0;

    return 0;
}

void handle_request(int sockfd) {
    // Receive data
    char buf[SIZE];
    memset(buf, 0, SIZE);
    if (read(sockfd, buf, SIZE) <= 0) {
        printf("[] HTTP Server disconnected.\n");
        return;
    }

    // Check for instruction
    if (!((buf[0] == 'S' || buf[0] == 'G') && buf[1] == 'E' && buf[2] == 'T')) {
        write(sockfd, INVALIDREQ_MESSAGE, sizeof(INVALIDREQ_MESSAGE));
        return;
    }
    printf("%s\n", buf);
    // Execute instruction
    /* Set */
    /* Format: SET <key0>=<value0>; <key1>=<value1>; ... */
    if (buf[0] == 'S') {
        printf("[] SET request\n");
        char key[SIZE], value[SIZE];
        int l = 4, r;
        int stage = 0;
        while (1) {
            // Get end-pos and copy string...
            r = l;
            while (r < SIZE && ((buf[r] != '=' && stage == 0) || (buf[r] != ';' && stage == 1)))
                r++;
            if      (stage == 0) memset(key,   0, SIZE), memcpy(key,   buf + l, r - l);
            else if (stage == 1) memset(value, 0, SIZE), memcpy(value, buf + l, r - l);

            // Set value to key & write to file in case of shutdown...
            if (stage == 1)
                set(key, value),
                printf("[] SET success for (%s=%s)!\n", key, value);

            // Go to next stage...
            stage = (stage + 1) % 2;

            // Get begin-pos for next...
            l = r;
            while (l < SIZE && (buf[l] == '=' || buf[l] == ';' || buf[l] == ' ')) {
                if (buf[l] == ';') {
                    if (l + 1 < SIZE && (buf[l + 1] == '\n' || !isprint(buf[l + 1])) ) return;
                    else if (l + 1 == SIZE) return;
                }
                l++;
            }
        }
    }
    /* Get */
    /* Format: GET <key>; */
    else {
        printf("[] GET request\n");
        char key[SIZE], value[SIZE];

        // Get key
        int l = 4, r = 4;
        while (r < SIZE && buf[r] != ';' && buf[r] != '\0' && buf[r] != '\n')
            r++;
        memset(key, 0, SIZE); memcpy(key, buf + l, r - l);

        // Get value
        printf("[] Get value of key \'%s\':\n", key);
        memset(value, 0, SIZE);
        get(key, value);
        if (strlen(value) != 0) {
            printf("[] GET request success! Key value: \'%s\'\n", value),
            write(sockfd, value, SIZE);
            return;
        } else write(sockfd, value, 1);
        printf("[] GET request ERROR: not found.\n");
    }
}

void handle_new_connection() {
    int newsockfd = accept(listenfd, (struct sockaddr*) &saddr, &slen);
    if (newsockfd < 0)
        printf("[!] Error accepting socket!\n");
    else for (int i = 0; i < MAXCONN; ++i)
        if (serverfds[i] == 0) {
            serverfds[i] = newsockfd;
            printf("[] New HTTP Server connected!\n");
            return;
        }
}

void run_server() {
    fd_set readfds;
    int    maxfd;

    while (1) {
        ///// Reset & set the socket set
        FD_ZERO(&readfds);
        maxfd = listenfd;
        FD_SET(listenfd, &readfds);
        for (int i = 0; i < MAXCONN; ++i)
            if (serverfds[i] > 0) {
                if (serverfds[i] > maxfd)
                    maxfd = serverfds[i];
                FD_SET(serverfds[i], &readfds);
            }

        ///// Wait for activity
        select(maxfd + 1, &readfds, NULL, NULL, NULL);

        ///// New connection...
        if (FD_ISSET(listenfd, &readfds))
            handle_new_connection();

        ///// Server says something....
        for (int i = 0; i < MAXCONN; ++i)
            if (serverfds[i] > 0 && FD_ISSET(serverfds[i], &readfds)) {
                // Handle request from server
                handle_request(serverfds[i]);

                // Close connection...
                close(serverfds[i]);
                serverfds[i] = 0;
            }
    }
}

int main(int argc, char** argv) {
    if (init_server() < 0)
        return -1;

    run_server();
}
