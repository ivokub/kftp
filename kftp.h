/*
 * kftp.h
 *
 *  Created on: Nov 18, 2012
 *      Author: ivo
 */

#define FORK 0x1
#define VERBOSE 0x2

#ifndef KFTP_H_
#define KFTP_H_
	char * currenttime();
	char timebuf[64];
	#define LOG(...) 	fprintf(log_output, "%s ", currenttime()); \
						fprintf (log_output, __VA_ARGS__)
	#define ERR(...) 	fprintf(err_output, "%s ", currenttime()); \
						fprintf (err_output, __VA_ARGS__)

	int main(int, char**);
	void parse_args(int, char**);
	int daemonize();

	int opts;
	FILE * log_output;
	FILE * err_output;
#endif /* KFTP_H_ */
