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

int limit_pixel(int value) {
   if (value > 255) return 255;
   else if(value < 0) return 0;
   else return value;
}

/*invert*/
void invert(int* image_host, int* image_inverse_host, int* image_device, int* image_inverse_device, int nx, int ny, int elems){
   
   for (int k = 0; k < 4; k++) {
      acc_map_data(&image_host[k * elems], image_device, elems*sizeof(int));
      acc_map_data(&image_inverse_host[k * elems], image_inverse_device, elems*sizeof(int));
      
      // TODO: do updates
      #pragma acc parallel loop present
      for (int j = 0; j < elems ; j++) {
         /*image_inverse_device[p] = 255 - image_device[p];
         image_inverse_device[p] = limit_pixel(image_device[p]);*/
         image_inverse_device[j] = 1; // FIXME: use host variable instead
         image_device[j] = 1;
      }
      // TODO: do updates

      acc_unmap_data( &image_host[k * elems] );
      acc_unmap_data(&image_inverse_host[k * elems] );
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

   /* Allocate pointers */
   // CPU allocation
   int*   image_host = (int *) malloc(sizeof(int)*nx*ny); 
   int*   image_inverse_host  = (int *) malloc(sizeof(int)*nx*ny);  

   // GPU allocation
   int elems = (nx*ny) / 4; // quarter of the image
   int size = sizeof(int) * elems; 
   int*   image_device = (int *) acc_malloc(size); 
   int*   image_inverse_device  = (int *) acc_malloc(size);  
   

   /* Read image and save in array imgage */
   readimg(filename,nx,ny,image_host);

   /* Apply filters */
   double t1 = omp_get_wtime();
   invert(image_host, image_inverse_host, image_device, image_inverse_device, nx, ny, elems);
   
   double t2 = omp_get_wtime();
   double runtime = t2 - t1;


   printf("Total time: %f\n",runtime);

   /* Save images */
   char fileout[255]={0};
   sprintf(fileout, "%s-inverse.txt", argv[1]);
   saveimg(fileout,nx,ny,image_inverse_host);
  
   /* Deallocate  */
   // GPU deallocation
   acc_free(image_device);
   acc_free(image_inverse_device);

   // CPU deallocation
   free(image_host);
   free(image_inverse_host);
}
