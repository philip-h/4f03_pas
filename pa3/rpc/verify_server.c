#include "verify.h"

#include<unistd.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<omp.h>

#define BUFLEN 512
#define PORT 9999

char *build_str = "aabcccabbcccabcabc";
int n, l, m;

void initPacktListener()
{
  struct sockaddr_in si_me, si_other;

  int s, i, slen = sizeof(si_other) , recv_len;
  char buf[BUFLEN];

  //Attempt to create a UDP socket
  if ((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
  {
      perror("Unable to create UDP socket!");
      exit(1);
  }

  memset((char *) &si_me, 0, sizeof(si_me));

  si_me.sin_family = AF_INET;
  si_me.sin_port = htons(PORT);
  si_me.sin_addr.s_addr = htonl(INADDR_ANY);

  //Attempt to bind socket to the port
  if( bind(s , (struct sockaddr*)&si_me, sizeof(si_me) ) == -1)
  {
      perror("Unable to bind socket to port");
      exit(1);
  }

    printf("Waiting for data...");
    fflush(stdout);

    //try to receive some data, this is a blocking call
    if ((recv_len = recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *) &si_other, &slen)) == -1)
    {
        perror("recvfrom()");
        exit(1);
    }

    //print details of the client/peer and the data received
    printf("Received packet from %s:%d\n", inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port));
    printf("Data: %s\n" , buf);

    close(s);

}

int *
rpcinitverifyserver_2_svc(verify_init_params *argp, struct svc_req *rqstp)
{
  l = argp->l;
  n = argp->n;
  m = argp->m;

	static int  result;

  #pragma omp parallel
  {
    initPacktListener();
  }


    result = 5;

    printf("AAAAAAAAAAAAAAAA RESULT = 5 !!!!");

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
        result[0] = '-';
        result[1] = '\0';
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
