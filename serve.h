/*
 * serve.h
 *
 *  Created on: Nov 18, 2012
 *      Author: ivo
 */

#define PORT "21"
#define HOST 0.0.0.0
#define BACKLOG 10

#ifndef SERVE_H_
#define SERVE_H_
	int serve();
	int shutdown_handler(int);
	serv_conf * read_conf(char *);
	int create_serv_socket(serv_conf *);
	int serve_ERRNO;
	typedef struct servconf {
		char port[6];
		int backlog;
	} serv_conf;
	int sock;


#endif /* SERVE_H_ */
