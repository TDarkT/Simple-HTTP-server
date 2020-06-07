#include <stdlib.h>  // malloc()
#include <string.h>  // memcpy(), strcmp()
#include <sys/stat.h> // mkdir()
#include <stdio.h>   // fopen()

#define SIZE 1024

void set(char* key, char* value) {
    // Create a cookie folder if not exist...
    mkdir("cookies", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    // Format string to specify destination...
    char filename[SIZE + 10]; sprintf(filename, "cookies/%s", key);
    // Open file...
    FILE* logfile = fopen(filename, "w");
    // Log to file...
    fputs(value, logfile);
    // Closing file...
    fclose(logfile);
}

void get(char* key, char* value) {
    // Format string to specify destination...
    char filename[SIZE + 10]; sprintf(filename, "cookies/%s", key);
    // Open file...
    FILE* logfile = fopen(filename, "r");
    if (!logfile)
        return;
    // Log to file...
    fgets(value, SIZE, logfile);
    // Closing file...
    fclose(logfile);
}
