/*
Contributors: Philip Habib (habibp)
Theo Stone (stonet)
*/

#include <iostream>
#include <string>

#include <cstdlib>
#include <cmath>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>


using namespace std;

/* Prototypes */
int checkArgs(int);
void *createString(void *);
void checkConditionNum(int);
void enforce0(long);
void enforce1(long);
void enforce2(long);
void enforce3(long);
//TODO: delete
void enforceRules(long);

/* Global vars */
string S = "";
int f, n, l, m;
char* c;
char* sigma;

pthread_mutex_t stringMutex;
pthread_mutex_t countMutex;

int MILIS = 1000000;
int numVerified = 0;

int main(int argc, char* argv[]) {
    /**
      Process command line arguments
      */
    if (argc < 8) {
        printf("Yo dawg, you didn't do the command line thing right... try again\n");
        return -1;
    }

    // Assign f,n,l,m from the command line
    f = atoi(argv[1]);
    n = atoi(argv[2]);

    l = atoi(argv[3]);
    m = atoi(argv[4]);

    // Assign which characters will be used to enforce segment rules
    c = (char*) malloc(sizeof(char)*3);
    for (short i = 0; i < 3; i++) {
        c[i] = *argv[i+5];
    }

    // Init our alphabet from a- [c-g]
    sigma = (char*) malloc(sizeof(char)*n);
    for (size_t i = 0; i < n; i++) {
        sigma[i] = (char)(i + (int)'a');
    }

    if (checkArgs(argc) != 0) {
        return -1;
    }

    /**
      Set up Posix Threads
      */
    srand(time(NULL));
    pthread_t* threads = (pthread_t*) malloc(n*sizeof(pthread_t));

    printf("Starting Threads ...\n");
    for (size_t i = 0; i < n; i++) {
        pthread_create(&threads[i], NULL, createString, (void *)i);
    }

    for (size_t i = 0; i < n; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("%s\n", S.c_str());
    printf("%d\n", numVerified);

    // Free allocated memory!
    free(threads);
    free(c);
    free(sigma);

    return 0;
}

/**
  Checks to make sure command line arguments don't break the program!
  Will print and return with an error if there is a problem with the arguments.
  */
int checkArgs(int argc){
    if (f < 0 || f > 3) {
        printf("F must be between 0 and 3\n");
        return -1;
    }

    if (n < 3 || n > 8) {
        printf("N must be between 3 and 8");
        return -1;
    }

    if (l < 0) {
        printf("L cannot be negative\n");
        return -1;
    }

    if (m < 0) {
        printf("M cannot be negative\n");
        return -1;
    }

    if(m % n != 0)
    {
        printf("M does not divide N\n");
        return -1;
    }

    return 0;
}

/**
  Uses Threads to create a string that will adhere to some rules specified
  in the command line arguments
  */
void *createString(void *r) {
    long rank = (long) r;

    int randSleep, randSleepNano;
    struct timespec tim, tim2;
    tim.tv_sec  = 0;

    while(true){
        randSleep = rand() % 400 + 100;
        randSleepNano = randSleep * MILIS;

        tim.tv_nsec = randSleepNano;

        if(nanosleep(&tim , &tim2) < 0 )
        {
            printf("Nano sleep system call failed! \n");
        }

        pthread_mutex_lock(&stringMutex);
        int lenS = S.length();
        if (lenS < m * l) {
            switch(f) {
                case 0: enforce0(rank); break;
                case 1: enforce1(rank); break;
                case 2: enforce2(rank); break;
                case 3: enforce3(rank); break;
            }
        } else {
            pthread_mutex_unlock(&stringMutex);
            checkConditionNum(rank);
            break;
        }
        pthread_mutex_unlock(&stringMutex);
    }
}

/**
  Checks whether a certain rule is satisfied in a segment.
  Each thread will check 1 or more segments.
  */
void checkConditionNum(int threadRank)
{
    if(threadRank + 1 <= m)
    {
        int c0 = 0, c1 = 0, c2 = 0;

        for (size_t t = threadRank; t < m; t+=n) {

            for (size_t i = t*l; i < t*l+l; i++) {
                if(S[i] == c[0])  c0++;
                else if(S[i] == c[1])  c1++;
                else if(S[i] == c[2])  c2++;
            }

            if(f == 0)
            {
                if((c0 + c1) == c2)
                {
                    pthread_mutex_lock(&countMutex);
                    numVerified++;
                    pthread_mutex_unlock(&countMutex);
                }
            }
            else if(f == 1)
            {
                if((c0 + 2*c1) == c2)
                {
                    pthread_mutex_lock(&countMutex);
                    numVerified++;
                    pthread_mutex_unlock(&countMutex);
                }
            }
            else if(f == 2)
            {
                if((c0 * c1) == c2)
                {
                    pthread_mutex_lock(&countMutex);
                    numVerified++;
                    pthread_mutex_unlock(&countMutex);
                }
            }
            else if(f == 3)
            {
                if((c0 - c1) == c2)
                {
                    pthread_mutex_lock(&countMutex);
                    numVerified++;
                    pthread_mutex_unlock(&countMutex);
                }
            }

            c0 = 0;
            c1 = 0;
            c2 = 0;
        }
    }
}

void enforce0(long rank) {
    // Count how many occurences of c0, c1 and c2 are already in S
    int c0 = 0, c1 = 0, c2 = 0;

    int lenS = S.length();
    int i_start = 0, i_end = 0;

    // For enforcing, to check the number of letters, every thread must count the
    // number of letters in the segment currently being written to... This for
    // loop calculates this range!
    for (size_t i = 0; i <= m; i++) {
        if (lenS < l*i) {
            i_start = l*i - l;
            i_end = l * i;
            break;
        }
    }

    // Count the number of occurrences of c[0..2] in the current segment
    for (size_t i = i_start; i < i_end; i++) {
        if(S[i] == c[0])  c0++;
        else if(S[i] == c[1])  c1++;
        else if(S[i] == c[2])  c2++;
    }

    int numC0Needed = (c2 - c1) - c0;
    int numC1Needed = (c2 - c0) - c1;
    int numC2Needed = (c0 + c1) - c2;

    if (n == 3 && l % 2 == 1) {
        /*ERROR*/
        fprintf(stderr, "n and l don't work for enforcing rule 0\n");
    }


    int lenCurrSegment = lenS % l;
    // If the current segment is empty
    if (lenCurrSegment == 0) {
        // Only let in a letter used in the property check
        if (sigma[rank] == c[0] || sigma[rank] == c[1] || sigma[rank] == c[2]) {
            S += sigma[rank];
        }  

    } else if (numC0Needed == 0 && numC1Needed == 0 && numC2Needed == 0) {
        // If the current segment is one away from being empty
        if (l - lenCurrSegment == 1) {
            // And we are not appending a letter used in the property check
            if (sigma[rank] != c[0] && sigma[rank] != c[1] && sigma[rank] != c[2]) {
                S += sigma[rank];
            } 
            // If the current segment is being filled, fill it  
        } else {
            S += sigma[rank];
        }
    } else {
        // If we are trying to add c0, and we need c0, add c0
        if (sigma[rank] == c[0] && numC0Needed > 0) {
            S += sigma[rank];
        } else if (sigma[rank] == c[1] && numC1Needed > 0) {
            S += sigma[rank];
        } else if (sigma[rank] == c[2] && numC2Needed > 0) {
            S += sigma[rank];
        }
    }
}

void enforce1(long rank) {
    /*printf("Currrent string start:\t%s\n", S.c_str());*/
    /*printf("trying to add %c\n", sigma[rank]);*/
    // Count how many occurences of c0, c1 and c2 are already in S
    int c0 = 0, c1 = 0, c2 = 0;

    int lenS = S.length();
    int i_start = 0, i_end = 0;

    // For enforcing, to check the number of letters, every thread must count the
    // number of letters in the segment currently being written to... This for
    // loop calculates this range!
    for (size_t i = 0; i <= m; i++) {
        if (lenS < l*i) {
            i_start = l*i - l;
            i_end = l * i;
            break;
        }
    }

    // Count the number of occurrences of c[0..2] in the current segment
    for (size_t i = i_start; i < i_end; i++) {
        if(S[i] == c[0])  c0++;
        else if(S[i] == c[1])  c1++;
        else if(S[i] == c[2])  c2++;
    }

    int numC0Needed = (c2 - 2*c1) - c0;
    int numC1Needed = (c2 - c0)/2 - c1;
    int numC2Needed = (c0 + 2*c1) - c2;

    if (n == 3 && l == 1) {
        /*ERROR*/
        fprintf(stderr, "n and l don't work for enforcing rule 1\n");
    }


    int lenCurrSegment = lenS % l;

    if (l == 2 || l == 4) {
        /* Don't add a c2*/
        if (sigma[rank] == c[1]) {
            sigma[rank] = '\0';
        } 
    }
    else if (l == 3) {
        if (sigma[rank] == c[0]) {
            /* Don't add a c0 */
            sigma[rank] = '\0';
        }
        numC1Needed = (c2-c0+1)/2 -c1;
    } 

    // If the current segment is empty
    if (lenCurrSegment == 0) {
        // Only let in a letter used in the property check
        if (sigma[rank] == c[0] || sigma[rank] == c[1] || sigma[rank] == c[2]) {
            S += sigma[rank];
        }  

    } else if (numC0Needed == 0 && numC1Needed == 0 && numC2Needed == 0) {
        if (sigma[rank] == c[0] || sigma[rank] == c[2] || sigma[rank] == c[1]) {
            if(l - lenCurrSegment == 1 && n == 3){
                // Randomness has messed up, let's try again
                printf("Resseting segment\n");
                S = S.substr(0,lenS-lenCurrSegment);
            }

            else if (sigma[rank] == c[0] || sigma[rank] == c[2]) {
                // dont have enough room (REMAKE THIS COMMENT)
                if (l - lenCurrSegment >= 2) {
                    S += sigma[rank];
                }  
            } else if(sigma[rank] == c[1]){
                if (l - lenCurrSegment >= 3) {
                    S += sigma[rank];
                }
            }      
        } else {
            S += sigma[rank];
        }
    } else {
        // If we are trying to add c0, and we need c0, add c0
        if (sigma[rank] == c[0] && numC0Needed > 0) {
            S += sigma[rank];
        } else if (sigma[rank] == c[1] && numC1Needed > 0) {
            S += sigma[rank];
        } else if (sigma[rank] == c[2] && numC2Needed > 0) {
            S += sigma[rank];
        }
    }   
}


void enforce2(long rank) {}
void enforce3(long rank) {}

/**
  Forces the rules f0 - f1 according to arguments specified.
  */
void enforceRules(long threadRank) {
    // Count how many occurences of c0, c1 and c2 are already in S
    int c0 = 0, c1 = 0, c2 = 0;

    int lenS = S.length();
    int i_start = 0, i_end = 0;

    // For enforcing, to check the number of letters, every thread must count the
    // number of letters in the segment currently being written to... This for
    // loop calculates this range!
    for (size_t i = 0; i <= m; i++) {
        if (lenS < l*i) {
            i_start = l*i - l;
            i_end = l * i;
            break;
        }
    }

    // Count the number of occurrences of c[0..2] in the current segment
    for (size_t i = i_start; i < i_end; i++) {
        if(S[i] == c[0])  c0++;
        else if(S[i] == c[1])  c1++;
        else if(S[i] == c[2])  c2++;
    }

    int numC0Needed = 0, numC1Needed = 0, numC2Needed = 0;
    // Calculate which letters are needed to enforce rule 0
    if (f == 0) {
        numC0Needed = (c2 - c1) - c0;
        numC1Needed = (c2 - c0) - c1;
        numC2Needed = (c0 + c1) - c2;
    }
    // Calculate which letters are needed to enforce rule 1
    else if (f == 1) {
        numC0Needed = (c2 - 2*c1) - c0;
        numC1Needed = (c2 - c0)/2 - c1;
        numC2Needed = (c0 + 2*c1) - c2;
    }
    // Calculate which letters are needed to enforce rule 2
    else if (f == 2) {
        if (c0 == 0 ) {
            numC0Needed = 1;
        } else if (c1 == 0) {
            numC1Needed = 1;
        } else {
            numC0Needed = (c2 / c1) - c0;
            numC1Needed = (c2 / c0) - c1;
            numC2Needed = (c0 * c1) - c2;
        }
    }
    // Calculate which letters are needed to enforce rule 3
    else if (f == 3) {
        numC0Needed = (c2 + c1) - c0;
        numC1Needed = (-c2 + c0) - c1;
        numC2Needed = (c0 - c1) - c2;
    }

    // if no letters are needed, any letter may be appended to S
    if (numC0Needed == 0 && numC1Needed == 0 && numC2Needed == 0) {
        // If we are beginning a new segment, only let c0, c1 or c2
        if (lenS % l == 0) {
            if (sigma[threadRank] == c[0] || sigma[threadRank] == c[1] || sigma[threadRank] == c[2]) {
                S += sigma[threadRank];
            }
        }
        S += sigma[threadRank];
    }

    else {
        // If we need some c0, add it to the string
        if (sigma[threadRank] == c[0] ) {
            if (numC0Needed > 0) {
                S += sigma[threadRank];
            }
        }


        // If we need some c1, add it to the string
        else if (sigma[threadRank] == c[1]) {
            if (numC1Needed > 0) {
                S += sigma[threadRank];
            }
        }

        // If we need some c2, add it to the string
        else if (sigma[threadRank] == c[2]) {
            if (numC2Needed > 0) {
                S += sigma[threadRank];
            }
        }
    }



}
