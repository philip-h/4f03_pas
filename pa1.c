/*
  Contributors: Philip Habib
                Theo Stone
*/

#include <iostream>
#include <string>

#include <cstdlib>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>


using namespace std;

/* Prototypes */
void *createString(void *);
int checkArgs(int);
void checkConditionNum(int);

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

  /* Type checking the first 4 arguments*/

  f = atoi(argv[1]);
  n = atoi(argv[2]);

  l = atoi(argv[3]);
  m = atoi(argv[4]);

  int cSize = argc-5;

  c = (char*) malloc(sizeof(char)*cSize);

  for (short i = 0; i < cSize; i++) {
    c[i] = *argv[i+5];
  }

  sigma = (char*) malloc(sizeof(char)*n);

  for (size_t i = 0; i < n; i++) {
    sigma[i] = (char)(i + (int)'a');
  }

  int len = argc - 5;

  if (checkArgs(argc) != 0) {
    return -1;
  }

  /**
    Set up Posix Threads
  */
  srand(time(NULL));
  pthread_t* threads = (pthread_t*) malloc(n*sizeof(pthread_t));


  for (size_t i = 0; i < n; i++) {
    pthread_create(&threads[i], NULL, createString, (void *)i);
  }

  for (size_t i = 0; i < n; i++) {
    pthread_join(threads[i], NULL);
  }

  printf("%s\n", S.c_str());
  printf("%d\n", numVerified);
  free(threads);
  free(c);
  free(sigma);

  return 0;
}

int checkArgs(int argc){
  if (f < 0 || f > 3) {
    printf("F must be between 0 and 3\n");
    return -1;
  }

  if (n < 3 || n > 8) {
    printf("N must be between 0 and 8");
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

  return 0;
}

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
      S += sigma[rank];
    } else {
      pthread_mutex_unlock(&stringMutex);
      checkConditionNum(rank);
      break;
    }
    pthread_mutex_unlock(&stringMutex);
  }
}

void checkConditionNum(int threadRank)
{
  if(threadRank + 1 <= m)
  {
    int c0 = 0;
    int c1 = 0;
    int c2 = 0;

    //TODO: Count occurances of vars
    for (size_t i = threadRank*l; i < threadRank*l+(l); i++) {
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
  }
}
