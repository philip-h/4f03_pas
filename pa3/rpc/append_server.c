#include "append.h"

int f;
int l;
int m;
char c0;
char c1;
char c2;
char *host_verify;

char *build_str;

int *
rpcinitappendserver_1_svc(append_init_params *argp, struct svc_req *rqstp)
{
	static int  result;

	f = argp->f;
	l = argp->l;
	m = argp->m;
	c0 = argp->c0;
	c1 = argp->c1;
	c2 = argp->c2;
	host_verify = argp->host_verify;

    build_str = (char *)malloc(sizeof(char) * m * l);
    build_str[0] = '\0';

	result = 1;

	return &result;
}

int *
rpcappend_1_svc(char *argp, struct svc_req *rqstp)
{
	static int  result;

	result = -1;

	int length = strlen(build_str);

	if(length < m*l)
	{
		build_str[length] = *argp;
		build_str[length+1] = '\0';
		result = 0;
	}

	return &result;
}
