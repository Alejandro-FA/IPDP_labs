/*
 *     
 *  IMAGE PROCESSING
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>

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
void clear_border(int* image, int nx, int ny){
   // Clearing both horizontal axis
   #pragma acc parallel loop copyout(image[0:nx*ny])
   for(int i = 0;  i < nx; i++) image[pixel(i,0,nx)] = 0; 
   #pragma acc parallel loop copyout(image[0:nx*ny])
   for(int i = 0;  i < nx; i++) image[pixel(i,ny-1,nx)] = 0; 
   
   // Clearing both vertical axis
   #pragma acc parallel loop copyout(image[0:nx*ny])
   for(int j = 1;  j < ny-1; j++) image[pixel(0,j,nx)] = 0;
   #pragma acc parallel loop copyout(image[0:nx*ny])
   for(int j = 1;  j < ny-1; j++) image[pixel(nx-1,j,nx)] = 0;
}

int limit_pixel(int value) {
   if (value > 255) return 255;
   else if(value < 0) return 0;
   else return value;
}

/*invert*/
void invert(int* image, int* image_invert, int nx, int ny){
   
   #pragma acc parallel loop collapse(2) copyin(image[0:nx*ny]) copyout(image_invert[0:nx*ny]) async(1)
   for (int j = 0; j < ny ; j++) {
      for (int i = 0; i < nx ; i++) {
         int p = pixel(i,j,nx);
         image_invert[p] = 255 - image[p];
         image_invert[p] = limit_pixel(image_invert[p]);
      }
   }
   // TODO: use update
}

/*smooth*/
void smooth(int* image, int* image_smooth, int nx, int ny){
   
   #pragma acc parallel loop collapse(2) copyin(image[0:nx*ny]) async(2)
   for (int j = 1; j < ny-1 ; j++) {
      for (int i = 1; i < nx-1 ; i++) {
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
         
         image_smooth[pixel(i,j,nx)] = limit_pixel(image_smooth[pixel(i,j,nx)]);
      }
   }
   clear_border(image_smooth, nx, ny);
   // TODO: use update
   
}

/*detect*/
void detect(int* image, int* image_detect, int nx, int ny){
  
   #pragma acc parallel loop collapse(2) copyin(image[0:nx*ny]) async(3)
   for(int j = 1; j < ny-1 ; j++) {
      for(int i = 1; i < nx-1 ; i++) {
         image_detect[pixel(i,j,nx)] =
            image[pixel(i-1,j,nx)] +
            image[pixel(i+1,j,nx)] +
            image[pixel(i,j-1,nx)] +
            image[pixel(i,j+1,nx)] -
            4 * image[pixel(i,j,nx)];

         image_detect[pixel(i,j,nx)] = limit_pixel(image_detect[pixel(i,j,nx)]);
      }
   }
   clear_border(image_detect, nx, ny);
   // TODO: use update
}

/*enhance*/
void enhance(int* image,int *image_enhance,int nx, int ny){
  
   #pragma acc parallel loop collapse(2) copyin(image[0:nx*ny]) async(4)
   for (int j = 1; j < ny-1 ; j++) {
      for (int i = 1; i < nx-1 ; i++) {
         image_enhance[pixel(i,j,nx)] = 5 * image[pixel(i,j,nx)] - (
            image[pixel(i-1,j,nx)] +
            image[pixel(i+1,j,nx)] +
            image[pixel(i,j-1,nx)] +
            image[pixel(i,j+1,nx)] );

         image_enhance[pixel(i,j,nx)] = limit_pixel(image_enhance[pixel(i,j,nx)]);
      }
   }
   clear_border(image_enhance, nx, ny);
   // TODO: use update
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
   int*   image = (int *) malloc(sizeof(int)*nx*ny); 
   int*   image_invert  = (int *) malloc(sizeof(int)*nx*ny);  
   int*   image_smooth  = (int *) malloc(sizeof(int)*nx*ny);  
   int*   image_detect  = (int *) malloc(sizeof(int)*nx*ny);  
   int*   image_enhance = (int *) malloc(sizeof(int)*nx*ny); 
   

   /* Read image and save in array imgage */
   readimg(filename,nx,ny,image);

   /* Apply filters */
   double t1 = omp_get_wtime();
   #pragma acc enter data copyin(image[0:nx*ny]) create(image_invert[0:nx*ny], image_invert[0:nx*ny], image_smooth[0:nx*ny], image_detect[0:nx*ny], image_enhance[0:nx*ny])
   // #pragma acc data copyin(image[0:nx*ny]) copyout(image_invert[0:nx*ny], image_invert[0:nx*ny], image_smooth[0:nx*ny], image_detect[0:nx*ny], image_enhance[0:nx*ny])
   {      
      invert(image, image_invert, nx, ny);
      smooth(image, image_smooth, nx, ny);
      detect(image, image_detect, nx, ny);
      enhance(image, image_enhance, nx, ny);
      #pragma acc wait
   }
   #pragma acc exit data delete(image[0:nx*ny], image_invert[0:nx*ny], image_invert[0:nx*ny], image_smooth[0:nx*ny], image_detect[0:nx*ny], image_enhance[0:nx*ny])

   double t2 = omp_get_wtime();
   double runtime = t2 - t1;


   printf("Total time: %f\n",runtime);

   /* Save images */
   char fileout[255]={0};
   sprintf(fileout, "%s-inverse.txt", argv[1]);
   saveimg(fileout,nx,ny,image_invert);
   sprintf(fileout, "%s-smooth.txt", argv[1]);
   saveimg(fileout,nx,ny,image_smooth);
   sprintf(fileout, "%s-detect.txt", argv[1]);
   saveimg(fileout,nx,ny,image_detect);
   sprintf(fileout, "%s-enhance.txt", argv[1]);
   saveimg(fileout,nx,ny,image_enhance);

   /* Deallocate  */

   free(image);
   free(image_invert);
   free(image_smooth);
   free(image_detect);
   free(image_enhance);

}
