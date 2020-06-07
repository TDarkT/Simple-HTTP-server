http_server: main.c server.c http.c cookie.c file_lib.c
	gcc -o http_server  main.c server.c http.c cookie.c file_lib.c
file_server: fileserver.c
	gcc -o file_server fileserver.c
