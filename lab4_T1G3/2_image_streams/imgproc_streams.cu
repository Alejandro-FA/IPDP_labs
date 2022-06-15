/*
 *     
 *  IMAGE PROCESSING
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "cuda.h"

#define pixel(i, j, n)  (((j)*(n)) +(i))


/*read*/
void  readimg(char * filename,int nx, int ny, int * image){
  
   FILE *fp=NULL;

   fp = fopen(filename,"r");
   for(int j=0; j<ny; ++j){
      for(int i=0; i<nx; ++i){
         fscanf(fp,"%d", &image[pixel(i,j,nx)]);      
      }
   }
   fclose(fp);
}

/* save */   
void saveimg(char *filename,int nx,int ny,int *image){

   FILE *fp=NULL;
   fp = fopen(filename,"w");
   for(int j=0; j<ny; ++j){
      for(int i=0; i<nx; ++i){
         fprintf(fp,"%d ", image[pixel(i,j,nx)]);      
      }
      fprintf(fp,"\n");
   }
   fclose(fp);
}

// invert
__global__ void invert(int* image, int* image_invert, int nx, int ny) {
   int i = threadIdx.x + blockIdx.x * blockDim.x;
   int j = threadIdx.y + blockIdx.y * blockDim.y;

   if (i < nx && j < ny) {
      int p = pixel(i,j,nx);
      image_invert[p] = 255 - image[p];
   }
}


// smooth
__global__ void smooth(int* image, int* image_smooth, int nx, int ny){
   int i = threadIdx.x + blockIdx.x * blockDim.x;
   int j = threadIdx.y + blockIdx.y * blockDim.y;

   if (i < nx && j < ny) {
      if (i == 0 || i == nx-1 || j == 0 || j == ny-1) image_smooth[pixel(i,j,nx)] = 0;
      else {
         image_smooth[pixel(i,j,nx)] = (
            image[pixel(i-1,j+1,nx)] + 
            image[pixel(i,j+1,nx)] +
            image[pixel(i+1,j+1,nx)] +
            image[pixel(i-1,j,nx)] +
            image[pixel(i,j,nx)] +
            image[pixel(i+1,j,nx)]+
            image[pixel(i-1,j-1,nx)] +
            image[pixel(i,j-1,nx)] +
            image[pixel(i+1,j-1,nx)] ) / 9;

         // Ensure that the pixel value is between 0 and 255 
         image_smooth[pixel(i,j,nx)] = image_smooth[pixel(i,j,nx)] < 0 ? 0 : image_smooth[pixel(i,j,nx)] > 255 ? 255 : image_smooth[pixel(i,j,nx)];
      }
   }
}

// detect
__global__ void detect(int* image, int* image_detect, int nx, int ny){
   int i = threadIdx.x + blockIdx.x * blockDim.x;
   int j = threadIdx.y + blockIdx.y * blockDim.y;
   
   if (i < nx && j < ny) {
      if (i == 0 || i == nx-1 || j == 0 || j == ny-1) image_detect[pixel(i,j,nx)] = 0;
      else {
         image_detect[pixel(i,j,nx)] =
            image[pixel(i-1,j,nx)] +
            image[pixel(i+1,j,nx)] +
            image[pixel(i,j-1,nx)] +
            image[pixel(i,j+1,nx)] -
            4 * image[pixel(i,j,nx)];

         // Ensure that the pixel value is between 0 and 255 
         image_detect[pixel(i,j,nx)] = image_detect[pixel(i,j,nx)] < 0 ? 0 : image_detect[pixel(i,j,nx)] > 255 ? 255 : image_detect[pixel(i,j,nx)];
      }
   }
}

// enhance
__global__ void enhance(int* image,int *image_enhance,int nx, int ny){
   int i = threadIdx.x + blockIdx.x * blockDim.x;
   int j = threadIdx.y + blockIdx.y * blockDim.y;
   
   if (i < nx && j < ny) {
      if (i == 0 || i == nx-1 || j == 0 || j == ny-1) image_enhance[pixel(i,j,nx)] = 0;
      else {
         image_enhance[pixel(i,j,nx)] = 5 * image[pixel(i,j,nx)] - (
            image[pixel(i-1,j,nx)] +
            image[pixel(i+1,j,nx)] +
            image[pixel(i,j-1,nx)] +
            image[pixel(i,j+1,nx)] );

         // Ensure that the pixel value is between 0 and 255 
         image_enhance[pixel(i,j,nx)] = image_enhance[pixel(i,j,nx)] < 0 ? 0 : image_enhance[pixel(i,j,nx)] > 255 ? 255 : image_enhance[pixel(i,j,nx)];  
      }
   }
}

