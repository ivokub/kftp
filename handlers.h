/*
 * handlers.h
 *
 *  Created on: Nov 23, 2012
 *      Author: ivo
 */

#include "serve.h"
#ifndef HANDLERS_H_
#define HANDLERS_H_

	void build_handlers(void);

	void handler_fun_auth(client *, char *);

	typedef struct handler_entry handler_entry_t;
	typedef struct handler_entry {
		char cmd[64];
		void (*fun)(client *, char *);
		handler_entry_t * next;
	} handler_entry_t;

	handler_entry_t * handlers;
	void build_handler(void (*fun)(client *, char *), char *);


#endif /* HANDLERS_H_ */
