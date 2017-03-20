#include "verify.h"

char *build_str = "aabcccabbcccabcabc";
int n, l, m;

int *
rpcinitverifyserver_2_svc(verify_init_params *argp, struct svc_req *rqstp)
{
    n = 3;
    l = 6;
    m = 3;

	static int  result;

	/*
	 * insert server code here
	 */
    result = 5;

	return &result;
}

char **
rpcgetseg_2_svc(int *argp, struct svc_req *rqstp)
{
	static char * result;
    result = (char *)malloc(sizeof(char)*m*l);

    int lenS = strlen(build_str);

    if ((*argp)*l+l <= lenS) {
        /* Send substring from rank*l to rank*l + l*/
        memcpy(result, &build_str[(*argp)*l], l);
        result[l] = '\0';
    } else {
        printf("Thread rank too high\n");
    } 

	return &result;
}

char **
rpcgets_2_svc(void *argp, struct svc_req *rqstp)
{
	static char * result;

    result = build_str;

	return &result;
}
