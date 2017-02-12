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
#include <omp.h>


using namespace std;

/* Prototypes */
int checkArgs(int);
void *createString(void *);
void checkConditionNum(int);
void enforce0(long);
void enforce1(long);
void enforce2(long);
void enforce3(long);

/* Global vars */
string S = "";
int f, n, l, m;
char* c;
char* sigma;

omp_lock_t stringLock;
omp_lock_t countLock;

int MILIS = 1000000;
int numVerified = 0;

string error = "Useage: ./pa2.x i N L M c0 c1 c2\n"
               "Where\n"
               "i\tthe index of the property F_i which each segment will satisfy\n"
               "N\tthe number of threads\n"
               "L\tthe length of each segment\n"
               "M\tthe number of segments to generate\n"
               "c_i\tthe letters to be used in the property check\n";

int main(int argc, char* argv[]) {
    /**
      Process command line arguments
      */
    if (argc < 8) {
        printf("Too few arguments\n%s", error.c_str());
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

    printf("Starting Threads ...\n");

#   pragma omp parallel num_threads(n)
    {
        createString(0);
    }

    FILE *f = fopen("out.txt", "w");
    if (f == NULL) {
        printf("Error opening file\n");
        return -1;
    }

    fprintf(f, "%s\n%d\n", S.c_str(), numVerified);
    fclose(f);

    printf("%s\n", S.c_str());
    printf("%d\n", numVerified);

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
        printf("F must be between 0 and 3\n%s", error.c_str());
        return -1;
    }

    if (n < 3 || n > 8) {
        printf("N must be between 3 and 8\n%s", error.c_str());
        return -1;
    }

    if (l < 0) {
        printf("L cannot be negative\n%s", error.c_str());
        return -1;
    }

    if (m < 0) {
        printf("M cannot be negative\n%s", error.c_str());
        return -1;
    }

    if(m % n != 0)
    {
        printf("M does not divide N\n%s", error.c_str());
        return -1;
    }

    return 0;
}

/**
  Uses Threads to create a string that will adhere to some rules specified
  in the command line arguments
  */
void *createString(void *r) {
    int rank = omp_get_thread_num();

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

        omp_set_lock(&stringLock);
        int lenS = S.length();
        if (lenS < m * l) {
            switch(f) {
                case 0: enforce0(rank); break;
                case 1: enforce1(rank); break;
                case 2: enforce2(rank); break;
                case 3: enforce3(rank); break;
            }
        } else {
            omp_unset_lock(&stringLock);
            checkConditionNum(rank);
            break;
        }
        omp_unset_lock(&stringLock);
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
                    omp_set_lock(&countLock);
                    numVerified++;
                    omp_unset_lock(&countLock);
                }
            }
            else if(f == 1)
            {
                if((c0 + 2*c1) == c2)
                {
                    omp_set_lock(&countLock);
                    numVerified++;
                    omp_unset_lock(&countLock);
                }
            }
            else if(f == 2)
            {
                if((c0 * c1) == c2)
                {
                    omp_set_lock(&countLock);
                    numVerified++;
                    omp_unset_lock(&countLock);
                }
            }
            else if(f == 3)
            {
                if((c0 - c1) == c2)
                {
                    omp_set_lock(&countLock);
                    numVerified++;
                    omp_unset_lock(&countLock);
                }
            }

            c0 = 0;
            c1 = 0;
            c2 = 0;
        }
    }
}

/*Enforce all 4 rules in 4 separate methods*/
void enforce0(long rank) {
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
        fprintf(stderr, "N and L don't work for enforcing rule 0\n");
        exit(-1);
    }

    int lenCurrSegment = lenS % l;

    // If the current segment is empty
    if (lenCurrSegment == 0) {
        // Only let in a letter used in the property check
        if (sigma[rank] == c[0] || sigma[rank] == c[1] || sigma[rank] == c[2]) {
            S += sigma[rank];
        }

    // If the current segment is not empty and the property is satisfied
    } else if (numC0Needed == 0 && numC1Needed == 0 && numC2Needed == 0) {
        // If the current segment does not have enough room left
        if (l - lenCurrSegment == 1) {
            // Only allow a letter that is not part of the property
            if (sigma[rank] != c[0] && sigma[rank] != c[1] && sigma[rank] != c[2]) {
                S += sigma[rank];
            }
        // Otherwise, add a letter!!
        } else {
            S += sigma[rank];
        }
    // If the current segment is not empty and the current property is not satisfied
    } else {
        // Add a letter according to its need
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
        fprintf(stderr, "N and L don't work for enforcing rule 1\n");
        exit(-1);
    }


    int lenCurrSegment = lenS % l;

    // When l == 2 or 4, the solution will never contain c1
    if (l == 2 || l == 4) {
        if (sigma[rank] == c[1]) {
            /* Don't add a c1*/
            sigma[rank] = '\0';
        }
    }
    // When l == 3 the solution will never contain c0
    else if (l == 3) {
        if (sigma[rank] == c[0]) {
            /* Don't add a c0 */
            sigma[rank] = '\0';
        }
        // When l == 3, this calculation should round up
        numC1Needed = (c2-c0+1)/2 -c1;
    }

    // If the current segment is empty
    if (lenCurrSegment == 0) {
        // Only let in a letter used in the property check
        if (sigma[rank] == c[0] || sigma[rank] == c[1] || sigma[rank] == c[2]) {
            S += sigma[rank];
        }

    // If the current segment is not empty and the property is satisfied
    } else if (numC0Needed == 0 && numC1Needed == 0 && numC2Needed == 0) {
        // If the incomming letter is a property checked letter
        if (sigma[rank] == c[0] || sigma[rank] == c[2] || sigma[rank] == c[1]) {
            // If the current segment does not have enough room left
            if(l - lenCurrSegment == 1 && n == 3){
                // Reset the segment and try again
                S = S.substr(0,lenS-lenCurrSegment);
            }
            /* 
             * If the incomming letter is c0 or c2, make sure 
             * there are at least 2 spots left. If there are, 
             * append the letter
             */
            else if (sigma[rank] == c[0] || sigma[rank] == c[2]) {
                if (l - lenCurrSegment >= 2) {
                    S += sigma[rank];
                }
            /* 
             * If the incomming letter is c1, make sure 
             * there are at least 3 spots left. If there are, 
             * append the letter
             */
            } else if(sigma[rank] == c[1]){
                if (l - lenCurrSegment >= 3) {
                    S += sigma[rank];
                }
            }
        // If the incomming letter is not a property checked letter, add it    
        } else {
            S += sigma[rank];
        }
        
    // If the current segment is not empty and the current property is not satisfied
    } else {
        // Add a letter according to it's need
        if (sigma[rank] == c[0] && numC0Needed > 0) {
            S += sigma[rank];
        } else if (sigma[rank] == c[1] && numC1Needed > 0) {
            S += sigma[rank];
        } else if (sigma[rank] == c[2] && numC2Needed > 0) {
            S += sigma[rank];
        }
    }
}


