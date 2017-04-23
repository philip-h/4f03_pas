#include <stdio.h>
#include <cuda.h>
#include <stdlib.h>
#include <string.h>
#include "ppmFile.cuh"

/* Globals */
const char usage [] = "./pa5.x r <inputFilename>.ppm <outputFilename>.ppm\n"
      "where:\n"
      "r is the bulrr radians in pixels\n"
      "<inputFilename>.ppm is the name of the file to blur\n"
      "<outputFilename>.ppm is the name of the file to save the blurred image\n\n\n";

__global__ void blurPPM(unsigned char *dataIn, unsigned char *dataOut, int *radius, int *width, int *height) {
  int index = threadIdx.x + blockIdx.x * blockDim.x;
  int totalR = 0, totalG = 0, totalB = 0, numPixels = 0;

  // x,y of current position given by index
  int x = (int)(index % (*width) );
  int y = (int)(index / (*width) );

  for(int bY = y-*radius; bY <= y+*radius; bY++)
  {
    for(int bX = x-*radius; bX <= x + *radius;  bX++)
    {
      if(bY < 0 || bX < 0 || bX > *width || bY > *height)
      {
        continue;
      }
      else
      {
        totalR += dataIn[(bY*(*width) + bX)*3 + 0];
        totalG += dataIn[(bY*(*width) + bX)*3 + 1];
        totalB += dataIn[(bY*(*width) + bX)*3 + 2];
        numPixels ++;
      }
    }
  }

  int newR = (int)(totalR / numPixels);
  int newG = (int)(totalG / numPixels);
  int newB = (int)(totalB / numPixels);

  dataOut[(y*(*width) + x)*3 + 0] = newR;
  dataOut[(y*(*width) + x)*3 + 1] = newG;
  dataOut[(y*(*width) + x)*3 + 2] = newB;

  
}

int main(int argc, char** argv) {
  /* Process command line arguments */
  int radius;
  Image *imgIn, *imgOut;

  // Remove the filename from the argv list
  argv ++;
  argc --;

  if (argc < 3) {
      fprintf(stderr, "Invalid number of arguments. Usage:\n%s", usage);
      return 1;
  }

  radius = atoi(argv[0]);

  if (radius == 0) {
    fprintf(stderr, "No need to blur - radius is 0\n\n\n");
    return 1;
  }

  int width, height, size;
  imgIn = ImageRead(argv[1]);
  width = ImageWidth(imgIn);
  height = ImageHeight(imgIn);
  size = width * height * 3;
  printf("size: %d \n", size);

  imgOut = ImageCreate(width, height);

  // Device Vars 
  unsigned char *dev_dataIn, *dev_dataOut;
  int *dev_radius, *dev_width, *dev_height;
  cudaMalloc((void**) &dev_dataIn,  size);
  cudaMalloc((void**) &dev_dataOut, size);
  cudaMalloc((void**) &dev_radius,  sizeof(int));
  cudaMalloc((void**) &dev_width,   sizeof(int));
  cudaMalloc((void**) &dev_height,   sizeof(int));

  cudaMemcpy(dev_dataIn,  imgIn->data, size,  cudaMemcpyHostToDevice );
  cudaMemcpy(dev_radius,  &radius, sizeof(int),  cudaMemcpyHostToDevice );
  cudaMemcpy(dev_width,  &width, sizeof(int),  cudaMemcpyHostToDevice );
  cudaMemcpy(dev_height,  &height, sizeof(int),  cudaMemcpyHostToDevice );

  blurPPM<<< (width*height)/1024  , 1024 >>>(dev_dataIn, dev_dataOut, dev_radius, dev_width, dev_height);

  cudaMemcpy(imgOut->data, dev_dataOut, size, cudaMemcpyDeviceToHost );
  
  ImageWrite(imgOut, argv[2]);

  cudaFree(dev_dataIn);
  cudaFree(dev_dataOut);
  cudaFree(dev_radius);
  cudaFree(dev_width);
  cudaFree(dev_height);
  
}


