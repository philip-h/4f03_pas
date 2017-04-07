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
  int* sendCounts;
  int* displ;
  char* fileIn;
  char* fileOut;
  Image* imgIn, imgOut;


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

  sendCounts = (int*)malloc(sizeof(int) * num_p);
  displ = (int*)malloc(sizeof(int) * num_p);

  // Create the sub array to send to each process
  numRows = (int)(height / num_p);
  numLeft = (int)(height % num_p);

  for (int i = 0; i < num_p; i++)
  {
    if (i == 0)
    {
      sendCounts[i] = (numRows + radius)*width*3;
      displ[i] = 0;
    } else if (i == num_p-1)
    {
      sendCounts[i] = (numRows + numLeft + radius)*width*3;
      displ[i] = (sendCounts[i-1] - (3*width)) + displ[i-1] - (radius*3*width);
    } else
    {
      sendCounts[i] = (numRows + 2*radius)*width*3;
      displ[i] = (sendCounts[i-1] - (3*width)) + displ[i-1] - (radius*3*width);
    } 
  }

  if (rank == 0)
  {
    
    printf("width: %d, height: %d, numRows: %d, numLeft: %d, radius: %d\n", width, height, numRows, numLeft, radius);
    for (int i = 0; i < width * height * 3; i++) {
      printf("%d,", imgIn->data[i]);
      if ((i+1) % 3 == 0)
        printf("\t");
      if ((i+1) % 9 == 0)
        printf("\n");
      
    }

    printf("\n");

  }

  unsigned char *subData = (char*)malloc(sizeof(char) * sendCounts[rank]);

  MPI_Scatterv(imgIn->data, sendCounts, displ, MPI_UNSIGNED_CHAR, subData, sendCounts[rank], MPI_UNSIGNED_CHAR, 0,  MPI_COMM_WORLD);

  printf("%d\n", rank);
  printf("send[%d] = %d\n", rank, sendCounts[rank]);
  printf("displ[%d] = %d\n", rank, displ[rank]);
  printf("\n");

  for(int i = 0; i < sendCounts[rank]; i++)
  {
    printf("%d ", subData[i]);
  }

  printf("\n");
  printf("\n");


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

  ImageWrite(imgOut, fileOut);
*/
  // Finalize the MPI environment.
  MPI_Finalize();

//  free(sendCounts);
//  free(displ);

  return 0;
}
