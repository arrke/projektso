#include "projectfunctions.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <syslog.h>
#include <signal.h>
int flag = 0;
void handler(int signum){
   if(signum == SIGUSR1)
   {
     printf("signal!\n");
     flag = 1;
   }

}

int main(int argc, char **argv){
  if (signal(SIGUSR1, handler) == SIG_ERR){
    perror("Error with signal: ");
  }
  char *source = NULL;
  char *destination = NULL;
  char *timeTmp = NULL;
  char *sizeTmp = NULL;
  int programTime = 0;
  int programSize = 0;
  int recursion = 0;
  int c;
  opterr = 0;
  while ((c = getopt (argc, argv, "rs:d:t:z:")) != -1)
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
            exit(EXIT_FAILURE);
            break;
          case 'd':
            fprintf (stderr, "Option -%c requires an argument.\n", optopt);
            exit(EXIT_FAILURE);
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
      if(timeTmp == NULL){
        programTime = 300;
      }
      else{
        programTime = atoi (timeTmp);
      }

      if(timeTmp == NULL){
        programSize = 20;
      }
      else {
        programSize = atoi (sizeTmp);
      }

      int fs, fd;
      if(strcmp(source,destination) == 0){
          fprintf(stderr,"Source and destination directory are the same.\n");
          exit(EXIT_FAILURE);
      }
      fs = open (source, O_DIRECTORY, S_IRWXU | S_IRWXG | S_IRWXO);
      if (fs == -1) {
        perror("Error with source directory:");
        exit(EXIT_FAILURE);
      }

      fd = open (destination, O_DIRECTORY, S_IRWXU | S_IRWXG | S_IRWXO);
      if (fd == -1) {
        perror("Error with destination directory:");
        exit(EXIT_FAILURE);
      }
      close(fd);
      close(fs);
      // Po podaniu prawidłowych danych przez użytkownika program zamienia się w demona
      pid_t pid, sid;

      pid = fork();
      if (pid < 0) {
          exit(EXIT_FAILURE);
      }

      if (pid > 0) {
          exit(EXIT_SUCCESS);
      }

      umask(0);

      sid = setsid();
      if (sid < 0) {
              exit(EXIT_FAILURE);
      }

      if ((chdir("/")) < 0) {
              exit(EXIT_FAILURE);
      }

      while (1) {
          sleep(programTime);
          fprintf(stdout,"Wybudzenie demona\n\n");
          browsingTheDirectories(source, destination, recursion, programSize);

      }

      exit(EXIT_SUCCESS);
}
