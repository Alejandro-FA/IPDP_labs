#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

// int read_split(int fin, char * buff, int maxlen, char * ch_end) {
//   int i;
//   char * is_split = 0;
//   for (i = 0; i < maxlen && !is_split && read(fin, ch_end, 1); i++) {
//     is_split = strchr(" \n\t;&", *ch_end); //c is one of ' ' or '\n' or '\t' or '&'
//     if (!is_split) buff[i] = *ch_end;
//   }
//   if (is_split) buff[i - 1] = '\0';
//   return i;
// }

void print_file(int * fd){
  double * buff = malloc(sizeof(double));
  
  while (fread (buff, sizeof(double), sizeof(double), fd))
    printf ("%lf\n", buff[0]);

  printf("\n");

  return 0;
  
}

int main(){

  char file1_name[100] = {"/shared/Labs/Lab_2/matrix.bin\0"};
  char file2_name[100] = {"/shared/Labs/Lab_2/matrix_vector.bin\0"};

  int fd1 = fopen(file1_name, "r");
  int fd2 = fopen(file2_name, "r");

  print_file(&fd1);
  print_file(&fd2);

  return 0;
}