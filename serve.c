#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include "serve.h"
#include "kftp.h"


void shutdown_handler(int signal) {
	close(sock);
	fprintf(log_output, "Caught signal. Quitting\n");
	exit(0);
}

int serve() {
	serve_ERRNO = 0;
	serv_conf * main_conf = malloc(sizeof(serv_conf));
	main_conf = read_conf("dummylocation");
	if ((sock = create_serv_socket(main_conf)) == -1)
		return 1;
	fprintf(log_output, "Created listening socket at %d\n", main_conf->port);
	signal(15, shutdown_handler);			// No handlers before functional socket or exit
	signal(2, shutdown_handler);
	fprintf(log_output, "Created handlers\n");
	start_ftp_server(sock);

	close(sock);
	return 0;
}

int create_serv_socket(serv_conf * main_conf) {
	int sock;
	struct addrinfo hints;
	struct addrinfo *results, *resptr;
	char port[6];
	sprintf(port, "%d", main_conf->port);
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags=0;
	hints.ai_protocol=0;
	hints.ai_canonname = NULL;
	hints.ai_addr = NULL;
	hints.ai_next = NULL;
	if (getaddrinfo(NULL, port, &hints, &results)) {
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
	serv_conf * main_conf = malloc(sizeof(serv_conf));
	memset(main_conf, 0, sizeof(main_conf));
	main_conf->port = PORT;
	main_conf->backlog = BACKLOG;
	return main_conf;
}

int start_ftp_server(int server_sock) {
	socklen_t addrlen = sizeof(struct sockaddr);
	int * connecting_sock = malloc(sizeof(int));
	struct sockaddr * client = malloc(sizeof(struct sockaddr));
	while (1) {
		*connecting_sock = accept(server_sock, client, &addrlen);
		fprintf(log_output, "Client connected from\n");
		if (*connecting_sock == -1) {
			fprintf(err_output, "Client socket failed\n");
			close(*connecting_sock);
		} else {
			client_handle(connecting_sock, client);
		}
	}
	return 1;
}

void client_handle(int * csock, struct sockaddr * client) {
	int recvd = 0;
	char buffer[1024];
	while ((recvd = recv(*csock, buffer, sizeof(buffer)/sizeof(char), 0)) > 0) {
		send(*csock,buffer,recvd,0);
	}
	if (recvd == -1) {
		fprintf(err_output, "Client socket broken\n");
	} else {
		fprintf(log_output, "Client closed connection\n");
	}
	close(*csock);
	free(csock);
	free(client);
}
