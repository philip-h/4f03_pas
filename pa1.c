/*
  Contributors: Philip Habib
                Theo Stone
*/

#include <iostream>
#include <cstdlib>
#include <string>
#include <string.h>

#include <pthread.h>

using namespace std;

/* Prototypes */
void *createString(void *);
int checkArgs(int);

/* Global vars */
string S = "";
int f, n, l, m;
char* c;

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
  printf("Thread %d running...\n", rank);
  int lenS = S.length();

  S += c[rank];

}
