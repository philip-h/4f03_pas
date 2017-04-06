// #include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ppmFile.h"

int radius;
char* inputFileName;
char* outputFileName;

int main(int argc, char** argv)
{
  //Assign variables from arguments
  if(argc != 4)
  {
    printf("%s\n", "Please input three arguments!");
    return -1;
  }

  radius = atoi(argv[1]);

  inputFileName = malloc(sizeof(char) * strlen(argv[2]));
  memcpy(inputFileName, argv[2], sizeof(char) * strlen(argv[2]));

  outputFileName = malloc(sizeof(char) * strlen(argv[3]));
  memcpy(outputFileName, argv[3], sizeof(char) * strlen(argv[3]));

  printf("You gave the input: \n\tradius = %d\n\tInput = %s\n\tOutput = %s\n", radius, inputFileName, outputFileName);

  // Initialize the MPI environment
  // MPI_Init(NULL, NULL);

  // Get the number of processes
  int num_p;
  // MPI_Comm_size(MPI_COMM_WORLD, &num_p);

  // Get the rank of the process
  int rank;
  // MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  // Finalize the MPI environment.
  // MPI_Finalize();

  const int RED = 0;
  const int GREEN = 1;
  const int BLUE = 2;

  Image* imgIn = ImageRead(inputFileName);
  int width = ImageWidth(imgIn);
  int height = ImageHeight(imgIn);

  Image* imgOut = ImageCreate(width, height);

  for(int y = 0; y < height; y++)
  {
    for(int x = 0; x < width; x++)
    {
      int totalR = 0, totalG = 0, totalB = 0, numPixels = 0;

      for(int bY = y-radius; bY <= y+radius; bY++)
      {
        for(int bX = x-radius; bX <= x + radius;  bX++)
        {
          if(bY < 0 || bX < 0 || bX > width || bY > height)
          {
            continue;
          }
          else
          {
            totalR += ImageGetPixel(imgIn, bX, bY, RED);
            totalG += ImageGetPixel(imgIn, bX, bY, GREEN);
            totalB += ImageGetPixel(imgIn, bX, bY, BLUE);
            numPixels ++;
          }
        }
      }

      int newR = (int)(totalR / numPixels);
      int newG = (int)(totalG / numPixels);
      int newB = (int)(totalB / numPixels);

      ImageSetPixel(imgOut, x, y, RED, newR);
      ImageSetPixel(imgOut, x, y, GREEN, newG);
      ImageSetPixel(imgOut, x, y, BLUE, newB);

    }
  }

  ImageWrite(imgOut, outputFileName);

  return 0;
}
