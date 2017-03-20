#include "append.h"

#include<arpa/inet.h>
#include<sys/socket.h>

#define PORT 8989

int f;
int n;
int l;
int m;
char c0;
char c1;
char c2;
char *host_verify;

char *build_str;

//Sends the current build_str to the verify server
void sendToVerify()
{
	struct sockaddr_in si_other;
  int s, i, slen=sizeof(si_other);
  char buf[l*m];

  if ( (s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
  {
      perror("error on socket bind");
			exit(1);
  }

	memset((char *) &si_other, 0, sizeof(si_other));
	si_other.sin_family = AF_INET;
	si_other.sin_port = htons(PORT);

	printf("HOST: %s\n", host_verify);

	if (inet_aton(host_verify , &si_other.sin_addr) == 0)
	{
			perror("inet_aton() failed");
			exit(1);
	}

	//send the string
	if (sendto(s, build_str, strlen(build_str) , 0 , (struct sockaddr *) &si_other, slen)==-1)
	{
			perror("error on sendto()");
			exit(1);
	}
}

int *rpcinitappendserver_1_svc(append_init_params *argp, struct svc_req *rqstp)
{
	static int  result;

	f = argp->f;
  n = argp->n;
	l = argp->l;
	m = argp->m;
	c0 = argp->c0;
	c1 = argp->c1;
	c2 = argp->c2;
	host_verify = (char*)malloc(sizeof(argp->host_verify)+1);
	memcpy(host_verify, argp->host_verify, sizeof(argp->host_verify)+1);
	host_verify[sizeof(host_verify)+1] = '\0';

	printf("%s\n", argp->host_verify);
	printf("%s\n", host_verify);

    build_str = (char *)malloc(sizeof(char) * m * l);
    build_str[0] = '\0';

	result = 1;

	return &result;
}

int *rpcappend_1_svc(char *argp, struct svc_req *rqstp)
{
	static int  result;

	result = -1;

    int length = strlen(build_str);

	if(length < m*l)
	{
        switch(f) {
            case 0: enforce0(*argp); break;
            case 1: enforce1(*argp); break;
            case 2: enforce2(*argp); break;
            case 3: enforce3(*argp); break;
        }
		result = 0;
	}
	else{
		sendToVerify();
		printf("Send to verify and continue");
	}
    printf("%s\n", build_str);

	return &result;
}

void append(char c)
{
    int len = strlen(build_str);
    build_str[len] = c;
    build_str[len+1] = '\0';
}

void enforce0(char c)
{
    int numC0 = 0, numC1 = 0, numC2 = 0;

    int len = strlen(build_str);
    int i_start = 0, i_end = 0;

    // For enforcing, to check the number of letters, every thread must count the
    // number of letters in the segment currently being written to... This for
    // loop calculates this range!
    for (size_t i = 0; i <= m; i++) {
        if (len < l*i) {
            i_start = l*i - l;
            i_end = l * i;
            break;
        }
    }

    // Count the number of occurrences of c[0..2] in the current segment
    for (size_t i = i_start; i < i_end; i++) {
        if     (build_str[i] == c0)  numC0++;
        else if(build_str[i] == c1)  numC1++;
        else if(build_str[i] == c2)  numC2++;
    }

    int numC0Needed = (numC2 - numC1) - numC0;
    int numC1Needed = (numC2 - numC0) - numC1;
    int numC2Needed = (numC0 + numC1) - numC2;

    if (n == 3 && l % 2 == 1) {
        /*ERROR*/
        fprintf(stderr, "N and L don't work for enforcing rule 0\n");
        exit(-1);
    }


    int lenCurrSegment = len % l;

    // If the current segment is empty
    if (lenCurrSegment == 0) {
        // Only let in a letter used in the property check
        if (c == c0 || c == c1 || c == c2) {
            append(c);
        }

    // If the current segment is not empty and the property is satisfied
    } else if (numC0Needed == 0 && numC1Needed == 0 && numC2Needed == 0) {
        // If the current segment does not have enough room left
        if (l - lenCurrSegment == 1) {
            // Only allow a letter that is not part of the property
            if (c != c0 && c != c1 && c != c2) {
                append(c);
            }
        // Otherwise, add a letter!!
        } else {
            append(c);
        }
    // If the current segment is not empty and the current property is not satisfied
    } else {
        // Add a letter according to its need
        if (c == c0 && numC0Needed > 0) {
            append(c);
        } else if (c == c1 && numC1Needed > 0) {
            append(c);
        } else if (c == c2 && numC2Needed > 0) {
            append(c);
        }
    }
}

void enforce1(char c)
{
    int numC0 = 0, numC1 = 0, numC2 = 0;

    int len = strlen(build_str);
    int i_start = 0, i_end = 0;

    // For enforcing, to check the number of letters, every thread must count the
    // number of letters in the segment currently being written to... This for
    // loop calculates this range!
    for (size_t i = 0; i <= m; i++) {
        if (len < l*i) {
            i_start = l*i - l;
            i_end = l * i;
            break;
        }
    }

    // Count the number of occurrences of c[0..2] in the current segment
    for (size_t i = i_start; i < i_end; i++) {
        if(build_str[i] == c0)  numC0++;
        else if(build_str[i] == c1)  numC1++;
        else if(build_str[i] == c2)  numC2++;
    }

    int numC0Needed = (numC2 - 2*numC1) - numC0;
    int numC1Needed = (numC2 - numC0)/2 - numC1;
    int numC2Needed = (numC0 + 2*numC1) - numC2;

    if (n == 3 && l == 1) {
        /*ERROR*/
        fprintf(stderr, "N and L don't work for enforcing rule 1\n");
        exit(-1);
    }


    int lenCurrSegment = len % l;

    // When l == 2 or 4, the solution will never contain c1
    if (l == 2 || l == 4) {
        if (c == c1) {
            /* Don't add a c1*/
            c = '\0';
        }
    }
    // When l == 3 the solution will never contain c0
    else if (l == 3) {
        if (c == c0) {
            /* Don't add a c0 */
            c = '\0';
        }
        // When l == 3, this calculation should round up
        numC1Needed = (numC2-numC0+1)/2 -numC1;
    }

    // If the current segment is empty
    if (lenCurrSegment == 0) {
        // Only let in a letter used in the property check
        if (c == c0 || c == c1 || c == c2) {
            append(c);
        }

    // If the current segment is not empty and the property is satisfied
    } else if (numC0Needed == 0 && numC1Needed == 0 && numC2Needed == 0) {
        // If the incomming letter is a property checked letter
        if (c == c0 || c == c2 || c == c1) {
            // If the current segment does not have enough room left
            if(l - lenCurrSegment == 1 && n == 3){
                // Reset the segment and try again
                build_str[len - lenCurrSegment] = '\0';
            }
            /*
             * If the incomming letter is c0 or c2, make sure
             * there are at least 2 spots left. If there are,
             * append the letter
             */
            else if (c == c0 || c == c2) {
                if (l - lenCurrSegment >= 2) {
                    append(c);
                }
            /*
             * If the incomming letter is c1, make sure
             * there are at least 3 spots left. If there are,
             * append the letter
             */
            } else if(c == c1){
                if (l - lenCurrSegment >= 3) {
                    append(c);
                }
            }
        // If the incomming letter is not a property checked letter, add it
        } else {
            append(c);
        }

    // If the current segment is not empty and the current property is not satisfied
    } else {
        // Add a letter according to it's need
        if (c == c0 && numC0Needed > 0) {
            append(c);
        } else if (c == c1 && numC1Needed > 0) {
            append(c);
        } else if (c == c2 && numC2Needed > 0) {
            append(c);
        }
    }
}

void enforce2(char c)
{
    int numC0 = 0, numC1 = 0, numC2 = 0;

    int len = strlen(build_str);
    int i_start = 0, i_end = 0;

    // For enforcing, to check the number of letters, every thread must count the
    // number of letters in the segment currently being written to... This for
    // loop calculates this range!
    for (size_t i = 0; i <= m; i++) {
        if (len < l*i) {
            i_start = l*i - l;
            i_end = l * i;
            break;
        }
    }

    // Count the number of occurrences of c[0..2] in the current segment
    for (size_t i = i_start; i < i_end; i++) {
        if     (build_str[i] == c0)  numC0++;
        else if(build_str[i] == c1)  numC1++;
        else if(build_str[i] == c2)  numC2++;
    }

    int numC0Needed = 1;
    int numC1Needed = 1;
    int numC2Needed = (numC0*numC1) - numC2;


    // Make sure we are not dividing by 0
    if (numC1 != 0) {
        numC0Needed = (numC2/numC1) - numC0;
    }
    if (numC0 != 0) {
        numC1Needed = (numC2/numC0) - numC1;
    }

    // When l == 1 the solution will never contain c2
    if (l == 1) {
        if (c == c2) {
            /* Don't add a c2*/
            c = '\0';
        }
    }

    int lenCurrSegment = len % l;

    // If the current segment is empty
    if (lenCurrSegment == 0) {
        // Only let in a letter used in the property check
        if (c == c0 || c == c1 || c == c2) {
            append(c);
        }

    // If the current segment is not empty and the property is satisfied
    } else if (numC0Needed == 0 && numC1Needed == 0 && numC2Needed == 0) {
        // If the incomming letter is a property checked letter
        if (c == c0 || c == c1 || c == c2) {
            // If the current segment does not have enough room left
            if (l - lenCurrSegment == 1 && n == 3) {
                // Reset the segment and try again
                build_str[len - lenCurrSegment] = '\0';
            }
            /*
             * If the incomming letter is c0, make sure
             * there are at least c1 spots left. If there are,
             * append the letter
             */
            if (c == c0) {
                if (l - lenCurrSegment > numC1){
                    append(c);
                }
            }
            /*
             * If the incomming letter is c1, make sure
             * there are at least c0 spots left. If there are,
             * append the letter
             */
            else if (c == c1) {
                if (l - lenCurrSegment > numC0) {
                    append(c);
                }
            }
            // If the incomming letter is c2
            else if (c == c2) {
                /*Do NOTHING*/
            }
        }
        // If the incomming letter is not a property checked letter, add it
        else {
            append(c);
        }
    // If the current segment is not empty and the current property is not satisfied
    } else {
        // Add a letter according to it's need
        if (c == c0 && numC0Needed > 0) {
            append(c);
        } else if (c == c1 && numC1Needed > 0) {
            append(c);
        } else if (c == c2 && numC2Needed > 0) {
            append(c);
        }
    }
}

void enforce3(char c)
{
    int numC0 = 0, numC1 = 0, numC2 = 0;

    int len = strlen(build_str);
    int i_start = 0, i_end = 0;

    // For enforcing, to check the number of letters, every thread must count the
    // number of letters in the segment currently being written to... This for
    // loop calculates this range!
    for (size_t i = 0; i <= m; i++) {
        if (len < l*i) {
            i_start = l*i - l;
            i_end = l * i;
            break;
        }
    }

    // Count the number of occurrences of c[0..2] in the current segment
    for (size_t i = i_start; i < i_end; i++) {
        if     (build_str[i] == c0)  numC0++;
        else if(build_str[i] == c1)  numC1++;
        else if(build_str[i] == c2)  numC2++;
    }

    int numC0Needed = (numC2 + numC1) - numC0;
    int numC1Needed = (numC0 - numC2) - numC1;
    int numC2Needed = (numC0 - numC1) - numC2;

    if (n == 3 && l % 2 == 1) {
        /*ERROR*/
        fprintf(stderr, "N and L don't work for enforcing rule 3\n");
        exit(-1);
    }


    int lenCurrSegment = len % l;

    // If the current segment is empty
    if (lenCurrSegment == 0) {
        // Only let in a letter used in the property check
        if (c == c0 || c == c1 || c == c2) {
            append(c);
        }

    // If the current segment is not empty and the property is satisfied
    } else if (numC0Needed == 0 && numC1Needed == 0 && numC2Needed == 0) {
        // If the current segment does not have enough room left
        if (l - lenCurrSegment == 1) {
            // Only allow a letter that is not part of the property
            if (c != c0 && c != c1 && c != c2) {
                append(c);
            }
        // Otherwise, add a letter!!
        } else {
            append(c);
        }
    // If the current segment is not empty and the current property is not satisfied
    } else {
        // Add a letter according to its need
        if (c == c0 && numC0Needed > 0) {
            append(c);
        } else if (c == c1 && numC1Needed > 0) {
            append(c);
        } else if (c == c2 && numC2Needed > 0) {
            append(c);
        }
    }
}
