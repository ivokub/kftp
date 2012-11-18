#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <signal.h>
#include "serve.h"


void shutdown_handler(int signal) {
	close(sock);
	fprintf(log_output, "Caught signal. Quitting");
	exit(0);
}

int serve() {
	serve_ERRNO = 0;
	struct servconf;
	serv_conf * main_conf = reaf_conf("dummylocation");
	if ((sock = create_serv_socket(main_conf)) == -1)
		return 1;
	signal(15, shutdown_handler);
	start_ftp_server(sock);

	return 0;
}

int create_serv_socket(serv_conf * main_conf) {
	int sock;
	struct addrinfo hints;
	struct addrinfo *results, *resptr;
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags=0;
	hints.ai_protocol=0;
	hints.ai_canonname = NULL;
	hints.ai_addr = NULL;
	hints.ai_next = NULL;
	if (getaddrinfo(NULL, main_conf->port, &hints, &results)) {
		fprintf(err_output, "Address error\n");
		return -1;
	}
	for (resptr = results; resptr != NULL; resptr = resptr->ai_next) {
	sock = socket(resptr->ai_family, resptr->ai_socktype, resptr->ai_protocol);
	if (sock == -1) {
		fprintf(err_output, "Socket error\n");
		return -1;
	}
	if (bind(sock, resptr->ai_addr, resptr->ai_addrlen) == 0)
	  listen(sock, main_conf->backlog);
	  return sock;
	close(sock);
	}
	return -1;

}

serv_conf * read_conf(char * conf_location) {
	serv_conf * main_conf = malloc(sizeof(main_conf));
	main_conf->port = PORT;
	main_conf->backlog = BACKLOG;
	return main_conf;
}
