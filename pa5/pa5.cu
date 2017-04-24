/*
 * Authors: Philip Habib
 *          Theo Stone
 * Date: April 25, 2017
 *
 * This program will blur a given image in ppm format with a given blur radius. It uses
 * the CUDA framework to blur the image with the GPU, giving us access to many threads.
 * The bulrPPM function will be called from the 'host' (CPU) and will run on the 
 * 'device' (GPU).
 *
 * Blur funciton is quite simple - the color of a pixel is determined by averaging the colors 
 * of all pixels around it, blur radius pixels from the current pixel.
 */

#include <stdio.h>
#include <cuda.h>
#include <stdlib.h>
#include <string.h>
#include "ppmFile.cuh"

const char usage [] = "./pa5.x r <inputFilename>.ppm <outputFilename>.ppm\n"
      "where:\n"
      "r is the bulrr radians in pixels\n"
      "<inputFilename>.ppm is the name of the file to blur\n"
      "<outputFilename>.ppm is the name of the file to save the blurred image\n\n\n";

/*
 * blurPPM implements a simple blur algorithm: color of pixel is the average of all pixels around this pixel.
 * This function is called from the CPU and is run on the GPU (this is what the __global__ decorator means.
 * The blurPPM function itself only blurs one pixel [@index]. Will be run in parallel to eventually blur every pixel in an image.
 *
 * Input
 * unsigned char *dataIn - pointer to data array for Image struct of the input image
 * int *radius - blur radius. Defines how many pixels away from the current pixel we need to calculate the color of the current pixel.
 * int *width - The width of the image. Used to determine the (x,y) coordinate of the image
 * int *height - The height of the image. Also used to determine the (x,y) coordinate of the image
 *
 * Output
 * unsigned char *dataOut - pointer to data array for Image struct of the output image
 */
__global__ void blurPPM(unsigned char *dataIn, unsigned char *dataOut, int *radius, int *width, int *height) {
  int index = threadIdx.x + blockIdx.x * blockDim.x;
  int totalR = 0, totalG = 0, totalB = 0, numPixels = 0;

  // x,y of current position of the pixel given by index
  int x = (int)(index % (*width) );
  int y = (int)(index / (*width) );


  /* **************** Average Pixel **************** */
  /* Calculates the color of the current pixel */
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
        int offset = (bY*(*width) + bX)*3;
        /*  
          0 - RED channel
          1 - GREEN channel
          2 - BLUE channel
        */
        totalR += dataIn[offset + 0];
        totalG += dataIn[offset + 1];
        totalB += dataIn[offset + 2];
        numPixels ++;
      }
    }
  }

  int newR = (int)(totalR / numPixels);
  int newG = (int)(totalG / numPixels);
  int newB = (int)(totalB / numPixels);
  /* ********************************* */

  /*  
    0 - RED channel
    1 - GREEN channel
    2 - BLUE channel
  */

  int offset = (y*(*width) + x)*3;
  dataOut[offest + 0] = newR;
  dataOut[offset + 1] = newG;
  dataOut[offset + 2] = newB;

  
}
/*
 * The main function. Handles command line arguments and represents the host program.
 *
 * Input:
 * int argc - Number of command line arguments
 * char** argv - The command line arguments 
 */
int main(int argc, char** argv) {
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

  // Timing with cuda events
  cudaEvent_t start, stop;
  cudaEventCreate(&start);
  cudaEventCreate(&stop);


  // Initialize device vars 
  unsigned char *dev_dataIn, *dev_dataOut;
  int *dev_radius, *dev_width, *dev_height;
  cudaMalloc((void**) &dev_dataIn,  size);
  cudaMalloc((void**) &dev_dataOut, size);
  cudaMalloc((void**) &dev_radius,  sizeof(int));
  cudaMalloc((void**) &dev_width,   sizeof(int));
  cudaMalloc((void**) &dev_height,   sizeof(int));

  // Copy device vars to GPU
  cudaMemcpy(dev_dataIn,  imgIn->data, size,  cudaMemcpyHostToDevice );
  cudaMemcpy(dev_radius,  &radius, sizeof(int),  cudaMemcpyHostToDevice );
  cudaMemcpy(dev_width,  &width, sizeof(int),  cudaMemcpyHostToDevice );
  cudaMemcpy(dev_height,  &height, sizeof(int),  cudaMemcpyHostToDevice );

  cudaEventRecord(start);
  blurPPM<<< (width*height)/1024  , 1024 >>>(dev_dataIn, dev_dataOut, dev_radius, dev_width, dev_height);
  cudaEventRecord(stop);

  // Copy output from blurPPM to local data array from Image struct
  cudaMemcpy(imgOut->data, dev_dataOut, size, cudaMemcpyDeviceToHost );

  ImageWrite(imgOut, argv[2]);

  // Calculate the time it took blurPPM to run
  cudaEventSynchronize(stop);
  float milis = 0;
  cudaEventElapsedTime(&milis, start, stop);
  printf("Elapsed time %f \n", milis);

  cudaFree(dev_dataIn);
  cudaFree(dev_dataOut);
  cudaFree(dev_radius);
  cudaFree(dev_width);
  cudaFree(dev_height);
  
}