/* Main program */
int main (int argc, char *argv[])
{
   int    nx,ny;
   char   filename[250];

   /* Get parameters */
   if (argc != 4) 
   {
      printf ("Usage: %s image_name N M \n", argv[0]);
      exit (1);
   }
   sprintf(filename, "%s.txt", argv[1]);
   nx  = atoi(argv[2]);
   ny  = atoi(argv[3]);

   printf("%s %d %d\n", filename, nx, ny);

   /* Allocate CPU pointers */
   int   *h_image, *d_image; 
   int   *h_image_invert, *d_image_invert;
   int   *h_image_smooth, *d_image_smooth;
   int   *h_image_detect, *d_image_detect;
   int   *h_image_enhance, *d_image_enhance;

   cudaMallocHost((void**)&h_image, sizeof(int)*nx*ny);
   cudaMallocHost((void**)&h_image_invert, sizeof(int)*nx*ny);
   cudaMallocHost((void**)&h_image_smooth, sizeof(int)*nx*ny);
   cudaMallocHost((void**)&h_image_detect, sizeof(int)*nx*ny);
   cudaMallocHost((void**)&h_image_enhance, sizeof(int)*nx*ny);
   
   /* Read image and save in array imgage */
   readimg(filename,nx,ny,h_image);

   /* Decide Block and Grid dimensions */
   int B = 16; // Number of blocks (maximum number of blocks that can be simultaneously active)
   dim3 dimBlock(B, B, 1);
   int dimgx = (nx+B-1)/B;
   int dimgy = (ny+B-1)/B;
   dim3 dimGrid(dimgx, dimgy, 1);

   float runtime;
   cudaEvent_t start, stop;
   cudaEventCreate(&start);
   cudaEventCreate(&stop);

   /************************************ Start recording ************************************/
   cudaEventRecord(start);

   /* Create Streams */
   cudaStream_t stream1, stream2, stream3, stream4;
   cudaStreamCreate(&stream1);
   cudaStreamCreate(&stream2);
   cudaStreamCreate(&stream3);
   cudaStreamCreate(&stream4);

   /* Allocate GPU pointers */
   cudaMallocHost((void**)&d_image, sizeof(int)*nx*ny);
   cudaMallocHost((void**)&d_image_invert, sizeof(int)*nx*ny);
   cudaMallocHost((void**)&d_image_smooth, sizeof(int)*nx*ny);
   cudaMallocHost((void**)&d_image_detect, sizeof(int)*nx*ny);
   cudaMallocHost((void**)&d_image_enhance, sizeof(int)*nx*ny);

   /* Copy image to GPU */
   cudaMemcpy(d_image, h_image, sizeof(int)*nx*ny, cudaMemcpyHostToDevice);  // Sync version since we need it in all streams

   /* Filters */
   invert<<<dimGrid, dimBlock, 0, stream1>>>(d_image, d_image_invert, nx, ny);
   smooth<<<dimGrid, dimBlock, 0, stream2>>>(d_image, d_image_smooth, nx, ny);
   detect<<<dimGrid, dimBlock, 0, stream3>>>(d_image, d_image_detect, nx, ny);
   enhance<<<dimGrid, dimBlock, 0, stream4>>>(d_image, d_image_enhance, nx, ny);
   
   /* Image transfer */
   cudaMemcpyAsync(h_image_invert, d_image_invert, sizeof(int)*nx*ny, cudaMemcpyDeviceToHost, stream1); 
   cudaMemcpyAsync(h_image_smooth, d_image_smooth, sizeof(int)*nx*ny, cudaMemcpyDeviceToHost, stream2);
   cudaMemcpyAsync(h_image_detect, d_image_detect, sizeof(int)*nx*ny, cudaMemcpyDeviceToHost, stream3);
   cudaMemcpyAsync(h_image_enhance, d_image_enhance, sizeof(int)*nx*ny, cudaMemcpyDeviceToHost, stream4);
   
   /* Delete streams */
   cudaStreamDestroy(stream1);
   cudaStreamDestroy(stream2);
   cudaStreamDestroy(stream3);
   cudaStreamDestroy(stream4);

   cudaEventRecord(stop);
   cudaEventSynchronize(stop);
   cudaEventElapsedTime(&runtime, start, stop);
   /************************************* End recording *************************************/

   printf("It took %f ms to apply all the filters and to manage all the data. \n", runtime);

   /* Save images */
   char fileout[255]={0};
   sprintf(fileout, "%s-inverse.txt", argv[1]);
   saveimg(fileout,nx,ny,h_image_invert);
   sprintf(fileout, "%s-smooth.txt", argv[1]);
   saveimg(fileout,nx,ny,h_image_smooth);
   sprintf(fileout, "%s-detect.txt", argv[1]);
   saveimg(fileout,nx,ny,h_image_detect);
   sprintf(fileout, "%s-enhance.txt", argv[1]);
   saveimg(fileout,nx,ny,h_image_enhance);

   /* Deallocate GPU pointers */
   cudaFree(d_image);
   cudaFree(d_image_invert);
   cudaFree(d_image_smooth);
   cudaFree(d_image_detect);
   cudaFree(d_image_enhance);

   /* Deallocate CPU pointers*/
   cudaFreeHost(h_image);
   cudaFreeHost(h_image_invert);
   cudaFreeHost(h_image_smooth);
   cudaFreeHost(h_image_detect);
   cudaFreeHost(h_image_enhance);
}
