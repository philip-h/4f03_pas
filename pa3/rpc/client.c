/*
Contributors: Philip Habib (habibp)
Theo Stone (stonet)
*/

#include "append.h"
#include "verify.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>
#include <omp.h>

/* Function Prototypes */
void initAppendServer(char*, int, int, int, char, char, char, char*);
void initVerifyServer(char*, int, int, int);

int checkArgs(int, int, int, int);
void *appendToS(void *);
int verifySegment(char *segment);
   
/* Global Vars */
CLIENT *clnt_append;
CLIENT *clnt_verify;

omp_lock_t countLock;

char **S;
int totalCount = 0;

int f, m, n, l;
char c0, c1, c2;

char *sigma;
int MILIS = 1000000;

const char *error = "Useage: ./pa2.x i N L M c0 c1 c2 host_name1 host_name2\n"
               "Where\n"
               "i\tthe index of the property F_i which each segment will satisfy\n"
               "N\tthe number of threads\n"
               "L\tthe length of each segment\n"
               "M\tthe number of segments to generate\n"
               "c_i\tthe letters to be used in the property check\n"
               "host_name1\tname of append server\n"
               "host_name2\tname of verify server\n";

int main (int argc, char *argv[])
{
    char* host_verify;
    char* host_append;

	if (argc < 10) {
		printf ("Too few arguments\n%s", error);
        return -1;
	}

    f = atoi(argv[1]);
    n = atoi(argv[2]);
    l = atoi(argv[3]);
    m = atoi(argv[4]);
    c0 = *argv[5];
    c1 = *argv[6];
    c2 = *argv[7];
    host_append = argv[8];
    host_verify = argv[9];

    if (checkArgs(f,n,l,m) != 0) {
        return -1;
    }

    /* Init our alphabet from a- [c-g] */
    sigma = (char*) malloc(sizeof(char)*n);
    for (size_t i = 0; i < n; i++) {
        sigma[i] = (char)(i + (int)'a');
    }

    initAppendServer(host_append, f, l, m, c0, c1, c2, host_verify);
    initVerifyServer(host_verify, n, l, m);

    /* Seed for random sleep time of the threads */
    srand(time(NULL));

    printf("Starting Threads ...\n");

    /* Spin up n threads */
#   pragma omp parallel num_threads(n)
    {
        appendToS(0);
    }

    /* Free memory of sigma */
    free(sigma);

    FILE *f = fopen("out.txt", "w");
    if (f == NULL) {
        printf("Error opening file\n");
        return -1;
    }

    fprintf(f, "%s\n%d\n", *S, totalCount);
    fclose(f);

    printf("%s\n%d\n", *S, totalCount);

    return 0;
}

/*
  Checks to make sure command line arguments don't break the program!
  Will print and return with an error if there is a problem with the arguments.
*/
int checkArgs(int f, int n, int l, int m)
{
    if (f < 0 || f > 3) {
        printf("F must be between 0 and 3\n%s", error);
        return -1;
    }

    if (n < 3 || n > 8) {
        printf("N must be between 3 and 8\n%s", error);
        return -1;
    }

    if (l < 0) {
        printf("L cannot be negative\n%s", error);
        return -1;
    }

    if (m < 0) {
        printf("M cannot be negative\n%s", error);
        return -1;
    }

    if(m % n != 0)
    {
        printf("M does not divide N\n%s", error);
        return -1;
    }

    return 0;
}

/* Initializes the append server!! */
void initAppendServer(char *host_append, int f, int l, int m, char c0, char c1, char c2, char* host_verify)
{
    int *initStatus;
    append_init_params append_params;

    clnt_append = clnt_create (host_append, APPEND_PROG, APPEND_VERS, "udp");
    if (clnt_append == NULL) {
		clnt_pcreateerror (host_append);
		exit(1);
	}

    append_params.f = f;
    append_params.l = l;
    append_params.m = m;
    append_params.c0 = c0;
    append_params.c1 = c1;
    append_params.c2 = c2;
    append_params.host_verify = host_verify;

    initStatus = rpcinitappendserver_1(&append_params, clnt_append);
	if (initStatus == (int *) NULL) {
		clnt_perror (clnt_append, "call failed");
	} else {
	    printf("Successfully initialized Append Server\n");
	}
}

/* Initializes the verify server!! */
void initVerifyServer(char *host_verify, int n, int l, int m)
{
    int *initStatus;
    verify_init_params verify_params;

	clnt_verify = clnt_create (host_verify, VERIFY_PROG, VERIFY_VERS, "udp");
    if (clnt_verify == NULL) {
		clnt_pcreateerror (host_verify);
		exit(1);
	}

    verify_params.n = n;
    verify_params.l = l;
    verify_params.m = m;

    initStatus = rpcinitverifyserver_2(&verify_params, clnt_verify);
	if (initStatus == (int *) NULL) {
		clnt_perror (clnt_verify, "call failed");
	} else {
        printf("Successfully initialized verify server\n");
	}
}

/* Method that is spun off into threads. 
 * Appends letters to a string on the server concurrently 
 */
void *appendToS(void *r)
{
    int rank = omp_get_thread_num();
    int *appendStatus;
    int localCount = 0;

    int randSleep, randSleepNano;
    struct timespec tim, tim2;
    tim.tv_sec  = 0;

    while(1){
        randSleep = rand() % 400 + 100;
        randSleepNano = randSleep * MILIS;

        tim.tv_nsec = randSleepNano;

        if(nanosleep(&tim , &tim2) < 0 )
        {
            printf("Nano sleep system call failed! \n");
        }
        char c = sigma[rank];

        appendStatus = rpcappend_1(&c, clnt_append);
        if (appendStatus == (int *)NULL) {
            clnt_perror (clnt_append, "call failed");
        } else if (*appendStatus == -1) {
            // S has been completed
            break;
        } else if (*appendStatus == 0) {
            // Success!!
        }
    }

    /* Verify segments of S */
    char **segToVerify;

    for (int r = rank; r < m; r += n) {
        segToVerify = rpcgetseg_2(&r, clnt_verify);
        if (segToVerify == (char **)NULL) {
            clnt_perror (clnt_verify, "call failed");
        }
        // This should never actually run!
        else if (strcmp(*segToVerify, "-") == 0) {
            break;
        } 

        localCount += verifySegment(*segToVerify);
    }

    /* Once completed, summate local count to global count */
    omp_set_lock(&countLock);
    totalCount += localCount;
    omp_unset_lock(&countLock);

    /* Get string from verify server to display from master thread */
    int *param = 0;
    S = rpcgets_2((void*)&param, clnt_verify);
    if (S == (char **)NULL) {
        clnt_perror (clnt_verify, "call failed");
    }
}

/* Return 1 if the segment is verified
 * Return 0 otherwise  */
int verifySegment(char* segment)
{
    int numC0 = 0, numC1 = 0, numC2 = 0;
    for (int i = 0; i < strlen(segment); i++) {
        if      (segment[i] == c0) numC0 ++;
        else if (segment[i] == c1) numC1 ++;
        else if (segment[i] == c2) numC2 ++;
    }

    int segVerified = 0;

    if(f == 0) {
        if((numC0 + numC1) == numC2) {
            segVerified = 1;
        }
    } else if(f == 1) {
        if((numC0 + 2*numC1) == numC2) {
            segVerified = 1;
        }
    } else if(f == 2) {
        if((numC0 * numC1) == numC2) {
            segVerified = 1;
        }
    } else if(f == 3) {
        if((numC0 - numC1) == numC2){
            segVerified = 1;
        }
    }

    return segVerified;
}

