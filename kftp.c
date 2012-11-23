#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>

#include "kftp.h"
#include "serve.h"

void parse_args(int argc, char **argv) {
	opts = 0;
	int opt;
	while ((opt = getopt(argc, argv, "fvh")) != -1) {
		switch (opt) {
		case 'f':
			opts|=FORK;
			break;
		case 'v':
			opts|=VERBOSE;
			break;
		case 'h':
			printf("Simple FTP server.\n\n"
				"Usage:\tkftp [-f] [-v]\n\n"
				"Argument usage:\n"
				"\t-f\t\tDaemonize\n"
				"\t-v\t\tVerbose output\n");
			exit(0);
			break;
		default:
			break;
		}
	}
}

int main(int argc, char **argv) {
	parse_args(argc, argv);
	log_output = stdout;
	err_output = stderr;
	if (opts&FORK){
		if (daemonize()){
			error(1,0,"Fork failed");
		}
	}
	if (serve()) {
		error(1,0, "Server exited unexpectedly, error code: %d", serve_ERRNO);
	} else {
		printf("Quitted. Bye!\n");
	}
	return 0;

}

int daemonize() {
	return 0;
}
