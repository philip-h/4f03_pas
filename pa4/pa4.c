// #include <mpi.h>
#include <stdio.h>
#include "ppmFile.h"

int main(int argc, char** argv) {
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

  int radius = 1;

  Image* imgIn = ImageRead("fox.ppm");
  int width = ImageWidth(imgIn);   
  int height = ImageHeight(imgIn);   

  printf("WIDTH %d\n", width);

  Image* imgOut = ImageCreate(width, height);

  int c = 0;
  
  for(int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      int totalR = 0, totalG = 0, totalB = 0, numPixels = 0;

      for (int i = y-radius; i <= y+radius; i++) {
        for (int j = x-radius; j <= x+radius; j++) {

          if (i < 0 || j < 0 || i >= width || j >= height) {
            continue;
          } else {
            totalR += ImageGetPixel(imgIn, j, i, RED);
            totalG += ImageGetPixel(imgIn, j, i, GREEN);
            totalB += ImageGetPixel(imgIn, j, i, BLUE);
            numPixels ++;
          }
        }
      }

      c ++;
      printf("%d\t num pixels %d\n", c, numPixels);
      int newR = (int)(totalR / numPixels);
      int newG = (int)(totalG / numPixels);
      int newB = (int)(totalB / numPixels);

      ImageSetPixel(imgOut, x, y, RED, newR);
      ImageSetPixel(imgOut, x, y, GREEN, newG);
      ImageSetPixel(imgOut, x, y, BLUE, newB);

    }
  }  

  ImageWrite(imgOut, "out.ppm");
  
  return 0;
}
