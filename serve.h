/*
 * serve.h
 *
 *  Created on: Nov 18, 2012
 *      Author: ivo
 */

#include <sys/socket.h>

#define PORT 21000
#define HOST 0.0.0.0
#define BACKLOG 10

#ifndef SERVE_H_
#define SERVE_H_
	int serve_ERRNO;
	typedef struct servconf {
		int backlog;
		int port;
	} serv_conf;
	int sock;

	int serve();
	void shutdown_handler(int);
	serv_conf * read_conf(char *);
	int create_serv_socket(serv_conf *);
	int start_ftp_server(int);
	void client_handle(int *, struct sockaddr *);
#endif /* SERVE_H_ */
