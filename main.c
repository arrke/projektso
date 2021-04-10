#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include "projectfunctions.h"

int main(int argc, char **argv){
  char *source = NULL;
  char *destination = NULL;
  char *timeTmp = NULL;
  char *sizeTmp = NULL;
  int programTime = 300;
  int programSize = 20;
  int c;
  int recursion = 0;
  opterr = 0;

  while ((c = getopt (argc, argv, "s:d:t:z:r")) != -1)
    switch (c)
      {
      case 's':
        source = optarg;
        break;
      case 'd':
        destination = optarg;
        break;
      case 't':
        timeTmp = optarg;
        break;
      case 'z':
        sizeTmp = optarg;
        break;
      case 'r':
        recursion = 1;
        break;
      case '?':
        switch(optopt)
        {
          case 's':
            fprintf (stderr, "Option -%c requires an argument.\n", optopt);
            break;
          case 'd':
            fprintf (stderr, "Option -%c requires an argument.\n", optopt);
            break;
        }
        if (isprint (optopt))
          fprintf (stderr, "Unknown option `-%c'.\n", optopt);
        else
          fprintf (stderr,"Unknown option character `\\x%x'.\n",optopt);
        return 1;
      default:
        abort ();
      }
      if(ifnumeric(timeTmp)){
        programTime = atoi(timeTmp);
      }
      else{
        fprintf(stderr,"Given time argument is string. Set default time.\n");
      }
      if(ifnumeric(sizeTmp)){
        programTime = atoi(sizeTmp);
      }
      else{
        fprintf(stderr,"Given size argument is string. Set default size.\n");
      }

      int fs, fd;
      if(strcmp(source,destination) == 0){
          fprintf(stderr,"Source and destination directory are the same.\n");
          exit(EXIT_FAILURE);
      }
      fs = open (source, O_DIRECTORY, 666);
      if(fs == -1){
        perror("Error with source directory:");
        exit(EXIT_FAILURE);
      }

      fd = open (destination, O_DIRECTORY, 666);
      if(fd == -1){
        perror("Error with destination directory:");
        exit(EXIT_FAILURE);
      }


      exit(EXIT_SUCCESS);
}
