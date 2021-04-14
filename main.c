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
#include <syslog.h>
#include <limits.h>
typedef void (*sighandler_t)(int);
volatile int flag = 0;

void handler(int signum){
   if(signum == SIGUSR1)
   {
     syslog ( LOG_NOTICE, "Wybudzenie demona poprzez sygnał SIGUSR1.");
     flag = 1;
   }
}

int main(int argc, char **argv){
  setlogmask ( LOG_UPTO (LOG_NOTICE) );

  openlog ( "PROJEKT: Demon synchronizujący", LOG_USER, LOG_LOCAL1 );
  if (signal(SIGUSR1, handler) == SIG_ERR){
    perror("Error with signal: ");
  }
  syslog ( LOG_NOTICE, "Program rozpoczety" );
  char *source = NULL;
  char *destination = NULL;
  int programTime = 20;
  int programSize = INT_MAX;
  int recursion = 0;
  int c;
  opterr = 0;
  while ((c = getopt (argc, argv, "Rs:d:T:S:h")) != -1)
    switch (c)
      {
      case 'h':
        man_projekt_program();
        exit(EXIT_FAILURE);
        break;
      break;
      case 's':
        source = optarg;
        break;
      case 'd':
        destination = optarg;
        break;
      case 'T':
        programTime = atoi (optarg);
        break;
      case 'S':
        programSize = atoi (optarg);
        break;
      case 'R':
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

      for( int i = 0 ; i < 1024*1024; i++){
        close (i);
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
      syslog ( LOG_NOTICE, "Zamiana w demona." );
      while (1) {
          syslog ( LOG_NOTICE, "Uśpienie demona." );
          int i=0;
          while(i<=programTime && !flag){
            sleep(1);
            i++;
          }
          if (!flag){
            syslog ( LOG_NOTICE, "Wybudzenie demona po %d s.", programTime);
          }
          browsingTheDirectories(source, destination, recursion, programSize);
          if(flag) flag = 0;
      }

      exit(EXIT_SUCCESS);
}
