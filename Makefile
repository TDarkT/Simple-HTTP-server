all: http_server file_server cookie_server

http_server: main.c server.c http.c cookie.c file_lib.c
	gcc -o http_server  main.c server.c http.c cookie.c file_lib.c -pthread
file_server: fileserver.c
	gcc -o file_server fileserver.c
cookie_server: cookie-server.c
	gcc -o cookie_server cookie-server.c
