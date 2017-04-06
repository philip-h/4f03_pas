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
      "<outputFilename>.ppm is the name of the file to save the blurred image\n";

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
  int sendCounts[num_p];
  int displ[num_p];
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

  fileIn = malloc(sizeof(char) * strlen(argv[2]));
  memcpy(fileIn, argv[2], sizeof(char) * strlen(argv[2]));

  fileOut = malloc(sizeof(char) * strlen(argv[3]));
  memcpy(fileOut, argv[3], sizeof(char) * strlen(argv[3]));


  if (rank == 0)
  {
    imgIn = ImageRead(fileIn);
    width = ImageWidth(imgIn);
    height = ImageHeight(imgIn);

    // Create the sub array to send to each process
    numRows = (int)(height / num_p);
    numLeft = (int)(height % num_p);

    for (int i = 0; i < num_p; i++)
    {
      if (i == 0)
      {
        sendCounts[num_p] = (numRows + radius)*width*3;
        displ[num_p] = 0;
      } else if (i == num_p - 1)
      {
        sendCounts[num_p] = (numRows + numLeft + radius)*width*3;
        displ[num_p] = (numRows + displ[num_p - 1])*width*3;
      } else
      {
        sendCounts[num_p] = (numRows + 2*radius)*width*3;
        displ[num_p] = (numRows + displ[num_p - 1])*width*3;
      }
    }
  }
  unsigned char *subData = (char*)malloc(sizeof(char) * sendCounts[rank]);

  MPI_Scatterv(imgIn->data, sendCounts, displ, MPI_UNSIGNED_CHAR, &subData, 6000, MPI_UNSIGNED_CHAR, 0, MPI_COMM_WORLD);

  printf("%d\n", rank);

  for(int i = 0; i < sendCounts[rank]; i++)
  {
    printf("%c\t", subData[i]);
  }

  printf("\n");



  // Take out name of program from argv list

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

  free(sendCounts);
  free(displ);

  return 0;
}
