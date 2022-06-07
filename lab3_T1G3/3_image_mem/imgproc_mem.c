/*
 *     
 *  IMAGE PROCESSING
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>
#include <openacc.h>

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

//////////* OUR OWN FUNCTIONS *////////////
/*invert*/
void invert(int* image_host, int* image_inverse_host, int nx, int ny) {
   // GPU allocation
   int partitions = 4;
   int elems = (nx*ny) / partitions;
   int size = sizeof(int) * elems; 
   int*   image_device = (int *) acc_malloc(size); 
   int*   image_inverse_device  = (int *) acc_malloc(size);  

   // Filtering process divided in 4 sections
   for (int k = 0; k < partitions; k++) {
      acc_map_data(&image_host[k * elems], image_device, elems*sizeof(int));
      acc_map_data(&image_inverse_host[k * elems], image_inverse_device, elems*sizeof(int));
      
      #pragma acc update device (image_host[k*elems: elems], image_inverse_host[k*elems: elems])
      #pragma acc parallel loop present(image_host[k*elems: elems], image_inverse_host[k*elems: elems])
      for (int j = k*elems; j < (k+1)*elems ; j++)
         image_inverse_host[j] = 255 - image_host[j];
      #pragma acc update host (image_host[k*elems: elems], image_inverse_host[k*elems: elems])

      acc_unmap_data( &image_host[k * elems] );
      acc_unmap_data(&image_inverse_host[k * elems] );
   }

   // GPU deallocation
   acc_free(image_device);
   acc_free(image_inverse_device);
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

   // CPU allocation
   int*   image = (int *) malloc(sizeof(int)*nx*ny); 
   int*   image_inverse  = (int *) malloc(sizeof(int)*nx*ny);  
   
   /* Read image and save in array imgage */
   readimg(filename,nx,ny,image);

   /* Apply filters */
   double t1 = omp_get_wtime();
   invert(image, image_inverse, nx, ny);
   
   double t2 = omp_get_wtime();
   double runtime = t2 - t1;

   printf("Total time: %f\n",runtime);

   /* Save images */
   char fileout[255]={0};
   sprintf(fileout, "%s-inverse.txt", argv[1]);
   saveimg(fileout,nx,ny,image_inverse);

   // CPU deallocation
   free(image);
   free(image_inverse);
}
