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

/* Global vars */
string S = "";
int f, n, l, m;
char* c;
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

  c = (char*) malloc(sizeof(char)*n);

  for (short i = 0; i < argc-5; i++) {
    c[i] = *argv[i+5];
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
  free(threads);
  free(c);

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

  if (argc -5 != n) {
    printf("The sizef of sigma must be equal to the number of threads\n");
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
      S += c[rank];
    } else {
      checkConditionNum(rank);
      pthread_mutex_unlock(&stringMutex);
      break;
    }
    pthread_mutex_unlock(&stringMutex);

  }
}

void checkConditionNum(int threadRank)
{
  int c0 = 0;
  int c1 = 0;
  int c2 = 0;

  //TODO: Count occurances of vars

  if(f == 0)
  {
    return (c0 + c1) == c2;
  }
  else if(f == 1)
  {
    return (c0 + 2*c1) == c2;
  }
  else if(f == 2)
  {
    return (c0 * c1) == c2;
  }
  else if(f == 3)
  {
    return (c0 - c1) == c2;
  }
}
