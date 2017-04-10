#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ppmFile.h"

/* Globals */
const int MAX_STRING = 100;
const int RED = 0;
const int GREEN = 1;
const int BLUE = 2;

const char usage [] = "./pa4.x r <inputFilename>.ppm <outputFilename>.ppm\n"
      "where:\n"
      "r is the bulrr radians in pixels\n"
      "<inputFilename>.ppm is the name of the file to blurr\n"
      "<outputFilename>.ppm is the name of the file to save the blurred image\n\n\n";

int main(int argc, char** argv) {
  // Initialize the MPI environment
  MPI_Init(&argc, &argv);

  // Get the number of processes
  int num_p;
  MPI_Comm_size(MPI_COMM_WORLD, &num_p);

  // Get the rank of the process
  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  int radius, width, height;
  int numRows, numLeft;
  int *scatterCounts, *scatterDispl;
  Image *imgIn, *imgOut;


  // Remove the filename from the argv list
  argv ++;
  argc --;

  if (argc < 3) {
      fprintf(stderr, "Invalid number of arguments. Usage:\n%s", usage);
      return 1;
  }

  /* Init of variables */
  radius = atoi(argv[0]);

  if (radius == 0) {
    fprintf(stderr, "No need to blur - radius is 0\n\n\n");
    return 1;
  }

  imgIn = ImageRead(argv[1]);
  width = ImageWidth(imgIn);
  height = ImageHeight(imgIn);
  imgOut = ImageCreate(width, height);
  
  // Number of Pixels Per Process (nppp)
  int nppp = (width * height) / num_p;
  int remaining = (width * height) % num_p;

  // Determine the range of pixels that each thread will read
  int start = rank * nppp;
  int end = start + nppp;
  int start_x = start % width;
  int end_x = end % width;
  int start_y = (int) (start / width);
  int end_y = (int) (end / width); 


  for (int y = start_y; y < end_y; y++)
  {
    for (int x = start_x; x < start_x; x++) 
    {
      int totalR = 0, totalG = 0, totalB = 0, numPixels = 0;
      
    }
  } 

/*
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

*/


  return 0;
}
