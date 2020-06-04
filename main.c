#include "server.h"
#include <pthread.h>
#include <sys/select.h>
#include <fcntl.h>
#include <stdio.h>

int signal = 1;
int main() {
    int server_fd = server_init();
    int fl = fcntl(server_fd, F_GETFL, 0);
    fl |= O_NONBLOCK;
    fcntl(server_fd, F_SETFL, fl);
    while (1) {
        fd_set set;
	FD_ZERO(&set);
        FD_SET(server_fd, &set);
	select(server_fd+1, &set, NULL, NULL, NULL); 
        if (FD_ISSET(server_fd, &set)) {
            signal = 1;
            pthread_t t;
            int res;
	    res = pthread_create(&t, NULL, client_connect, NULL);
            while (signal) {};
        }
    } 
    return 0;
}
