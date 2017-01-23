/*
  Contributors: Philip Habib
                Theo Stone
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

/* Prototypes */
void *createString(void *);
int checkArgs(int, int, int, int, char* [], int);

/* Global vars */
char* S;
int f, n, l, m;
char* c [];

int main(int argc, char* argv[]) {
  /**
    Process command line arguments
  */
  if (argc < 8) {
    printf("Yo dawg, you didn't do the command line thing right... try again\n");
    return -1;
  }

  /* Type checking the first 4 arguments*/
  //TODO: Actually check them

  f = atoi(argv[1]);
  n = atoi(argv[2]);

  l = atoi(argv[3]);
  m = atoi(argv[4]);
  // loop from 5 to end of arcg
  for (short i = 0; i < argc-5; i++) {
    c[i] = argv[i+5];
  }
  int len = argc - 5;
  for (size_t i = 0; i < len; i++) {
    printf("%s\n", c[i]);
  }

  if (checkArgs(argc) != 0) {
    return -1;
  }

  /**
    Set up Posix Threads
  */
  pthread_t* threads = malloc(n*sizeof(pthread_t));

  for (short i = 0; i < n; i++) {
    pthread_create(&threads[i], NULL, createString, (void *)i);
  }

  for (short i = 0; i < n; i++) {
    pthread_join(threads[i], NULL);
  }

  free(threads);
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
  short rank = (short) r;

}
