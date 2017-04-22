#include <stdio.h>
#include </usr/include/linux/cuda.h>
#include <stdlib.h>
#include <string.h>
#include "ppmFile.h"

int radius;
char* inputFileName;
char* outputFileName;

// __global__ void processImg(int *a)
// {
//
// }

int main(int argc, char** argv)
{
  if (argc != 3)
  {
      fprintf(stderr, "Invalid number of arguments!");
      return 1;
  }

  radius = atoi(argv[0]);

  inputFileName = malloc(sizeof(char) * strlen(argv[2]));
  memcpy(inputFileName, argv[2], sizeof(char) * strlen(argv[2]));

  outputFileName = malloc(sizeof(char) * strlen(argv[3]));
  memcpy(outputFileName, argv[3], sizeof(char) * strlen(argv[3]));

  Image* imgIn = ImageRead(inputFileName);
  int width = ImageWidth(imgIn);
  int height = ImageHeight(imgIn);

  Image* imgOut = ImageCreate(width, height);



  return 0;
}