void enforce2(long rank) {
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

    int numC0Needed = 1;
    int numC1Needed = 1;
    int numC2Needed = (c0*c1) - c2;


    // Make sure we are not dividing by 0
    if (c1 != 0) {
        numC0Needed = (c2/c1) - c0;
    }
    if (c0 != 0) {
        numC1Needed = (c2/c0) - c1;
    }

    // When l == 1 the solution will never contain c2
    if (l == 1) {
        if (sigma[rank] == c[2]) {
            /* Don't add a c2*/
            sigma[rank] = '\0';
        }
    }

    int lenCurrSegment = lenS % l;

    // If the current segment is empty
    if (lenCurrSegment == 0) {
        // Only let in a letter used in the property check
        if (sigma[rank] == c[0] || sigma[rank] == c[1] || sigma[rank] == c[2]) {
            S += sigma[rank];
        }

    // If the current segment is not empty and the property is satisfied
    } else if (numC0Needed == 0 && numC1Needed == 0 && numC2Needed == 0) {
        // If the incomming letter is a property checked letter
        if (sigma[rank] == c[0] || sigma[rank] == c[1] || sigma[rank] == c[2]) {
            // If the current segment does not have enough room left
            if (l - lenCurrSegment == 1 && n == 3) {
                // Reset the segment and try again
                S = S.substr(0,lenS - lenCurrSegment);
            }
            /* 
             * If the incomming letter is c0, make sure 
             * there are at least c1 spots left. If there are, 
             * append the letter
             */
            if (sigma[rank] == c[0]) {
                if (l - lenCurrSegment > c1){
                    S += sigma[rank];
                }
            }
            /* 
             * If the incomming letter is c1, make sure 
             * there are at least c0 spots left. If there are, 
             * append the letter
             */
            else if (sigma[rank] == c[1]) {
                if (l - lenCurrSegment > c0) {
                    S += sigma[rank];
                }
            }
            // If the incomming letter is c2
            else if (sigma[rank] == c[2]) {
                /*Do NOTHING*/
            }
        }
        // If the incomming letter is not a property checked letter, add it    
        else {
            S += sigma[rank];
        }
    // If the current segment is not empty and the current property is not satisfied
    } else {
        // Add a letter according to it's need
        if (sigma[rank] == c[0] && numC0Needed > 0) {
            S += sigma[rank];
        } else if (sigma[rank] == c[1] && numC1Needed > 0) {
            S += sigma[rank];
        } else if (sigma[rank] == c[2] && numC2Needed > 0) {
            S += sigma[rank];
        }
    }
}

void enforce3(long rank) {
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

    int numC0Needed = (c2 + c1) - c0;
    int numC1Needed = (c0 - c2) - c1;
    int numC2Needed = (c0 - c1) - c2;

    if (n == 3 && l % 2 == 1) {
        /*ERROR*/
        fprintf(stderr, "N and L don't work for enforcing rule 3\n");
        exit(-1);
    }


    int lenCurrSegment = lenS % l;

    // If the current segment is empty
    if (lenCurrSegment == 0) {
        // Only let in a letter used in the property check
        if (sigma[rank] == c[0] || sigma[rank] == c[1] || sigma[rank] == c[2]) {
            S += sigma[rank];
        }

    // If the current segment is not empty and the property is satisfied
    } else if (numC0Needed == 0 && numC1Needed == 0 && numC2Needed == 0) {
        // If the current segment does not have enough room left
        if (l - lenCurrSegment == 1) {
            // Only allow a letter that is not part of the property
            if (sigma[rank] != c[0] && sigma[rank] != c[1] && sigma[rank] != c[2]) {
                S += sigma[rank];
            }
        // Otherwise, add a letter!!
        } else {
            S += sigma[rank];
        }
    // If the current segment is not empty and the current property is not satisfied
    } else {
        // Add a letter according to its need
        if (sigma[rank] == c[0] && numC0Needed > 0) {
            S += sigma[rank];
        } else if (sigma[rank] == c[1] && numC1Needed > 0) {
            S += sigma[rank];
        } else if (sigma[rank] == c[2] && numC2Needed > 0) {
            S += sigma[rank];
        }
    }
}
