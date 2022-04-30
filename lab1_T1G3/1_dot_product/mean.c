#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

int read_split(int fin, char * buff, int maxlen, char * ch_end) {
  int i;
  char * is_split = 0;
  for (i = 0; i < maxlen && !is_split && read(fin, ch_end, 1); i++) {
    is_split = strchr(" \n\t;&", *ch_end); //c is one of ' ' or '\n' or '\t' or '&'
    if (!is_split) buff[i] = *ch_end;
  }
  if (is_split) buff[i - 1] = '\0';
  return i;
}

void clearBuffer(char * buffer){
    for (int i = 0; i < 100; i++){
        buffer[i] = 0;
    }
}

void readTimes(int * fd, int N_executions){
    char* buff = malloc(sizeof(char)*100);
    char ch_end = '\t';
    
    while(ch_end != '\n'){ // Printing the execution type
        read_split(*fd, buff, sizeof(char)*100, &ch_end);
        for(int i = 0; i<100; i++)
            printf("%c", buff[i]);
        printf(" ");
        clearBuffer(buff);
    }
    printf("\n");
    clearBuffer(buff);

    ch_end = '\t';
    double times[N_executions];
    
    for(int i = 0; i < N_executions; i++){ // We read all the values from the file and store them in `times`
        
        read_split(*fd, buff, sizeof(char)*10, &ch_end);
        for(int j = 0; j<100; j++)
            printf("%c", buff[j]);

        printf("\n");
        times[i] = atof(buff);

        while(ch_end != '\n')
            read_split(*fd, buff, sizeof(char)*100, &ch_end);
        clearBuffer(buff);
    }

    double sum = 0;
    for(int i = 0; i < N_executions; i++){
        sum += times[i];
    }

    double mean = sum / N_executions;
    printf("Mean: %f \n", mean);

    ch_end = '\t';
    while(ch_end != '\n') // We skip the division line
        read_split(*fd, buff, sizeof(char)*100, &ch_end);

    free(buff);

    clearBuffer(buff);
}


int main(int argc, char const *argv[])
{
    int fd = open("out_dotp.out", O_RDWR, 0644);

    int N_exercises = 9;
    int N_executions = 5;

    for(int i = 0; i < N_exercises; i++)
        readTimes(&fd, N_executions );

    close(fd);
    return 0;
}
