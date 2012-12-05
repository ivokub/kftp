/*
 * serve.h
 *
 *  Created on: Nov 18, 2012
 *      Author: ivo
 */

#include <sys/socket.h>
#include <netinet/in.h>

#define PORT 21000
#define HOST 0.0.0.0
#define BACKLOG 10
#define MAXCONN 10

#ifndef SERVE_H_
#define SERVE_H_
	int serve_ERRNO;

	typedef struct session {
		char user[64];
		char pass[64];
		int auth;
		char pwd[256];
		int data_sock;
		struct sockaddr_in data_addr;
	} session_t;
	typedef struct servconf {
		int backlog;
		int port;
		int maxconn;
		char interface[6];
	} serv_conf;
	typedef struct client_s client;
	typedef struct client_s {
		int id;
		int sock;
		client * next_client;
		struct sockaddr_in addr;
		session_t session;
	} client;

	int sock;
	client * clients;
	int clientcount;
	struct sockaddr * serve_addr;

	int serve();
	serv_conf * read_conf(char *);
	struct sockaddr * local_addr(char *);
	int create_serv_socket(serv_conf *);
	int start_ftp_server(int, serv_conf *);

	void shutdown_handler(int);

	client * assign_client(int);
	int add_client(client *);
	void remove_client(client *);

	void * client_handle_middle(void *);
	void client_handle(client *);
	pthread_t * thread_client_handle(client *);
	void command_parser(client *, char *);


#endif /* SERVE_H_ */
