/*
 * handlers.c
 *
 *  Created on: Nov 23, 2012
 *      Author: ivo
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <poll.h>
#include "handlers.h"
#include "serve.h"
#include "kftp.h"

int create_data_sock(client * this_client) {
	socklen_t addrlen = sizeof(struct sockaddr);
	serv_conf * data_conf = malloc(sizeof(serv_conf));
	int clientsock;
	data_conf->backlog = 1;
	data_conf->maxconn = 1;
	data_conf->port = 0;
	this_client->session.data_sock = create_serv_socket(data_conf);
	if (this_client->session.data_sock == -1)
		return -1;
	getsockname(this_client->session.data_sock, (struct sockaddr *) &(this_client->session.data_addr), &addrlen);
	free(data_conf);
	return 0;
}

void handler_fun_user(client * this_client, char * args) {
	LOG("Client %d user %s\n", this_client->id, args);
	strncpy(this_client->session.user, args, 63);
	send(((client *) this_client)->sock,"331 OK\n",7,0);
}

void handler_fun_pass(client * this_client, char * args) {
	LOG("Client %d pass %s\n", this_client->id, args);
	strncpy(this_client->session.pass, args, 63);
	send(((client *) this_client)->sock,"230 OK\n",7,0);
	this_client->session.auth = 1;
}

void handler_fun_pwd(client * this_client, char * args) {
	char msg[260];
	sprintf(msg, "257 \"%s\"\n", this_client->session.pwd);
	send(((client *) this_client)->sock, msg, strlen(msg), 0);
}

void handler_fun_type(client * this_client, char * args) {
	send(((client *) this_client)->sock, "200 TYPE\n", 9, 0);
}

void handler_fun_acct(client * this_client, char * args) {
	send(this_client->sock, "230 OK\n",7,0);
}

void handler_fun_syst(client * this_client, char * args) {
	send(this_client->sock, "215 UNIX Type: L8\n", 18, 0);
}

void bg_accept(client * this_client) {
	struct sockaddr tempaddr;
	socklen_t addrlen;
	struct pollfd interesting;
	interesting.fd = this_client->session.data_sock;
	interesting.events = POLLIN;
	if (poll(&interesting, (nfds_t) 1, 1000)>0) {
		this_client->session.data_sock = accept(this_client->session.data_sock, &tempaddr, &addrlen);
	} else {
		close(this_client->session.data_sock);
		this_client->session.data_sock = -1;
	}
}

void handler_fun_pasv(client * this_client, char * args) {
	char buf[64];
	memset(buf, 0, 64);
	int port;
	int clientsock;
	struct sockaddr tempaddr;
	pthread_t current_thread;
	socklen_t addrlen;
	clientsock = create_data_sock(this_client);
	port = (int) this_client->session.data_addr.sin_port;
	sprintf(buf, "227 =127,0,0,1,%d,%d\n", port%256, port/256);
	LOG("Socket is on port %d\n", port);
	pthread_create(&current_thread, NULL, bg_accept, this_client);
	send(this_client->sock, buf, strlen(buf),0);
	pthread_join(current_thread, NULL);
	if (this_client->session.data_sock == -1) {
		send(this_client->sock, "426 Broken\n", 11, 0);
	} else {
		send(this_client->sock, "150 Accepted\n", 13, 0);
	}
}

void handler_fun_list(client * this_client, char * args) {
	int status;
	if (this_client->session.data_sock == -1){
		send(this_client->sock, "425 No socket\n", 14, 0);
	} else {
		if (fork() == 0) {
			LOG("Sending content of %s", args);
			dup2(this_client->session.data_sock, 1);
			dup2(this_client->session.data_sock, 2);
			if (args[0]) {
				execl("/bin/ls", "ls", "-l", args, NULL);
			} else {
				execl("/bin/ls", "ls", "-l", this_client->session.pwd, NULL);
			}
		} else {
			wait(&status);
			close(this_client->session.data_sock);
			this_client->session.data_sock = -1;
			send(this_client->sock, "226 Sent\n", 9, 0);
			LOG("Listing sent\n");
		}
	}
}

void handler_fun_quit(client * this_client, char * args) {
	close(this_client->sock);
}

void handler_fun_cwd(client * this_client, char * args) {
	strncpy(this_client->session.pwd, args, 255);
	send(this_client->sock, "250 OK\n", 7, 0);
}

void build_handler(void (*fun)(client *, char *), char * CMD) {
	handler_entry_t * handler_name = malloc(sizeof(handler_entry_t));
	handler_entry_t * hp;
	if (handlers == NULL){
		handlers = handler_name;
	} else {
		for (hp = handlers; hp->next != NULL; hp = hp->next);
		hp->next = handler_name;
	}
	strcpy(handler_name->cmd, CMD);
	handler_name->fun = fun;
	handler_name->next = NULL;
	LOG("Built %s handler\n", CMD);
}



void build_handlers() {
	build_handler(handler_fun_user, "USER");
	build_handler(handler_fun_pass, "PASS");
	build_handler(handler_fun_pwd, "PWD");
	build_handler(handler_fun_type, "TYPE");
	build_handler(handler_fun_acct, "ACCT");
	build_handler(handler_fun_syst, "SYST");
	build_handler(handler_fun_pasv, "PASV");
	build_handler(handler_fun_list, "LIST");
	build_handler(handler_fun_quit, "QUIT");
	build_handler(handler_fun_cwd, "CWD");
}
