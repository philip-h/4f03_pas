/*
 * This is sample code generated by rpcgen.
 * These are only templates and you can use them
 * as a guideline for developing your own functions.
 */

#include "append.h"
#include "verify.h"


void
verify_prog_2(char *host)
{
	CLIENT *clnt;
	int  *result_1;
	verify_init_params  rpcinitverifyserver_2_arg;
	char * *result_2;
	int  rpcgetseg_2_arg;
	char * *result_3;
	char *rpcgets_2_arg;

#ifndef	DEBUG
	clnt = clnt_create (host, VERIFY_PROG, VERIFY_VERS, "udp");
	if (clnt == NULL) {
		clnt_pcreateerror (host);
		exit (1);
	}
#endif	/* DEBUG */

	result_1 = rpcinitverifyserver_2(&rpcinitverifyserver_2_arg, clnt);
	if (result_1 == (int *) NULL) {
		clnt_perror (clnt, "call failed");
	}
	result_2 = rpcgetseg_2(&rpcgetseg_2_arg, clnt);
	if (result_2 == (char **) NULL) {
		clnt_perror (clnt, "call failed");
	}
	result_3 = rpcgets_2((void*)&rpcgets_2_arg, clnt);
	if (result_3 == (char **) NULL) {
		clnt_perror (clnt, "call failed");
	}
#ifndef	DEBUG
	clnt_destroy (clnt);
#endif	 /* DEBUG */
}

void
append_prog_1(char *host)
{
	CLIENT *clnt;
	int  *result_1;
	append_init_params  rpcinitappendserver_1_arg;
	int  *result_2;
	char  rpcappend_1_arg;

#ifndef	DEBUG
	clnt = clnt_create (host, APPEND_PROG, APPEND_VERS, "udp");
	if (clnt == NULL) {
		clnt_pcreateerror (host);
		exit (1);
	}
#endif	/* DEBUG */

	result_1 = rpcinitappendserver_1(&rpcinitappendserver_1_arg, clnt);
	if (result_1 == (int *) NULL) {
		clnt_perror (clnt, "call failed");
	}
	result_2 = rpcappend_1(&rpcappend_1_arg, clnt);
	if (result_2 == (int *) NULL) {
		clnt_perror (clnt, "call failed");
	}
#ifndef	DEBUG
	clnt_destroy (clnt);
#endif	 /* DEBUG */
}


int
main (int argc, char *argv[])
{
	char *host;

	if (argc < 2) {
		printf ("usage: %s server_host\n", argv[0]);
		exit (1);
	}
	host = argv[1];
	verify_prog_2 (host);
    append_prog_1 (host);
exit (0);
}
