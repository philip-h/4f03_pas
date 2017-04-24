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

  double local_start, local_finish, local_elapsed, elapsed;
  MPI_Barrier(MPI_COMM_WORLD);
  local_start = MPI_Wtime();

  imgIn = ImageRead(argv[1]);
  width = ImageWidth(imgIn);
  height = ImageHeight(imgIn);
  imgOut = ImageCreate(width, height);

  // Number of Pixels Per Process (nppp)
  int nppp = (width * height) / num_p;
  int remaining = (width * height) % num_p;

  // Determine the range of pixels that each thread will read

  int start, end;
  if (rank == num_p - 1)
  {
    start = rank * nppp;
    end = start + nppp + remaining;
  } else
  {
    start = rank * nppp;
    end = start + nppp;
  }

  int *outPixels;
  outPixels = (int *)malloc(sizeof(int) * ((end - start)*5));

  int counter = 0;
  for (int pixel = start; pixel < end; pixel ++)
  {
    int totalR = 0, totalG = 0, totalB = 0, numPixels = 0;
    int x = (int)(pixel % width);
    int y = (int)(pixel / width);

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
    outPixels[counter++] = x;
    outPixels[counter++] = y;
    outPixels[counter++] = newR;
    outPixels[counter++] = newG;
    outPixels[counter++] = newB;

  }

  if(rank != 0)
  {
    MPI_Ssend(outPixels, (end - start)*5, MPI_INT, 0, 0, MPI_COMM_WORLD);
  }
  else
  {
    for(int i = 0; i < (end - start)*5; i+=5)
    {
      int x = outPixels[i];
      int y = outPixels[i+1];
      int r = outPixels[i+2];
      int g = outPixels[i+3];
      int b = outPixels[i+4];

      ImageSetPixel(imgOut, x, y, RED, r);
      ImageSetPixel(imgOut, x, y, GREEN, g);
      ImageSetPixel(imgOut, x, y, BLUE, b);
    }

    for(int i = 1; i < num_p; i++)
    {
      MPI_Recv(outPixels, (end + remaining - start)*5, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

      for(int j = 0; j < (end - start)*5; j+=5)
      {
        int x = outPixels[j];
        int y = outPixels[j+1];
        int r = outPixels[j+2];
        int g = outPixels[j+3];
        int b = outPixels[j+4];

  
        ImageSetPixel(imgOut, x, y, RED, r);
        ImageSetPixel(imgOut, x, y, GREEN, g);
        ImageSetPixel(imgOut, x, y, BLUE, b);
      }
    }
    ImageWrite(imgOut, argv[2]);
  }

  local_finish = MPI_Wtime();
  local_elapsed = local_finish - local_start;
  MPI_Reduce(&local_elapsed, &elapsed, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
  if (rank == 0) {
    printf("Elapsed time: %e seconds\n", elapsed);
  }

  
  MPI_Finalize();

  return 0;
}
