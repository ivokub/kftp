/*
 * handlers.c
 *
 *  Created on: Nov 23, 2012
 *      Author: ivo
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "handlers.h"
#include "kftp.h"

void handler_fun_user(client * this_client, char * args) {
	fprintf(log_output, "Client %d user %s\n", this_client->id, args);
	send(((client *) this_client)->sock,"331 OK\n",7,0);
	strncpy(this_client->session.user, args, 63);
}

void handler_fun_pass(client * this_client, char * args) {
	fprintf(log_output, "Client %d pass %s\n", this_client->id, args);
	send(((client *) this_client)->sock,"230 OK\n",7,0);
	strncpy(this_client->session.pass, args, 63);
	this_client->session.auth = 1;
}

void handler_fun_pwd(client * this_client, char * args) {
	send(((client *) this_client)->sock, "257 \"/\"\n", 8, 0);
	strncpy(this_client->session.pwd, "/", 2);
}

void handler_fun_type(client * this_client, char * args) {
	send(((client *) this_client)->sock, "200 TYPE\n", 9, 0);
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
	fprintf(log_output, "Built %s handler\n", CMD);
}

void build_handlers() {
	build_handler(handler_fun_user, "USER");
	build_handler(handler_fun_pass, "PASS");
	build_handler(handler_fun_pwd, "PWD");
	build_handler(handler_fun_type, "TYPE");

}
