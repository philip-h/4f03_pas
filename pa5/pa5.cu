#include <stdio.h>
#include <cuda.h>
#include <stdlib.h>
#include <string.h>
#include "ppmFile.cuh"

/* Globals */
// const int MAX_STRING = 100;
// const int RED = 0;
// const int GREEN = 1;
// const int BLUE = 2;

const char usage [] = "./pa5.x r <inputFilename>.ppm <outputFilename>.ppm\n"
      "where:\n"
      "r is the bulrr radians in pixels\n"
      "<inputFilename>.ppm is the name of the file to blur\n"
      "<outputFilename>.ppm is the name of the file to save the blurred image\n\n\n";

__global__ void blurPPM(unsigned char *dataIn, unsigned char *dataOut) {
  dataOut[threadIdx.x] = dataIn[threadIdx.x];
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

  imgOut = ImageCreate(width, height);

  // Device Vars 
  unsigned char *dev_dataIn, *dev_dataOut;
  cudaMalloc((void**) &dev_dataIn,  size);
  cudaMalloc((void**) &dev_dataOut, size);

  cudaMemcpy(dev_dataIn,  imgIn->data, size,  cudaMemcpyHostToDevice );

  blurPPM<<< size , 1 >>>(dev_dataIn, dev_dataOut);

  cudaMemcpy(imgOut->data, dev_dataOut, size, cudaMemcpyDeviceToHost );
  
  ImageWrite(imgOut, argv[2]);
  
}


