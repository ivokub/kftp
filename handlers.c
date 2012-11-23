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

void handler_fun_auth(client * this_client){

}

void build_handlers() {
	handler_entry_t * handler_auth = malloc(sizeof(handler_entry_t));
	strcpy(handler_auth->cmd, "USER");
	handler_auth->fun = handler_fun_auth;
	fprintf(log_output, "Built authentication handler\n");


}
