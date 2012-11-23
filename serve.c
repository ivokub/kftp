#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <pthread.h>
#include "serve.h"
#include "kftp.h"
#include "handlers.h"


void shutdown_handler(int signal) {
	close(sock);
	if (clients != NULL)
	fprintf(log_output, "Caught signal. Quitting\n");
	exit(0);
}

int serve() {
	serve_ERRNO = 0;
	serv_conf * main_conf = malloc(sizeof(serv_conf));
	main_conf = read_conf("dummylocation");
	if ((sock = create_serv_socket(main_conf)) == -1){
		serve_ERRNO = 1;
		return 1;
	}

	fprintf(log_output, "Created listening socket at %d\n", main_conf->port);
	signal(15, shutdown_handler);
	signal(2, shutdown_handler);
	fprintf(log_output, "Created signal handlers\n");
	start_ftp_server(sock, main_conf);
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
			continue;
		}
		if (bind(sock, resptr->ai_addr, resptr->ai_addrlen) == 0){
			listen(sock, main_conf->backlog);
			return sock;
		}
	}
	close(sock);
	return -1;

}

serv_conf * read_conf(char * conf_location) {
	serv_conf * main_conf = malloc(sizeof(serv_conf));
	memset(main_conf, 0, sizeof(main_conf));
	main_conf->port = PORT;
	main_conf->backlog = BACKLOG;
	main_conf->maxconn = MAXCONN;
	return main_conf;
}

int start_ftp_server(int server_sock, serv_conf * main_conf) {
	struct sockaddr_in netaddress;
	client * this_client;
	clients = NULL;
	clientcount = 0;
	build_handlers();
	while (1) {
		this_client = assign_client(server_sock);
		if (this_client->sock == -1) {
			fprintf(err_output, "Client socket failed\n");
			close(this_client->sock);
		} else {
			thread_client_handle(this_client);
		}
	}
	return 1;
}

client * assign_client(int server_sock) {
	socklen_t addrlen = sizeof(struct sockaddr);
	client * this_client = malloc(sizeof(client));
	this_client->id = ++clientcount;
	this_client->sock = accept(server_sock, (struct sockaddr *) &(this_client->addr), &addrlen);
	fprintf(log_output, "Client %d connected from %s:%d\n", clientcount, inet_ntoa(this_client->addr.sin_addr), (int) this_client->addr.sin_port);
	return this_client;
}

pthread_t * thread_client_handle(client * this_client){
	pthread_t * current_thread = malloc(sizeof(pthread_t));
	if (pthread_create(current_thread, NULL, client_handle_middle, (void *) this_client)) {
		return NULL;
	} else {
		add_client(this_client);
		return current_thread;
	}
}

int add_client(client * connected_client) {
	int i = 0;
	client * rp;
	if (clients == NULL) {
		clients = connected_client;
		return 0;
	} else {
		for (rp = clients; rp->next_client != NULL; rp = rp->next_client)
			i++;
		rp->next_client = connected_client;
		return i;
	}
}

void remove_client(client * connected_client) {
	int i = 0;
	client * rp = clients;
	close(connected_client->sock);
	if (rp == connected_client) {
		clients = rp->next_client;
		free(connected_client);
	}
	while (rp->next_client != NULL) {
		if (rp->next_client == connected_client){
			rp->next_client = rp->next_client->next_client;
			free(connected_client);
		}
	}
}

void * client_handle_middle(void * this_client){
	client_handle((client *) this_client);
	return NULL;
}

void client_handle(client * this_client) {
	int recvd = 0;
	char buffer[1024];
	memset(buffer, 0, 1024);
	send(this_client->sock, "220 Welcome\n", 12, 0);
	while ((recvd = recv(this_client->sock, buffer, sizeof(buffer)/sizeof(char), 0)) > 0) {
		command_parser(this_client, buffer);
	}
	if (recvd == -1) {
		fprintf(err_output, "Client %d socket broken\n", this_client->id);
	} else {
		fprintf(log_output, "Client %d closed connection\n", this_client->id);
	}
	remove_client(this_client);
}

void command_parser(client * this_client, char * command) {
	fprintf(log_output, "Client %d command: %s", this_client->id, command);

	send(((client *) this_client)->sock,command,strlen(command),0);
}
